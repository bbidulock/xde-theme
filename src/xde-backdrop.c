/*****************************************************************************

 Copyright (c) 2010-2018  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2002-2009  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 *****************************************************************************/

#include "xde.h"

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <sys/poll.h>
#include <sys/timerfd.h>

typedef struct {
	Window root;
	int desktop;
	int desktops;
	struct {
		int rows;
		int cols;
	} layout;

} BdScreen;

BdScreen *scr, *screens;

const char *program = NAME;

static enum { StartingUp, WaitForWM, WaitAfterWM, DetectWM, ShutDown } state;

WmMonitor *monitors;
int nmon;

#define EXTRANGE    16		/* all (used) X11 extension events must fit in this range 
				 */

enum { XrandrBase, XineramaBase, XsyncBase, BaseLast };

typedef struct {
	Bool have;
	int base;
} Extension;

Extension ext[BaseLast];

int ext_xinerama;			/* */
int ext_xrandr;				/* */

int evb_xinerama;			/* */
int evb_xrandr;				/* */

static int xfd;
static int tfd;
static struct itimerspec it = { {0, 0}, {2, 0} };

int trm_signal;
int hup_signal;
int alm_signal;

static RETSIGTYPE
sig_handler(int signum)
{
	if (signum == SIGTERM)
		trm_signal = 1;
	if (signum == SIGHUP)
		hup_signal = 1;
	if (signum == SIGALRM)
		alm_signal = 1;
	return (RETSIGTYPE) (0);
}

void
trm_action()
{
}

void
wait_after_wm()
{
	state = WaitAfterWM;
	if (timerfd_settime(tfd, 0, &it, NULL) != 0) {
		EPRINTF("could not set timer: %s\n", strerror(errno));
		exit(1);
	}
}

void
wait_for_wm()
{
	state = WaitForWM;
}

void
check_wm()
{
	if (xde_detect_wm())
		wait_after_wm();
	else
		wait_for_wm();
}

void
recheck_wm()
{
	wait_after_wm();
}

extern Atom _XA_BB_THEME;
extern Atom _XA_BLACKBOX_PID;
extern Atom _XA_DT_WORKSPACE_CURRENT;
extern Atom _XA_DT_WORKSPACE_LIST;
extern Atom _XA_ESETROOT_PMAP_ID;
extern Atom _XA_GTK_READ_RCFILES;
extern Atom _XA_I3_CONFIG_PATH;
extern Atom _XA_I3_PID;
extern Atom _XA_I3_SHMLOG_PATH;
extern Atom _XA_I3_SOCKET_PATH;
extern Atom _XA_ICEWMBG_QUIT;
extern Atom _XA_MOTIF_WM_INFO;
extern Atom _XA_NET_CURRENT_DESKTOP;
extern Atom _XA_NET_DESKTOP_LAYOUT;
extern Atom _XA_NET_DESKTOP_PIXMAPS;
extern Atom _XA_NET_NUMBER_OF_DESKTOPS;
extern Atom _XA_NET_SUPPORTED;
extern Atom _XA_NET_SUPPORTING_WM_CHECK;
extern Atom _XA_NET_VISIBLE_DESKTOPS;
extern Atom _XA_NET_WM_NAME;
extern Atom _XA_NET_WM_PID;
extern Atom _XA_OB_THEME;
extern Atom _XA_OPENBOX_PID;
extern Atom _XA_WIN_DESKTOP_BUTTON_PROXY;
extern Atom _XA_WINDOWMAKER_NOTICEBOARD;
extern Atom _XA_WIN_PROTOCOLS;
extern Atom _XA_WIN_SUPPORTING_WM_CHECK;
extern Atom _XA_WIN_WORKSPACE;
extern Atom _XA_WIN_WORKSPACE_COUNT;
extern Atom _XA_WM_COMMAND;
extern Atom _XA_WM_DESKTOP;
extern Atom _XA_XDE_THEME_NAME;
extern Atom _XA_XROOTPMAP_ID;
extern Atom _XA_XSETROOT_ID;

static void handle_BB_THEME(XEvent *);
static void handle_BLACKBOX_PID(XEvent *);
static void handle_ESETROOT_PMAP_ID(XEvent *);
static void handle_GTK_READ_RCFILES(XEvent *);
static void handle_I3_PID(XEvent *);
static void handle_ICEWMBG_QUIT(XEvent *);
static void handle_NET_CURRENT_DESKTOP(XEvent *);
static void handle_NET_DESKTOP_NAMES(XEvent *);
static void handle_NET_DESKTOP_PIXMAP(XEvent *);
static void handle_NET_NUMBER_OF_DESKTOPS(XEvent *);
static void handle_NET_SUPPORTED(XEvent *);
static void handle_NET_SUPPORTING_WM_CHECK(XEvent *);
static void handle_NET_WM_NAME(XEvent *);
static void handle_NET_WM_PID(XEvent *);
static void handle_OB_THEME(XEvent *);
static void handle_OPENBOX_PID(XEvent *);
static void
handle_WIN_DESKTOP_BUTTON_PR(XEvent *)
    OXY;
	static void handle_WIN_PROTOCOLS(XEvent *);
	static void handle_WIN_SUPPORTING_WM_CHECK(XEvent *);
	static void handle_WIN_WORKSPACE_COUNT(XEvent *);
	static void handle_WIN_WORKSPACE(XEvent *);
	static void handle_WM_CLASS(XEvent *);
	static void handle_WM_CLIENT_MACHINE(XEvent *);
	static void handle_WM_COMMAND(XEvent *);
	static void handle_WM_NAME(XEvent *);
	static void handle_XDE_THEME_NAME(XEvent *);
	static void handle_XROOTMAP_ID(XEvent *);
	static void handle_XROOTPMAP_ID(XEvent *);
	static void handle_XSETROOT_ID(XEvent *);

	typedef struct {
		char *name;
		Atom *atom;
		void (*handler) (XEvent *);
		Atom value;
	} AtomHandler;

	static AtomHandler myatoms[] = {
	/* *INDENT-OFF* **/
	{ "_BB_THEME",			&_XA_BB_THEME,			&handle_BB_THEME,			None },
	{ "_BLACKBOX_PID",		&_XA_BLACKBOX_PID,		&handle_BLACKBOX_PID,			None },
	{ "ESETROOT_PMAP_ID",		&_XA_ESETROOT_PMAP_ID,		&handle_ESETROOT_PMAP_ID,		None },
	{ "_GTK_READ_RCFILES",		&_XA_GTK_READ_RCFILES,		&handle_GTK_READ_RCFILES,		None },
	{ "I3_PID",			&_XA_I3_PID,			&handle_I3_PID,				None },
	{ "_ICEWMBG_QUIT",		&_XA_ICEWMBG_QUIT,		&handle_ICEWMBG_QUIT,			None },
	{ "_NET_CURRENT_DESKTOP",	&_XA_NET_CURRENT_DESKTOP,	&handle_NET_CURRENT_DESKTOP,		None },
	{ "_NET_DESKTOP_NAMES",		&_XA_NET_DESKTOP_NAMES,		&handle_NET_DESKTOP_NAMES,		None },
	{ "_NET_DESKTOP_PIXMAPS",	&_XA_NET_DESKTOP_PIXMAP,	&handle_NET_DESKTOP_PIXMAPS,		None },
	{ "_NET_NUMBER_OF_DESKTOPS",	&_XA_NET_NUMBER_OF_DESKTOPS,	&handle_NET_NUMBER_OF_DESKTOPS,		None },
	{ "_NET_SUPPORTED",		&_XA_NET_SUPPORTED,		&handle_NET_SUPPORTED,			None },
	{ "_NET_SUPPORTING_WM_CHECK",	&_XA_NET_SUPPORTING_WM_CHECK,	&handle_NET_SUPPORTING_WM_CHECK,	None },
	{ "_NET_WM_NAME",		&_XA_NET_WM_NAME,		&handle_NET_WM_NAME,			None },
	{ "_NET_WM_PID",		&_XA_NET_WM_PID,		&handle_NET_WM_PID,			None },
	{ "_OB_THEME",			&_XA_OB_THEME,			&handle_OB_THEME,			None },
	{ "_OPENBOX_PID",		&_XA_OPENBOX_PID,		&handle_OPENBOX_PID,			None },
	{ "_WIN_DESKTOP_BUTTON_PROXY",	&_XA_WIN_DESKTOP_BUTTON_PROXY,	&handle_WIN_DESKTOP_BUTTON_PROXY,	None },
	{ "_WIN_PROTOCOLS",		&_XA_WIN_PROTOCOLS,		&handle_WIN_PROTOCOLS,			None },
	{ "_WIN_SUPPORTING_WM_CHECK",	&_XA_WIN_SUPPORTING_WM_CHECK,	&handle_WIN_SUPPORTING_WM_CHECK,	None },
	{ "_WIN_WORKSPACE_COUNT",	&_XA_WIN_WORKSPACE_COUNT,	&handle_WIN_WORKSPACE_COUNT,		None },
	{ "_WIN_WORKSPACE",		&_XA_WIN_WORKSPACE,		&handle_WIN_WORKSPACE,			None },
	{ "WM_CLASS",			NULL,				&handle_WM_CLASS,			XA_WM_CLASS },
	{ "WM_CLIENT_MACHINE",		NULL,				&handle_WM_CLIENT_MACHINE,		XA_WM_CLIENT_MACHINE },
	{ "WM_COMMAND",			NULL,				&handle_WM_COMMAND,			XA_WM_COMMAND },
	{ "WM_NAME",			NULL,				&handle_WM_NAME,			XA_WM_NAME },
	{ "_XDE_THEME_NAME",		&_XA_XDE_THEME_NAME,		&handle_XDE_THEME_NAME,			None },
	{ "_XROOTMAP_ID",		&_XA_XROOTMAP_ID,		&handle_XROOTMAP_ID,			None },
	{ "_XROOTPMAP_ID",		&_XA_XROOTPMAP_ID,		&handle_XROOTPMAP_ID,			None },
	{ "_XSETROOT_ID",		&_XA_XSETROOT_ID,		&handle_XSETROOT_ID,			None },
	{ NULL,				NULL,				NULL,					None }
	/* *INDENT-ON* **/
	};

/** @brief handle _BB_THEME property notification
  *
  * Our blackbox(1) theme files have a rootCommand that changes the _BB_THEME
  * property on the root window.  Check the theme again when it changes.
  */
static void
handle_BB_THEME(XEvent *e)
{
	check_theme();
}

/** @brief handle _BLACKBOX_PID property notification
  *
  * When fluxbox(1) restarts, it does not change the _NET_SUPPORTING_WM_CHECK
  * window, but it does change the _BLACKBOX_PID cardinal (with our setup), even
  * if it is just to replace it with the same value.  When restarting, recheck
  * the theme.
  */
static void
handle_BLACKBOX_PID(XEvent *e)
{
	set_deferred_wmcheck();
}

/** @brief handle ESETROOT_PMAP_ID property changes
  *
  * We do not really process this because all proper root setters now set the
  * _XROOTPMAP_ID property which we handle above.  However, it is used to
  * trigger recheck of the theme needed by some window managers such as
  * blackbox(1).  If it means we check 3 times after a theme switch, so be it.
  */
static void
handle_ESETROOT_PMAP_ID(XEvent *e)
{
	// check_theme();
}

static void
handle_I3_PID(XEvent *e)
{
	set_deferred_wmcheck();
}

static void
handle_GTK_READ_RCFILES(XEvent *e)
{
	check_theme();
}

static void
handle_ICEWMBG_QUIT(XEvent *e)
{
}

/** @brief handle _NET_CURRENT_DESKTOP property change
  *
  * Handle when _NET_CURRENT_DESKTOP property changes on the root window of any
  * screen.  This is how we determine that the desktop has changed.
  */
static void
handle_NET_CURRENT_DESKTOP(XEvent *e)
{
	long which = 0;

	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	if (xde_get_cardinal(scr->root, _XA_NET_CURRENT_DESKTOP, XA_CARDINAL, &which)) {
		int newdesk = which;

		DPRINTF("new desktop %d (was %d)\n", newdesk, scr->curdesk);
		if (newdesk != scr->curdesk) {
			if (0 <= newdesk && newdesk < scr->numdesk) {
				scr->curdesk = newdesk;
				set_deferred_desktop(scr);
			} else
				EPRINTF("unreasonable desktop %d\n", newdesk);
		}

	} else
		DPRINTF("could not retrieve _NET_CURRENT_DESKTOP\n");

}

static void
handle_NET_DESKTOP_NAMES(XEvent *e)
{
}

static void
handle_NET_DESKTOP_PIXMAP(XEvent *e)
{
}

/** @brief handle _NET_NUMBER_OF_DESKTOPS property changes
  *
  * Handle when _NET_NUMBER_OF_DESKTOPS property changes on the root window of
  * any screen.  This is how we determine the total number of desktops.
  */
static void
handle_NET_NUMBER_OF_DESKTOPS(XEvent *e)
{
	long number = 0;

	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	if (xde_get_cardinal(scr->root, _XA_NET_NUMBER_OF_DESKTOPS, XA_CARDINAL, &number)) {
		int desktops = number;

		DPRINTF("new number of desktops %d (was %d)\n", desktops, scr->numdesk);

		if (0 < desktops && desktops <= 64) {
			if (desktops != scr->numdesk) {
				scr->numdesk = desktops;
				modulate_desktops();
			}
		} else
			EPRINTF("unreasonable desktop number %d\n", desktops);
	} else
		DPRINTF("could not retrieve _NET_NUMBER_OF_DESKTOPS\n");
}

/** @brief handle _NET_SUPPORTED on the root window
  *
  * A restarting window manager (either the old one or the new one) will change
  * this property on the root window.  Recheck the window manager and theme when
  * this happens.
  */
static void
handle_NET_SUPPORTED(XEvent *e)
{
	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	set_deferred_wmcheck();
}

/** @brief handle _NET_SUPPORTING_WM_CHECK property changes
  *
  * A restarting window manager (either the old one or a new one) will change
  * this recursive property on the root window.  Re-establish the identity of
  * the window manager and recheck the theme for that window manager as some
  * window managers restart when setting themes (such as icewm(1)).
  */
static void
handle_NET_SUPPORTING_WM_CHECK(XEvent *e)
{
	set_deferred_wmcheck();
}

/** @brief handle _NET_WM_NAME property changes
  *
  * Some window managers set the _NET_WM_NAME property on the root window.  When
  * this property changes on the root window, we must suspect that the window
  * manager has changed and recheck the window manager for the screen.
  */
static void
handle_NET_WM_NAME(XEvent *e)
{
	set_deferred_wmcheck();
}

static void
handle_NET_WM_PID(XEvent *e)
{
	set_deferred_wmcheck();
}

/** @brief handle _OB_THEME property change
  *
  * openbox(1) signals a theme change by changing the _OB_THEME property on the
  * root window.  Check the theme again when it changed.
  */
static void
handle_OB_THEME(XEvent *e)
{
	check_theme();
}

static void
handle_OPENBOX_PID(XEvent *e)
{
	set_deferred_wmcheck();
}

/** @brief handle _WIN_DESKTOP_BUTTON_PROXY property changes
  *
  * When the _WIN_DESKTOP_BUTTON_PROXY changes we want to select for button
  * press and release events on the window primarily so that we can switch
  * desktops using the scroll wheel.  Interestingly enough, most window managers
  * that provide the _WIN_DESKTOP_BUTTON_PROXY do not change desktops in
  * response to the scoll wheel on their own by default (e.g. icewm(1)).  The
  * two that provide this are icewm(1) and fvwm(1), both of which do not default
  * to changing the desktop with the scroll wheel on the root window.
  */
static void
handle_WIN_DESKTOP_BUTTON_PROXY(XEvent *e)
{
	Window proxy;

	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	if (e->xproperty.state == PropertyNewValue) {
		if (xde_get_window(scr->root, _XA_WIN_DESKTOP_BUTTON_PROXY,
				   XA_CARDINAL, &proxy) && proxy) {
			if (proxy != scr->proxy) {
				src->proxy = proxy;
				XSelectInput(dpy, proxy,
					     ButtonPressMask | ButtonReleaseMask |
					     StructureNotifyMask);
			}
		} else
			DPRINTF("cannot get _WIN_DESKTOP_BUTTON_PROXY\n");
	} else
		scr->proxy = None;
}

/** @brief handle _WIN_PROTOCOLS property changes
  *
  * A restarting window manager (either the old one or a new one) will change
  * this property on the root window.  Check the window manager and theme again
  * when it changes.
  */
static void
handle_WIN_PROTOCOLS(XEvent *e)
{
	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	set_deferred_wmcheck();
}

/** @brief handle _WIN_SUPPORTING_WM_CHECK property changes
  *
  * A restarting window manager (either the old one or a new one) will change
  * this property on the root window.  Reestablish support for responding to the
  * button proxy when that happens.
  */
static void
handle_WIN_SUPPORTING_WM_CHECK(XEvent *e)
{
	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	set_deferred_wmcheck();
}

/** @brief handle _WIN_WORKSPACE_COUNT property changes
  *
  * Handle when the _WIN_WORKSPACE_COUNT property changes on the root window of
  * any screen.  This is how we determine the total number of workspaces.  This
  * is for compatability with older window managers (such as wmaker(1)).
  */
static void
handle_WIN_WORKSPACE_COUNT(XEvent *e)
{
	long count = 0;

	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	if (xde_get_cardinal(scr->root, _XA_WIN_WORKSPACE_COUNT, XA_CARDINAL, &count)) {
		int workspaces = count;

		DPRINTF("new number of workspaces %d (was %d)\n", workspaces,
			scr->numdesk);

		if (0 < workspaces && workspaces <= 64) {
			if (workspaces != scr->numdesk) {
				scr->numdesk = workspaces;
				modulate_desktops();
			}
		} else
			EPRINTF("unresonable workspace count %d\n", workspaces);
	} else
		DPRINTF("could not retrieve _WIN_WORKSPACE_COUNT\n");
}

/** @brief handle _WIN_WORKSPACE property change
  *
  * Handle when the _WIN_WORKSPACE property changes on the root window of any
  * screen.  This is how we determin that a workspace has changed.  This is for
  * compatability with older window managers (such as wmaker(1)).
  */
static void
handle_WIN_WORKSPACE(XEvent *e)
{
	long wkspace = 0;

	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	if (xde_get_cardinal(scr->root, _XA_WIN_WORKSPACE, XA_CARDINAL, &wkspace)) {
		int newdesk = wkspace;

		DPRINTF("new workspace %d (was %d)\n", newdesk, scr->curdesk);
		if (newdesk != scr->curdesk) {
			if (0 <= newdesk && newdesk < scr->numdesk) {
				scr->curdesk = newdesk;
				set_deferred_desktop(scr);
			} else
				EPRINTF("unreasonable workspace %d\n", newdesk);
		}
	} else
		DPRINTF("could not retrieve _WIN_WORKSPACE_COUNT\n");
}

static void
handle_WM_CLIENT_MACHINE(XEvent *e)
{
	set_deferred_wmcheck();
}

static void
handle_XDE_THEME_NAME(XEvent *e)
{
	check_theme();
}

/** @brief handle _XROOTMAP_ID property changes
  *
  * We do not really process this becuase all proper root setters now set the
  * _XROOTPMAP_ID property which we handle below.  However, it could be used to
  * trigger a recheck of the theme needed by some window managers such as
  * blackbox(1).  If it means we check 3 times after a theme switch, so be it.
  */
static void
handle_XROOTMAP_ID(XEvent *e)
{
	// check_theme();
}

/** @brief handle _XROOTPMAP_ID property changes
  *
  * Handles when the _XROOTPMAP_ID property chages on the root window of any
  * screen.  This is how we determine that another root setting tool has been
  * used to set the background.  A couple of notes:
  *
  * - most root setters are not xinerama/xrandr monitor aware, so that must be
  *   taken into consideration.
  */
static void
handle_XROOTPMAP_ID(XEvent *e)
{
	Pixmap pixmap;

	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	if (xde_get_pixmap(_XA_XROOTPMAP_ID, XA_PIXMAP, &pixmap) && pixmap) {
		if (pixmap != scr->pmid) {
			/* 
			 * FIXME: basically root out the image pixmap that is
			 * currently referenced by the desktop on the root
			 * monitor and replace it with this pixmap.  The old
			 * pixmap (scr->pmid) must be freed and replaced with a
			 * new one, possibly 'pixmap'.
			 */
		}
	} else if (scr->pmid)
		XChangeProperty(dpy, scr->root, _XA_XROOTPMAP_ID, XA_PIXMAP, 32,
				PropModeReplace, (unsigned char *) &scr->pmid, 1);
}

/** @brief handle _XSETROOT_ID property changes
  *
  * Handle when the _XSETROOT_ID property changes on the root window of any
  * screen.  This is how we dtermine that another root setting tool has been
  * used to set the background.  This is for backward compatability with older
  * root setters.
  *
  * We do not really process this because all proper root setters now set the
  * _XROOTPMAP_ID property which we handle above.  However, it is used to
  * trigger recheck of the theme needed by some window managers such as
  * blackbox(1).  If it means we check 3 times after a theme switch, so be it.
  */
static void
handle_XSETROOT_ID(XEvent *e)
{
	Pixmap pixmap;

	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root) {
		DPRINTF("ignoring event\n");
		return;
	}
	if (xde_get_pixmap(scr->root, _XA_XSETROOT_ID, XA_PIXMAP, &pixmap)) {
		WmImage *image = scr->d.desktops[scr->curdesk].image;
		Pixmap oldid = image->pmid;

		if (oldid && oldid != pixmap) {
			/* 
			 * FIXME
			 */
		}
	}
}

/** @brief handle ButtonPress events
  * @param ev - X Event
  *
  * We receive button press events from the desktop button proxy or from our own backdrop
  * windows.
  *
  * Currently we are activating button event actions only on the ButtonRelease.
  * 
  */
static void
handle_ButtonPress(XEvent *ev)
{
}

/** @brief handle ButtonRelease events
  *
  * We recieve button release events from the desktop button proxy or from our
  * own backdrop windows.  This is used to change the desktop on window managers
  * that provide this and need to have the scroll wheel change desktops
  * (L<icewm(1)> and L<fvwm(1)>).
  *
  * Without modifiers, we treat the desktops as a simple linear list of desktops
  * and wrap from the last desktop to the first.  This does not follow a desktop
  * layout.  With a Control modifier, we move horizontally in the same row of
  * desktops according to the layout, wrapping from the end of the row to the
  * beginning of the row.  With a Shift modifier, we move vertically in the same
  * column of the desktop layout, wrapping from the end of the column to the
  * beginning of the column.
  */
static void
handle_ButtonRelease(XEvent *e)
{
	int desktop;
	unsigned int state;
	int row, col;

	XFindContext(dpy, e->xbutton.root, ScreenContext, (XPointer *) &scr);
	desktop = scr->desktop;
	state = e->xbutton.state & (ShiftMask | ControlMask);
	col = desktop % scr->layout.cols;
	row = (desktop - col) / scr->layout.rows;
	switch (e->xbutton.button) {
	case 4:
		switch (state) {
		case ControlMask:
			if (++col >= scr->layout.cols)
				col = 0;
			desktop = (row * scr->layout.cols) + col;
			break;
		case ShiftMask:
			if (++row >= scr->layout.rows)
				row = 0;
			desktop = (row * scr->layout.cols) + col;
			if (desktop >= scr->desktops)
				desktop = col;
			break;
		default:
			/* increase desktop number */
			if (++desktop >= scr->desktops)
				desktop = 0;
			break;
		}
		break;
	case 5:
		switch (state) {
		case ControlMask:
			if (--col < 0)
				col = scr->layout.cols - 1;
			desktop = (row * scr->layout.cols) + col;
			if (desktop >= scr->desktops)
				desktop = scr->desktops;
			break;
		case ShiftMask:
			if (--row < 0)
				row = scr->layout.rows - 1;
			desktop = (row * scr->layout.cols) + col;
			if (desktop >= scr->desktops)
				desktop -= scr->layout.cols;
			break;
		default:
			/* decrease desktop number */
			if (--desktop < 0)
				desktop = scr->desktops - 1;
			break;
		}
		break;
	default:
		/* ignore other buttons */
		return;
	}
	if (options.debug) {
		fprintf(stderr, "Mouse button: %s\n", e->xbutton.button);
		fprintf(stderr, "Current desktop: %d\n", scr->desktop);
		fprintf(stderr, "Requested desktop: %d\n", desktop);
		fprintf(stderr, "Number of desktops: %d\n", scr->desktops);
	}
	if (scr->desktop != desktop) {
		XEvent ev = { 0, };

		ev.xclient.type = ClientMessage;
		ev.xclient.serial = 0;
		ev.xclient.send_event = False;
		ev.xclient.display = dpy;
		ev.xclient.window = scr->root;
		ev.xclient.message_type = _XA_NET_CURRENT_DESKTOP;
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = desktop;
		ev.xclient.data.l[1] = 0;
		ev.xclient.data.l[2] = 0;
		ev.xclient.data.l[3] = 0;
		ev.xclient.data.l[4] = 0;

		XSendEvent(dpy, scr->root, False,
			   StructureNotifyMask | SubstructureNotifyMask |
			   SubstructureRedirectMask, &ev);
		XFlush(dpy);
	}
}

static void
handle_ConfigureNotify(XEvent *ev)
{
}

static void
handle_DestroyNotify(XEvent *ev)
{
}

static void
handle_EnterNotify(XEvent *ev)
{
}

static void
handle_LeaveNotify(XEvent *ev)
{
}

/** @brief handle FocusIn events
  *
  * We select for FocusIn events on the root window.
  */
static void
handle_FocusIn(XEvent *ev)
{
}

static void
handle_Expose(XEvent *ev)
{
}

static void
handle_KeyPress(XEvent *ev)
{
}

static void
handle_MappingNotify(XEvent *ev)
{
}

static void
handle_MapRequest(XEvent *ev)
{
}

static void
handle_PropertyNotify(XEvent *ev)
{
	int i;

	XFindContext(dpy, ev->xproperty.window, ScreenContext, (XPointer *) &scr);

	for (i = 0; myatoms[i].name; i++)
		if (myatoms[i].value == ev->xproperty.atom) {
			if (myatoms[i].handler)
				return myatoms[i].handler(ev);
			return;
		}
}

static void
handle_ReparentNotify(XEvent *ev)
{
}

static void
handle_UnmapNotify(XEvent *ev)
{
}

static void
handle_ClientMessage(XEvent *ev)
{
	Atom type;
	int i;

	XFindContext(dpy, ev->xclient.window, ScreenContext, (XPointer *) &scr);

	if ((type = ev->xclient.message_type) == XA_WM_PROTOCOLS)
		type = ev->xclient.data.l[0];
	for (i = 0; myatoms[i].name; i++)
		if (myatoms[i].value == type) {
			if (myatoms[i].handler)
				return myatoms[i].handler(ev);
			return;
		}
}

static void
handle_SelectionClear(XEvent *ev)
{
}

static void
handle_RRScreenChangeNotify(XEvent *ev)
{
}

void (*handler[LASTEvent + (EXTRANGE + BaseLast)]) (XEvent *ev) = {
	/* *INDENT-OFF* */
	[ButtonPress]		= handle_ButtonPress,
	[ButtonRelease]		= handle_ButtonRelease,
	[ConfigureNotify]	= handle_ConfigureNotify,
	[DestroyNotify]		= handle_DestroyNotify,
	[EnterNotify]		= handle_EnterNotify,
	[LeaveNotify]		= handle_LeaveNotify,
	[FocusIn]		= handle_FocusIn,
	[Expose]		= handle_Expose,
	[KeyPress]		= handle_KeyPress,
	[MappingNotify]		= handle_MappingNotify,
	[MapRequest]		= handle_MapRequest,
	[PropertyNotify]	= handle_PropertyNotify,
	[ReparentNotify]	= handle_ReparentNotify,
	[UnmapNotify]		= handle_UnmapNotify,
	[ClientMessage]		= handle_ClientMessage,
	[SelectionClear]	= handle_SelectionClear,
#ifdef HAVE_XRANDR
	[RRScreenChangeNotify + LASTEvent + EXTRANGE * XrandrBase] = handle_RRScreenChangeNotify,
#endif
	/* *INDENT-ON* */
};

static void
handle_event(XEvent *ev)
{
	int i, slot;

	if (ev->type <= LASTEvent)
		if (handler[ev->type])
			return (handler[ev->type]) (ev);
	for (i = BaseLast - 1; i >= 0; i--) {
		if (!ext[i].have)
			continue;
		if (ev->type >= ext[i].base && ev->type < ext[i].base + EXTRANGE) {
			slot = ev->type - ext[i].base + LASTEvent + EXTRANGE * i;
			if (handler[slot])
				return (handler[slot]) (ev);
		}
	}
}

static void
got_xevents()
{
	XEvent ev;

	while (XPending(dpy)) {
		XNextEvent(dpy, &ev);
		handle_event(&ev);
	}
}

static void
got_timeout()
{
	if (xde_detect_wm())
		process_wm();
	else
		wait_for_wm();
}

static void
bd_init_xinerama()
{
	int dummy;

#ifdef HAVE_XINERAMA
	if ((ext[XineramaBase].have =
	     XineramaQueryExtension(dpy, &ext[XineramaBase].base, &dummy))) {
		XineramaScreenInfo *si;
		int i, n;

		do {
			if (!XineramaIsActive(dpy))
				break;
			if (!(si = XineramaQueryScreens(dpy, &n)) || n < 2)
				break;
			free(monitors);
			monitors = calloc(n, sizeof(*monitors));
			for (i = 0; i < n; i++) {
				monitors[i].index = i;
				monitors[i].x = si[i].x_org;
				monitors[i].y = si[i].y_org;
				monitors[i].width = si[i].width;
				monitors[i].height = si[i].height;
			}
			XFree(si);
			nmon = n;
		} while (0);
	}
#endif				/* HAVE_XINERAMA */
}

static void
bd_init_xrandr()
{
	int dummy;

#ifdef HAVE_XRANDR
	if ((ext[XrandrBase].have =
	     XRRQueryExtension(dpy, &ext[XrandrBase].base, &dummy))) {
		XRRScreenResources *sr;
		int i, j, n;

		do {
			if (!(sr = XRRGetScreenResources(dpy, root)) || sr->ncrtc < 2)
				break;
			free(monitors);
			monitors = calloc(sr->ncrtc, sizeof(*monitors));
			for (i = 0, n = 0; i < sr->nctrc; i++) {
				XRRCrtcInfo *ci;

				if (!(ci = XRRGetCrtcInfo(dpy, sr, sr->crtcs[i])))
					continue;
				if (!ci->width || !ci->height)
					continue;
				/* skip mirrors */
				for (j = 0; j < n; j++)
					if (monitors[j].x == monitors[n].x &&
					    monitors[j].y == monitors[n].y)
						break;
				if (j < n)
					continue;
				monitors[n].x = ci->x;
				monitors[n].y = ci->y;
				monitors[n].width = ci->width;
				monitors[n].height = ci->height;
				n++;
			}
			nmon = n;
		} while (0);
	}
#endif				/* HAVE_XRANDR */
}

void
get_desktops()
{
	long retval = 0;
	Bool found_one = False;
	int i;

	if (!(wm = scr->wm))
		return;

	if (wm->netwm_check) {
		int number_of_desktops, current_desktop;

		number_of_desktops =
		    xde_get_cardinal(root, _XA_NET_NUMBER_OF_DESKTOPS, XA_CARDINAL,
				     &retval)
		    ? retval : 0;
		current_desktop =
		    xde_get_cardinal(root, _XA_NET_CURRENT_DESKTOP, XA_CARDINAL, &retval)
		    ? retval : 0;
		if (number_of_desktops && !found_one) {
			found_one = True;
			scr->numdesk = number_of_desktops;
			scr->curdesk = current_desktop;
		}
	}
	if (wm->winwm_check) {
		int workspace_count, workspace;

		workspace_count =
		    xde_get_cardinal(root, _XA_WIN_WORKSPACE_COUNT, XA_CARDINAL, &retval)
		    ? retval : 0;
		workspace =
		    xde_get_cardinal(root, _XA_WIN_WORKSPACE, XA_CARDINAL, &retval)
		    ? retval : 0;
		if (workspace_count && !found_one) {
			found_one = True;
			scr->numdesk = workspace_count;
			scr->curdesk = workspace;
		}
	}
	if (wm->motif_check) {
		Atom *workspace_list, workspace_current;
		int workspace_list_len;

		workspace_list =
		    xde_get_atoms(root, _XA_DT_WORKSPACE_LIST, XA_ATOM, &retval);
		if (workspace_list)
			workspace_list_len = retval;
		workspace_current =
		    xde_get_atom(root, _XA_DT_WORKSPACE_CURRENT, XA_ATOM, &retval)
		    ? retval : None;
		if (workspace_list && !found_one) {
			for (i = 0; i < workspace_list_len; i++) {
				if (workspace_list[i] == workspace_current) {
					found_one = True;
					scr->numdesk = workspace_list_len;
					scr->curdesk = i;
					break;
				}
			}
		}
		free(workspace_list);
	}
	/* yeah, I know, but some WM's do this */
	if (wm->redir_check) {
		int desktop;

		desktop = xde_get_cardinal(root, _XA_WM_DESKTOP, XA_CARDINAL, &retval)
		    ? retval : -1;
		if (desktop >= 0 && !found_one) {
			found_one = True;
			scr->curdesk = desktop;
		}
	}
	if (found_one) {
	} else {
	}
}

static void
mainloop()
{
	Window dw;
	unsigned int du;
	int dummy;

	state = StartingUp;
	xde_init_display();
	bd_init_xinerama();
	bd_init_xrandr();

	sig_register(SIGTERM, &sig_handler);
	sig_register(SIGHUP, &sig_handler);
	sig_register(SIGALRM, &sig_handler);

	for (screen = 0; screen < nscr; screen++) {
		scr = screens + screen;
		root = scr->root;
		XSelectInput(dpy, root, PropertyChangeMask | StructureNotifyMask |
			     SubstructureNotifyMask);
		get_desktops();
	}

	tfd = timerfd_create(CLOCK_MONOTONIC, 0);
	xfd = ConnectionNumber(dpy);

	check_wm();

	XSync(dpy, False);

	for (;;) {
		struct pollfd pfd[] = {
			{tfd, POLLIN | POLLERR | POLLHUP, 0},
			{xfd, POLLIN | POLLERR | POLLHUP, 0}
		}

		if (trm_signal) {
			trm_signal = 0;
			trm_action();
		}
		if (hup_signal) {
			hup_signal = 0;
			hup_action();
		}
		if (alm_signal) {
			alm_signal = 0;
			alm_action();
		}

		pfd[0].revents = 0;
		pfd[1].revents = 0;

		switch (poll(pfd, 2, -1)) {
		case -1:
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART)
				continue;
			EPRINTF("%s\n", strerror(errno));
			exit(1);
		case 0:	/* timedout */
			continue;
		case 1:
		case 2:
			if (pfd[0].revents & POLLIN)
				got_timeout();
			if (pfd[1].revents & POLLIN)
				got_xevents();
			if ((pfd[0].
			     revents | pfd[1].revents) & (POLLNVAL | POLLHUP | POLLERR)) {
				EPRINTF("fatal error on poll");
				exit(1);
			}
		}
		if (state == ShutDown)
			exit(0);
	}
}

static void
copying(int argc, char *argv[])
{
	if (!options.output && !options.debug)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2010-2018  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2002-2009  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software: you can  redistribute it  and/or modify  it under\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\n\
Software Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
", NAME " " VERSION);
}

static void
version(int argc, char *argv[])
{
	if (!options.output && !options.debug)
		return;
	(void) fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018  Monavacon Limited.\n\
Copyright (c) 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION);
}

static void
usage(int argc, char *argv[])
{
	if (!options.output && !options.debug)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [{-c|--current}] [options]\n\
    %1$s {-l|--list} [options] [STYLE]\n\
    %1$s {-s|--set} [options] STYLE\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!options.output && !options.debug)
		return;
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Command options:\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
Options:\n\
    -D, --debug [LEVEL]\n\
        increment or set debug LEVEL [default: 0]\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL [default: 1]\n\
        this option may be repeated.\n\
", argv[0]);
}

int
main(int argc, char *argv[])
{
	while (1) {
		int c, val;
		char *endptr = NULL;

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {

			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "D::v::hVCH?",
				     long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "DvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			if (options.debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;

		case 'D':	/* -D, --debug [level] */
			if (options.debug)
				fprintf(stderr, "%s: increasing debug verbosity\n",
					argv[0]);
			if (optarg == NULL) {
				options.debug++;
			} else {
				val = strtol(optarg, &endptr, 0);
				if (*endptr || val < 0)
					goto bad_option;
				options.debug = val;
			}
			break;
		case 'v':	/* -v, --verbose [level] */
			if (options.debug)
				fprintf(stderr, "%s: increasing output verbosity\n",
					argv[0]);
			if (optarg == NULL) {
				options.output++;
				break;
			}
			val = strtol(optarg, &endptr, 0);
			if (*endptr || val < 0)
				goto bad_option;
			options.output = val;
			break;
		case 'h':	/* -h, --help */
		case 'H':	/* -H, --? */
			if (options.debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (options.debug)
				fprintf(stderr, "%s: printing version message\n",
					argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			if (options.debug)
				fprintf(stderr, "%s: printing copying message\n",
					argv[0]);
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
		      bad_nonopt:
			if (options.output || options.debug) {
				if (optind < argc) {
					fprintf(stderr, "%s: syntax error near '",
						argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
					fprintf(stderr, "'\n");
				} else {
					fprintf(stderr, "%s: missing option or argument",
						argv[0]);
					fprintf(stderr, "\n");
				}
				fflush(stderr);
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	if (options.debug) {
		fprintf(stderr, "%s: option index = %d\n", argv[0], optind);
		fprintf(stderr, "%s: option count = %d\n", argv[0], argc);
	}
	mainloop();
	exit(0);
}

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
