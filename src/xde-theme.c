/*****************************************************************************

 Copyright (c) 2008-2014  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

#include "xde.h"

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

const char *program = NAME;

char *
get_text(Window win, Atom prop)
{
	XTextProperty tp = { NULL, };

	XGetTextProperty(dpy, win, &tp, prop);
	if (tp.value) {
		tp.value[tp.nitems + 1] = '\0';
		return (char *) tp.value;
	}
	return NULL;
}

static long *
get_cardinals(Window win, Atom prop, Atom type, long *n)
{
	Atom real;
	int format;
	unsigned long nitems, after, num = 1;
	long *data = NULL;

      try_harder:
	if (XGetWindowProperty(dpy, win, prop, 0L, num, False, type, &real, &format,
			       &nitems, &after, (unsigned char **) &data) == Success
	    && format != 0) {
		if (after) {
			num += ((after + 1) >> 2);
			XFree(data);
			goto try_harder;
		}
		if ((*n = nitems) > 0)
			return data;
		if (data)
			XFree(data);
	} else
		*n = -1;
	return NULL;
}

static Bool
get_cardinal(Window win, Atom prop, Atom type, long *card_ret)
{
	Bool result = False;
	long *data, n;

	if ((data = get_cardinals(win, prop, type, &n)) && n > 0) {
		*card_ret = data[0];
		result = True;
	}
	if (data)
		XFree(data);
	return result;
}

Window *
get_windows(Window win, Atom prop, Atom type, long *n)
{
	return (Window *) get_cardinals(win, prop, type, n);
}

Bool
get_window(Window win, Atom prop, Atom type, Window *win_ret)
{
	return get_cardinal(win, prop, type, (long *) win_ret);
}

Time *
get_times(Window win, Atom prop, Atom type, long *n)
{
	return (Time *) get_cardinals(win, prop, type, n);
}

Bool
get_time(Window win, Atom prop, Atom type, Time *time_ret)
{
	return get_cardinal(win, prop, type, (long *) time_ret);
}

Atom *
get_atoms(Window win, Atom prop, Atom type, long *n)
{
	return (Atom *) get_cardinals(win, prop, type, n);
}

Bool
get_atom(Window win, Atom prop, Atom type, Atom *atom_ret)
{
	return get_cardinal(win, prop, type, (long *) atom_ret);
}

Pixmap *
get_pixmaps(Window win, Atom prop, Atom type, long *n)
{
	return (Pixmap *) get_cardinals(win, prop, type, n);
}

Bool
get_pixmap(Window win, Atom prop, Atom type, Pixmap *pixmap_ret)
{
	return get_cardinal(win, prop, type, (long *) pixmap_ret);
}

typedef struct Deferred Deferred;
struct Deferred {
	Deferred *next;
	void (*action) (void *);
	void *data;
};

Deferred *deferred = NULL;

Deferred *
defer_action(void (*action) (void *), void *data)
{
	Deferred *d = calloc(1, sizeof(*d));

	d->next = deferred;
	deferred = d;
	d->action = action;
	d->data = data;
	return d;
}

/*
 * Sets the pixmap specified by pixmap id, pmid, on the root window specified by proot.
 * This is normally only called internally.  This function needs to be improved to set the
 * background color as well and to support tiling and solid colors and gradients when ther
 * is no pixmap specified.
 */
void
set_pixmap(Window proot, Pixmap pmid)
{
	Bool grab = options.grab;
	XSetWindowAttributes wa;

	if (!pmid)
		return;
	wa.background_pixmap = pmid;
	XChangeWindowAttributes(dpy, proot, CWBackPixmap, &wa);
	XClearArea(dpy, proot, 0, 0, 0, 0, False);
	XSync(dpy, False);
	if (grab)
		XGrabServer(dpy);
	XChangeProperty(dpy, proot, _XA_XROOTPMAP_ID, XA_PIXMAP, 32,
			PropModeReplace, (unsigned char *) &pmid, 1);
	XSync(dpy, False);
	if (grab)
		XUngrabServer(dpy);
}

void
deferred_set_pixmap(void *data)
{
	Pixmap pmid;

	scr = (WmScreen *) data;
	screen = scr->screen;
	root = scr->root;
	dsk = &scr->d.desktops[scr->d.current];
	if (!(pmid = dsk->image->pmid))
		return;
	if (!scr->pmid)
		get_cardinal(root, _XA_XROOTPMAP_ID, XA_PIXMAP, (long *) &scr->pmid);
	if (!scr->pmid)
		return;
	if (pmid != scr->pmid)
		set_pixmap(root, pmid);
}

void
check_theme()
{
}

static Bool
get_context(XEvent *e)
{
	int status =
	    XFindContext(dpy, e->xany.window, ScreenContext, (XPointer *) &event_scr);

	if (status == Success) {
		scr = event_scr;
		screen = scr->screen;
		root = scr->root;
		wm = scr->wm;
		return True;
	}
	return False;
}

/** @brief handle a _BB_THEME property change notification
  *
  *  Our blackbox(1) theme files have a rootCommand that changes the _BB_THEME
  *  property on the root window.  Check the theme again when it changes.  Check
  *  the window manager again when it is not blackbox.
  */
static void
handle_BB_THEME(XEvent *e)
{
	if (get_context(e) && xde_check_wm())
		check_theme();
}

/** @brief handle a _BLACKBOX_PID property change notification.
  *
  *  When fluxbox(1) restarts, it does not change the _NET_SUPPORTING_WM_CHECK
  *  window, but it does change the _BLACKBOX_PID, even if it is just to replace
  *  it with the same value.  When restarting, check the theme again.  Check the
  *  window manager again when it is not fluxbox.
  */
static void
handle_BLACKBOX_PID(XEvent *e)
{
	if (get_context(e) && xde_check_wm())
		check_theme();
}

/** @brief handle a _ESETROOT_PMAP_ID property change notification
  *
  *  We do not really process this because all property root setters now set the
  *  _XROOTPMAP_ID property which we handle below.  However, it is used to
  *  trigger recheck of the theme needed by some window managers such as
  *  blackbox.  If it means we check 3 times after a theme switch, so be it.
  */
static void
handle_ESETROOT_PMAP_ID(XEvent *e)
{
	if (get_context(e) && xde_check_wm())
		check_theme();
}

/** @brief handle a _GTK_READ_RCFILES client message
  *
  *  When a GTK style changer changes its style it sends a _GTK_READ_RCFILES
  *  message to the root window to let GTK clients know to reread their RC
  *  files.  We use this also for XDE GTK styles for the desktop.  When we
  *  receive such a message, check the theme again.
  *
  */
static void
handle_GTK_READ_RCFILES(XEvent *e)
{
	check_theme();
}

static void
process_desktops(long *desktops, long n)
{
	WmMonitor *mon;
	unsigned int i, desktop;
	Pixmap oldid, newid;
	Bool changed = False;

	if (n < 1)
		return;
	desktop = desktops[0];
	if (desktop >= scr->d.numb)
		return;
	if (n > 1 && scr->m.numb > 1) {
		/* WM knows how to set different desktops on each monitor */
		for (i = 0; i < n; i++) {
			desktop = desktops[i];
			mon = &scr->m.monitors[i];
			if (desktop >= scr->d.numb || desktop == mon->i.current)
				continue;
			oldid = mon->i.images[mon->i.current].pmid;
			newid = mon->i.images[desktop].pmid;
			mon->i.current = desktop;
			if (oldid != newid)
				changed = True;
		}
	} else if (scr->m.numb > 1) {
		/* WM does not know how to set different desktops on each monitor */
		/* NOTE: this just mods the images around in a line by monitor number. We 
		   eventually need to do this by desktop layout and monitor position on
		   the layout. */
		for (i = 0; i < scr->d.numb; i++, desktop = (desktop + 1) % scr->d.numb) {
			mon = &scr->m.monitors[i];
			if (desktop >= scr->d.numb || desktop == mon->i.current)
				continue;
			oldid = mon->i.images[mon->i.current].pmid;
			newid = mon->i.images[desktop].pmid;
			mon->i.current = desktop;
			if (oldid != newid)
				changed = True;
		}
	} else {
		if (desktop != scr->d.current) {
			oldid = scr->d.desktops[scr->d.current].image->pmid;
			newid = scr->d.desktops[desktop].image->pmid;
			scr->d.current = desktop;
			if (oldid != newid)
				changed = True;
		}
	}
	if (changed)
		defer_action(&deferred_set_pixmap, scr);
}

static void
handle_NET_CURRENT_DESKTOP(XEvent *e)
{
	if (get_context(e)) {
		long *d, n;

		if ((d = get_cardinals(root, _XA_NET_CURRENT_DESKTOP, XA_CARDINAL, &n))) {
			process_desktops(d, n);
			XFree(d);
		}
	}
}

static void
handle_NET_DESKTOP_LAYOUT(XEvent *e)
{
}

static void
handle_NET_NUMBER_OF_DESKTOPS(XEvent *e)
{
	if (get_context(e)) {
		long data = scr->d.numb;
		unsigned int numb;

		get_cardinal(root, _XA_NET_NUMBER_OF_DESKTOPS, XA_CARDINAL, &data);
		if ((numb = data) != scr->d.numb) {
			if (numb < scr->d.numb) {
			} else {
			}
		}
	}
}

static void
handle_NET_SUPPORTED(XEvent *e)
{
}

static void
handle_NET_SUPPORTING_WM_CHECK(XEvent *e)
{
	if (get_context(e) && xde_check_wm())
		check_theme();
}

static void
handle_NET_VISIBLE_DESKTOPS(XEvent *e)
{
}

/** @brief handle an _OB_THEME property change notification.
  *
  * openbox(1) signals a theme change by changing the _OB_THEME property on the
  * root window.  Check the theme again when it changes.  Check the window
  * manager again when it is not openbox.
  */
static void
handle_OB_THEME(XEvent *e)
{
	check_theme();
}

static void
handle_OPENBOX_PID(XEvent *e)
{
}

static void
handle_WIN_DESKTOP_BUTTON_PROXY(XEvent *e)
{
}

static void
handle_WINDOWMAKER_NOTICEBOARD(XEvent *e)
{
	if (get_context(e) && xde_check_wm())
		check_theme();
}

static void
handle_WIN_PROTOCOLS(XEvent *e)
{
}

static void
handle_WIN_SUPPORTING_WM_CHECK(XEvent *e)
{
	if (get_context(e) && xde_check_wm())
		check_theme();
}

static void
handle_WIN_WORKSPACE_COUNT(XEvent *e)
{
}

static void
handle_WIN_WORKSPACE(XEvent *e)
{
}

static void
handle_XDE_THEME_NAME(XEvent *e)
{
}

static void
handle_XROOTPMAP_ID(XEvent *e)
{
	/* We do not really process this because all proper root setters now set the
	   _XROOTPMAP_ID property which we handle above.  However, it is used to trigger
	   recheck of the theme needed by some window managers such as blackbox.  If it
	   means we check 3 times after a theme switch, so be it. */
	check_theme();
}

static void
handle_XSETROOT_ID(XEvent *e)
{
	/* Internal function that handles the _XSETROOT_ID property changes on the root
	   window or any screen.  This is how xde-theme determines that another root
	   setting tool has been used to set the background.  THis is for backward
	   compatibility with older root setters. */
	/* We do not really process this bceause all proper root setters now set the
	   _XROOTPMAP_ID property which we handle above.  However, it is used to trigger
	   recheck of the theme needed by some window managers such as blackbox.  If it
	   means we check 3 times after a theme switch, so be it. */
	if (XFindContext(dpy, e->xany.window, ScreenContext, (XPointer *) &scr) ==
	    Success) {
		Pixmap pmid = None, oldid;

		screen = scr->screen;
		root = scr->root;
		dsk = &scr->d.desktops[scr->d.current];
		if (get_pixmap(root, _XA_XSETROOT_ID, XA_PIXMAP, &pmid) && pmid) {
			oldid = dsk->image->pmid;
			if (oldid && oldid != pmid) {
				XFreePixmap(dpy, oldid);
				free(dsk->image->file);
				dsk->image->file = NULL;
			}
			if (pmid) {
				dsk->image->pmid = pmid;
			}
			scr->pmid = pmid;
		}
	}
}

/** @brief find the screen associated with an X event
  */
void
get_event_screen(XEvent *e)
{
	XPointer xptr = NULL;

	event_scr = NULL;

	if (XFindContext(dpy, e->xany.window, ScreenContext, &xptr)) {
		Window root, parent, *children = NULL;
		unsigned int nchildren;

		/* try to find the root of the window */
		if (XQueryTree
		    (dpy, e->xany.window, &root, &parent, &children, &nchildren))
			if (!XFindContext(dpy, root, ScreenContext, &xptr))
				event_scr = (WmScreen *) xptr;
		if (children)
			XFree(children);
	} else
		event_scr = (WmScreen *) xptr;
}

typedef struct {
	Atom *atom;
	void (*handler_PropertyNotify) (XEvent *);
	void (*handler_ClientMessage) (XEvent *);
	Atom value;
} Handlers;

Handlers handlers[] = {
	/* *INDENT-OFF* */
	/* global			handler_PropertyNotify		handler_ClientMessage	value		*/
	/* ------			-------				---------------------	*/
	{  &_XA_BB_THEME,		handle_BB_THEME,		NULL,			None		},
	{  &_XA_BLACKBOX_PID,		handle_BLACKBOX_PID,		NULL,			None		},
	{  &_XA_ESETROOT_PMAP_ID,	handle_ESETROOT_PMAP_ID,	NULL,			None		},
	{  &_XA_GTK_READ_RCFILES,	NULL,				handle_GTK_READ_RCFILES,None		},
	{  &_XA_I3_CONFIG_PATH,		NULL,				NULL,			None		},
	{  &_XA_I3_PID,			NULL,				NULL,			None		},
	{  &_XA_I3_SHMLOG_PATH,		NULL,				NULL,			None		},
	{  &_XA_I3_SOCKET_PATH,		NULL,				NULL,			None		},
	{  &_XA_ICEWMBG_QUIT,		NULL,				NULL,			None		},
	{  &_XA_MOTIF_WM_INFO,		NULL,				NULL,			None		},
	{  &_XA_NET_CURRENT_DESKTOP,	handle_NET_CURRENT_DESKTOP,	NULL,			None		},
	{  &_XA_NET_DESKTOP_LAYOUT,	handle_NET_DESKTOP_LAYOUT,	NULL,			None		},
	{  &_XA_NET_DESKTOP_PIXMAPS,	NULL,				NULL,			None		},
	{  &_XA_NET_NUMBER_OF_DESKTOPS,	handle_NET_NUMBER_OF_DESKTOPS,	NULL,			None		},
	{  &_XA_NET_SUPPORTED,		handle_NET_SUPPORTED,		NULL,			None		},
	{  &_XA_NET_SUPPORTING_WM_CHECK,handle_NET_SUPPORTING_WM_CHECK,	NULL,			None		},
	{  &_XA_NET_VISIBLE_DESKTOPS,	handle_NET_VISIBLE_DESKTOPS,	NULL,			None		},
	{  &_XA_NET_WM_NAME,		NULL,				NULL,			None		},
	{  &_XA_NET_WM_PID,		NULL,				NULL,			None		},
	{  &_XA_OB_THEME,		handle_OB_THEME,		NULL,			None		},
	{  &_XA_OPENBOX_PID,		handle_OPENBOX_PID,		NULL,			None		},
	{  &_XA_WIN_DESKTOP_BUTTON_PROXY,handle_WIN_DESKTOP_BUTTON_PROXY,NULL,			None		},
	{  &_XA_WINDOWMAKER_NOTICEBOARD,handle_WINDOWMAKER_NOTICEBOARD,	NULL,			None		},
	{  &_XA_WIN_PROTOCOLS,		handle_WIN_PROTOCOLS,		NULL,			None		},
	{  &_XA_WIN_SUPPORTING_WM_CHECK,handle_WIN_SUPPORTING_WM_CHECK,	NULL,			None		},
	{  &_XA_WIN_WORKSPACE,		handle_WIN_WORKSPACE,		NULL,			None		},
	{  &_XA_WIN_WORKSPACE_COUNT,	handle_WIN_WORKSPACE_COUNT,	NULL,			None		},
	{  &_XA_XDE_THEME_NAME,		handle_XDE_THEME_NAME,		NULL,			None		},
	{  &_XA_XROOTPMAP_ID,		handle_XROOTPMAP_ID,		NULL,			None		},
	{  &_XA_XSETROOT_ID,		handle_XSETROOT_ID,		NULL,			None		},
	{  NULL,			NULL,				NULL,			None		}
	/* *INDENT-ON* */
};

void
handle_event(XEvent *e)
{
	int i;

	get_event_screen(e);

	switch (e->type) {
	case PropertyNotify:
		for (i = 0; handlers[i].atom; i++) {
			if (e->xproperty.atom == *handlers[i].atom) {
				if (handlers[i].handler_PropertyNotify)
					handlers[i].handler_PropertyNotify(e);
				break;
			}
		}
		break;
	case ClientMessage:
		for (i = 0; handlers[i].atom; i++) {
			if (e->xclient.message_type == *handlers[i].atom) {
				if (handlers[i].handler_ClientMessage)
					handlers[i].handler_ClientMessage(e);
				break;
			}
		}
	case DestroyNotify:
		if (get_context(e))
			if (e->xany.window == scr->wm->netwm_check ||
			    e->xany.window == scr->wm->winwm_check ||
			    e->xany.window == scr->wm->maker_check ||
			    e->xany.window == scr->wm->motif_check ||
			    e->xany.window == scr->wm->icccm_check)
				xde_check_wm();
		break;
	}
}

void
handle_deferred_events()
{
	Deferred *d, *next;

	while ((d = deferred)) {
		next = d->next;
		d->action(d->data);
		if (deferred == d) {
			deferred = next;
			free(d);
		}
	}
}

Bool running;

void
event_loop()
{
	fd_set rd;
	int xfd;
	XEvent ev;

	running = True;
	XSelectInput(dpy, root, PropertyChangeMask);
	XSync(dpy, False);
	xfd = ConnectionNumber(dpy);

	while (running) {
		FD_ZERO(&rd);
		FD_SET(xfd, &rd);
		if (select(xfd + 1, &rd, NULL, NULL, NULL) == -1) {
			if (errno == EINTR)
				continue;
			fprintf(stderr, "select failed\n");
			fflush(stderr);
			exit(1);
		}
		while (XPending(dpy)) {
			XNextEvent(dpy, &ev);
			handle_event(&ev);
		}
		handle_deferred_events();
	}
}

/*
 * Testing for window managers:
 *
 * IceWM:   Sets _NET_SUPPORTING_WM_CHECK(WINDOW) appropriately.  Note that it sets
 *	    _WIN_SUPPORTING_WM_CHECK(CARDINAL) as well.  Also, it sets both to the
 *	    same window.  It sets _NET_WM_NAME(STRING) to "IceWM 1.3.7 (Linux
 *	    3.4.0/x86_64)" or some such.  Extract the first word of the string for
 *	    the actual name.  Note that _NET_WM_NAME should be (UTF8_STRING) instead
 *	    of (STRING) [this has been fixed].  It sets _NET_WM_PID(CARDINAL) to the
 *	    pid of the window manager; however, it does not set
 *	    WM_CLIENT_MACHINE(STRING) to the fully qualified domain name of the
 *	    window manager machine as required by the EWMH specification [this has
 *	    been fixed].
 *
 * Blackbox:
 *	    Blackbox is only ICCCM/EWMH compliant and is not GNOME/WMH compliant.  It
 *	    properly sets _NET_SUPPORTING_WM_CHECK(WINDOW) on both the root and the
 *	    check window.  On the check window the only other thing that it sets is
 *	    _NET_WM_NAME(UTF8_STRING) whcih is a property UTF8_STRING with the single
 *	    word "Blackbox".  [It now sets _NET_WM_PID correctly, but still does not
 *	    set WM_CLIENT_MACHINE(STRING) to the fully qualified domain name of the
 *	    window manager machine. [fixed]]
 *
 * Fluxbox: Fluxbox is only ICCCM/EWMH compliant and is not GNOME/WMH compliant.  It
 *	    properly sets _NET_SUPPORTING_WM_CHECK(WINDOW) on both the root and the
 *	    check window.  On the check window the only other thing it sets is
 *	    _NET_WM_NAME(UTF8_STRING) which is a propert UTF8_STRING with the single
 *	    word "Fluxbox".
 *
 *	    Fluxbox also sets _BLACKBOX_PID(CARDINAL) on the root window.  (Gee,
 *	    blackbox doesn't!)  Fluxbox interns the _BLACKBOX_ATTRIBUTES atom and
 *	    then does nothing with it.  Fluxbox interns the _FLUXBOX_ACTION,
 *	    _FLUXBOX_ACTION_RESULT and _FLUXBOX_GROUP_LEFT atoms.  Actions are only
 *	    possible when the session.session0.allowRemoteActions resources is set to
 *	    tru.  THey are affected by changing the _FLUXBOX_ACTION(STRING) property
 *	    on the root window to reflect the new command.  The result is
 *	    communicated by fluxbox setting the _FLUXBOX_ACTION_RESULT(STRING)
 *	    property on the root window with the result.
 *
 * Openbox: Openbox is only ICCCM/EWMH compliant and is not GNOME/WMH compliant.  It
 *	    properly sets _NET_SUPPORTING_WM_CHECK(WINDOW) on both the root and the
 *	    check window.  On the check window, the only other thing that it sets is
 *	    _NET_WM_NAME(UTF8_STRING) which is a proper UTF8_STRING with the single
 *	    word "Openbox".
 *
 *	    Openbox also sets _OPENBOX_PID(CARDINAL) on the root window.  It also
 *	    sets _OB_VERSION(UTF8_STRING) and _OB_THEME(UTF8_STRING) on the root
 *	    window.
 *
 * FVWM:    FVWM is both GNOME/WMH and ICCCM/EWMH compliant.  It sets
 *	    _NET_SUPPORTING_WM_CHECK(WINDOW) properly on the root and check window.
 *	    On the check window it sets _NET_WM_NAME(UTF8_STRING) to "FVWM".  It sets
 *	    WM_NAME(STRING) to "fvwm" and WM_CLASS(STRING) to "fvwm", "FVWM".  FVWM
 *	    implements _WIN_SUPPORTING_WM_CHECK(CARDINAL) in a separate window from
 *	    _NET_SUPPORTING_WM_CHECK(WINDOW), but the same one as
 *	    _WIN_DESKTOP_BUTTON_PROXY(CARDINAL).  There are no additional properties
 *	    set on those windows.
 *
 * WindowMaker:
 *	    WindowMaker is only ICCCM/EWMH compliant and is not GNOME/WMH compliant.
 *	    It properly sets _NET_SUPPORTING_WM_CHECK(WINDOW) on both the root and
 *	    the check window.  It does not set the _NET_WM_NAME(UTF8_STRING) on the
 *	    check window.  It does, however, define a recursive
 *	    _WINDOWMAKER_NOTICEBOARD(WINDOW) that shares the same window as the check
 *	    window and sets the _WINDOWMAKER_ICON_TILE(_RGBA_IMAGE) property on this
 *	    window to the ICON/DOCK/CLIP tile.
 *
 * PeKWM:
 *	    PeKWM is only ICCCM/EWMH compliant and is not GNOME/WMH compliant.  It
 *	    properly sets _NET_SUPPORTING_WM_CHECK(WINDOW) on both the root and the
 *	    check window.  It sets _NET_WM_NAME(STRING) on the check window.  Note
 *	    that _NET_WM_NAME should be (UTF8_STRING) instead of (STRING) (corrected
 *	    in 'git' version).  It does not set WM_CLIENT_MACHINE(STRING) on the
 *	    check window as required by EWMH, but sets it on the root window.  It
 *	    does not, however, set it to the fully qualified domain name as required
 *	    by EWMH.  Also, it sets _NET_WM_PID(CARDINAL) on the check window, but
 *	    mistakenly sets it on the root window.  It sets WM_CLASS(STRING) to a
 *	    null string on the check window and does not set WM_NAME(STRING).
 *
 * JWM:
 *	    JWM is only ICCCM/EWMH compliant and is not GNOME/WMH compliant.  It
 *	    properly sets _NET_SUPPORTING_WM_CHECK(WINDOW) on both the root and the
 *	    check window.  It properly sets _NET_WM_NAME(UTF8_STRING) on the check
 *	    window (to "JWM").  It does not properly set _NET_WM_PID(CARDINAL) on the
 *	    check window, or anywhere for that matter [it does now].  It does not set
 *	    WM_CLIENT_MACHINE(STRING) anywhere and there is no WM_CLASS(STRING) or
 *	    WM_NAME(STRING) on the check window.
 *
 */

static void
copying(int argc, char *argv[])
{
	if (!options.output && !options.debug)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2014  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
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
Copyright (c) 2008, 2009, 2010, 2011, 2012, 2013, 2014  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
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
    %1$s [command option] [options] [FILE [FILE ...]]\n\
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
    %1$s [command option] [options] [FILE [FILE ...]]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    [FILE [FILE ...]]\n\
        a list of files (one per virtual desktop)\n\
Command options:\n\
    -s, --set\n\
        set the background\n\
    -e, --edit\n\
        launch background settings editor\n\
    -q, --quit\n\
        ask running instance to quit\n\
    -r, --restart\n\
        ask running instance to restart\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
Options:\n\
    -g, --grab\n\
	grab the X server while setting backgrounds\n\
    -s, --setroot\n\
	set the background pixmap instead of just properties\n\
    -n, --nomonitor\n\
	exit after setting the background\n\
    -t, --theme THEME\n\
	set the specified theme\n\
    -d, --delay MILLISECONDS\n\
	specifies delay after window manager appearance\n\
    -a, --areas\n\
	distribute backgrounds also over work areas\n\
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

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"grab",	no_argument,		NULL, 'g'},
			{"setroot",	no_argument,		NULL, 's'},
			{"nomonitor",	no_argument,		NULL, 'n'},
			{"theme",	required_argument,	NULL, 't'},
			{"delay",	required_argument,	NULL, 'd'},
			{"areas",	no_argument,		NULL, 'a'},

			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "D::v::hVCH?", long_options,
				     &option_index);
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
		case 'g':	/* -g, --grab */
			options.grab = True;
			break;
		case 's':	/* -s, --setroot */
			options.setroot = True;
			break;
		case 'n':	/* -n, --nomonitor */
			options.nomonitor = True;
			break;
		case 't':	/* -t, --theme THEME */
			options.style = strdup(optarg);
			break;
		case 'd':	/* -d, --delay MILLISECONDS */
			options.delay = strtoul(optarg, NULL, 0);
			break;
		case 'a':	/* -a, --areas */
			options.areas = True;
			break;

		case 'D':	/* -D, --debug [level] */
			if (options.debug)
				fprintf(stderr, "%s: increasing debug verbosity\n",
					argv[0]);
			if (optarg == NULL) {
				options.debug++;
			} else {
				if ((val = strtol(optarg, NULL, 0)) < 0)
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
			if ((val = strtol(optarg, NULL, 0)) < 0)
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
	if (optind < argc) {
		int n = argc - optind, j = 0;

		options.files = calloc(n + 1, sizeof(*options.files));
		while (optind < argc)
			options.files[j++] = strdup(argv[optind++]);
	}
	exit(0);
}

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
