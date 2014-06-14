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

static char **rargv;
static int rargc;

Atom _XA_XDE_WATCH_COMMAND;

Bool foreground = False;

enum {
	XDE_WATCH_QUIT,
	XDE_WATCH_RESTART,
	XDE_WATCH_RECHECK,
	XDE_WATCH_ARGV,
};

typedef struct {
	char *stylefile;
	char *style;
	char *stylename;
	char *menu;
	char *icon;
	char *theme;
	char *themefile;
} WmSettings;

static WmSettings *setting;
static WmSettings *settings;

static Bool
wm_event(const XEvent *e)
{
	switch (e->type) {
	case ClientMessage:
		if (e->xclient.message_type == _XA_XDE_WATCH_COMMAND) {
			DPRINTF("got _XDE_WATCH_COMMAND\n");
			switch (e->xclient.data.l[0]) {
			case XDE_WATCH_RECHECK:
				DPRINTF("got _XDE_WATCH_COMMAND(Recheck)\n");
				xde_defer_wm_check(0);
				return XDE_EVENT_STOP;
			case XDE_WATCH_QUIT:
				DPRINTF("got _XDE_WATCH_COMMAND(Quit)\n");
				xde_main_quit((XPointer) XDE_WATCH_QUIT);
				return XDE_EVENT_STOP;
			case XDE_WATCH_RESTART:
				DPRINTF("got _XDE_WATCH_COMMAND(Restart)\n");
				xde_main_quit((XPointer) XDE_WATCH_RESTART);
				return XDE_EVENT_STOP;
			case XDE_WATCH_ARGV:
				DPRINTF("got _XDE_WATCH_COMMAND(Argv)\n");
				if (XGetCommand(dpy, e->xclient.window, &rargv, &rargc)) {
					XDeleteProperty(dpy, e->xclient.window,
							XA_WM_COMMAND);
					xde_main_quit((XPointer) XDE_WATCH_ARGV);
					return XDE_EVENT_STOP;
				}
				break;
			}
		}
		break;
	case SelectionClear:
		if (e->xselectionclear.window == scr->selwin
		    && e->xselectionclear.selection == scr->selection) {
			DPRINTF("%s selection cleared\n",
				XGetAtomName(dpy, scr->selection));
			xde_del_properties();
			xde_main_quit((XPointer) XDE_WATCH_QUIT);
			return XDE_EVENT_STOP;

		}
		break;
	}
	return XDE_EVENT_PROPAGATE;
}

static Bool
wm_signal(int signum)
{
	return XDE_EVENT_PROPAGATE;
}

/** @brief window manager changed callback
  *
  * Invoked whenever the window manager changes.  Note that everything (style,
  * menu, icon, theme) has already been updated, so we can either handle them
  * here, or wait for the invocation of the individual callbacks, below.
  */
static void
wm_changed()
{
	xde_set_properties();
}

/** @brief window manager style change callback
  *
  * Invoked whenever the window manager style changes, or whenever the window
  * manager changes (regardless of whether the style actually changed).
  */
static void
wm_style_changed(char *newname, char *newstyle, char *newfile)
{
	Window win, wins[2] = { scr->selwin, scr->root };
	int i;

	setting = settings + scr->screen;
	free(setting->stylename);
	setting->stylename = newname ? strdup(newname) : NULL;
	free(setting->style);
	setting->style = newstyle ? strdup(newstyle) : NULL;
	free(setting->stylefile);
	setting->stylefile = newfile ? strdup(newfile) : NULL;
	for (i = 0; i < 2; i++) {
		if (!(win = wins[i]))
			continue;
		xde_set_text(win, _XA_XDE_WM_STYLENAME, XUTF8StringStyle, newname);
		xde_set_text(win, _XA_XDE_WM_STYLE, XUTF8StringStyle, newstyle);
		xde_set_text(win, _XA_XDE_WM_STYLEFILE, XUTF8StringStyle, newfile);
	}
}

/** @brief window manager root menu changed callback
  *
  * Invoked whenever the window manager root menu path changes, or whenever the
  * window manager changes (regardless of whether the path actually changed).
  */
static void
wm_menu_changed(char *newmenu)
{
	Window win, wins[2] = { scr->selwin, scr->root };
	int i;

	setting = settings + scr->screen;
	free(setting->menu);
	setting->menu = newmenu ? strdup(newmenu) : NULL;
	for (i = 0; i < 2; i++) {
		if (!(win = wins[i]))
			continue;
		xde_set_text(win, _XA_XDE_WM_MENU, XUTF8StringStyle, newmenu);
	}
}

/** @brief window manager icon changed callback
  *
  * Invoked whenever the window manager icon changes, or whenever the window
  * manager changes (regardless of whether the icon actually changed).
  */
static void
wm_icon_changed(char *newicon)
{
	Window win, wins[2] = { scr->selwin, scr->root };
	int i;

	setting = settings + scr->screen;
	free(setting->icon);
	setting->icon = newicon ? strdup(newicon) : NULL;
	for (i = 0; i < 2; i++) {
		if (!(win = wins[i]))
			continue;
		xde_set_text(win, _XA_XDE_WM_ICON, XUTF8StringStyle, newicon);
	}
}

/** @brief window manager (XDE really) theme changed callback
  *
  * Invoked whenever the theme changes, or whenever the window manager changes
  * (regardless of whether the theme actually changed).
  */
static void
wm_theme_changed(char *newtheme, char *newfile)
{
	Window win, wins[2] = { scr->selwin, scr->root };
	int i;

	setting = settings + scr->screen;
	free(setting->theme);
	setting->theme = newtheme ? strdup(newtheme) : NULL;
	free(setting->themefile);
	setting->themefile = newfile ? strdup(newfile) : NULL;
	for (i = 0; i < 2; i++) {
		if (!(win = wins[i]))
			continue;
		xde_set_text(win, _XA_XDE_WM_THEME, XUTF8StringStyle, newtheme);
		xde_set_text(win, _XA_XDE_WM_THEMEFILE, XUTF8StringStyle, newfile);
	}

	if (!options.dryrun) {
		XClientMessageEvent xcm;

		xcm.type = ClientMessage;
		xcm.serial = 0;
		xcm.send_event = False;
		xcm.display = dpy;
		xcm.window = scr->root;
		xcm.message_type = _XA_GTK_READ_RCFILES;
		xcm.format = 32;
		xcm.data.l[0] = 0;
		xcm.data.l[1] = 0;
		xcm.data.l[2] = 0;
		xcm.data.l[3] = 0;
		xcm.data.l[4] = 0;

		DPRINTF("sending %s to 0x%08lx\n",
			XGetAtomName(dpy, _XA_GTK_READ_RCFILES), scr->root);
		XSendEvent(dpy, scr->root, False, StructureNotifyMask |
			   SubstructureRedirectMask | SubstructureNotifyMask,
			   (XEvent *) &xcm);
	} else
		OPRINTF("would send _GTK_READ_RCFILES client message\n");

	xde_set_text(scr->root, _XA_XDE_THEME_NAME, XUTF8StringStyle, newtheme);

}

static WmCallbacks wm_callbacks = {
	.wm_event = wm_event,
	.wm_signal = wm_signal,
	.wm_changed = wm_changed,
	.wm_style_changed = wm_style_changed,
	.wm_menu_changed = wm_menu_changed,
	.wm_icon_changed = wm_icon_changed,
	.wm_theme_changed = wm_theme_changed,
};

static XPointer
do_startup(void)
{
	int s;

	if (!foreground) {
		pid_t pid;

		if ((pid = fork()) < 0) {
			EPRINTF("fork: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else if (pid != 0) {
			/* parent exits */
			exit(EXIT_SUCCESS);
		}
		/* become a session leader */
		setsid();
		/* close files */
		fclose(stdin);
		/* fork once more for SVR4 */
		if ((pid = fork()) < 0) {
			EPRINTF("fork: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else if (pid != 0) {
			/* parent exits */
			exit(EXIT_SUCCESS);
		}
		/* release current directory */
		if (chdir("/") < 0) {
			EPRINTF("chdir: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		/* clear file creation mask */
		umask(0);
	}
	settings = calloc(nscr, sizeof(*settings));
	for (s = 0; s < nscr; s++) {
		xde_set_screen(s);
		xde_recheck_wm();
	}
	return xde_main_loop();
}

#if 0
static Bool
owner_died_predicate(Display *display, XEvent *event, XPointer arg)
{
	Window owner = (Window) arg;

	if (event->type != DestroyNotify)
		return False;
	if (event->xdestroywindow.window != owner)
		return False;
	return True;
}
#endif

#if 0
static Bool
cmd_remove_predicate(Display *display, XEvent *event, XPointer arg)
{
	Window mine = (Window) arg;

	if (event->type != PropertyNotify)
		return False;
	if (event->xproperty.window != mine)
		return False;
	if (event->xproperty.atom != XA_WM_COMMAND)
		return False;
	if (event->xproperty.state != PropertyDelete)
		return False;
	return True;
}
#endif

static Bool
selectionreleased(Display *display, XEvent *event, XPointer arg)
{
	if (event->type != DestroyNotify)
		return False;
	if (event->xdestroywindow.window != (Window) arg)
		return False;
	return True;
}

static void
do_run(int argc, char *argv[])
{
	xde_init(&wm_callbacks);
	_XA_XDE_WATCH_COMMAND = XInternAtom(dpy, "_XDE_WATCH_COMMAND", False);
	for (screen = 0; screen < nscr; screen++) {
		char name[64] = { 0, };

		xde_set_screen(screen);
		snprintf(name, sizeof(name), "_XDE_WATCH_S%d", screen);
		scr->selection = XInternAtom(dpy, name, False);
		scr->selwin = XCreateSimpleWindow(dpy, scr->root,
						  DisplayWidth(dpy, screen),
						  DisplayHeight(dpy, screen), 1, 1, 0,
						  BlackPixel(dpy, screen),
						  BlackPixel(dpy, screen));
		XSaveContext(dpy, scr->selwin, ScreenContext, (XPointer) scr);
		XSelectInput(dpy, scr->selwin, StructureNotifyMask | PropertyChangeMask);

		XGrabServer(dpy);
		if ((scr->owner = XGetSelectionOwner(dpy, scr->selection))) {
			XSelectInput(dpy, scr->owner,
				     StructureNotifyMask | PropertyChangeMask);
			XSaveContext(dpy, scr->owner, ScreenContext, (XPointer) scr);
			XSync(dpy, False);
		}
		XUngrabServer(dpy);

		if (!scr->owner || options.replace)
			XSetSelectionOwner(dpy, scr->selection, scr->selwin, CurrentTime);
		else {
			EPRINTF("another instance of %s already on screen %d\n",
			     NAME, scr->screen);
			exit(EXIT_FAILURE);
		}
	}
	for (screen = 0; screen < nscr; screen++) {
		XEvent ev;

		xde_set_screen(screen);
		if (scr->owner && options.replace) {
			XIfEvent(dpy, &ev, &selectionreleased, (XPointer) scr->owner);
			scr->owner = None;
		}
		xde_set_window(scr->selwin, _XA_XDE_WM_INFO, XA_WINDOW, scr->selwin);
		xde_set_window(scr->root, _XA_XDE_WM_INFO, XA_WINDOW, scr->selwin);

		ev.xclient.type = ClientMessage;
		ev.xclient.serial = 0;
		ev.xclient.send_event = False;
		ev.xclient.display = dpy;
		ev.xclient.window = scr->root;
		ev.xclient.message_type = _XA_MANAGER;
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = CurrentTime;	/* FIXME */
		ev.xclient.data.l[1] = scr->selection;
		ev.xclient.data.l[2] = scr->selwin;
		ev.xclient.data.l[3] = 2;
		ev.xclient.data.l[4] = 0;

		XSendEvent(dpy, scr->root, False, StructureNotifyMask, (XEvent *) &ev);
		XSync(dpy, False);
	}

#if 0
	if (scr->owner && !options.replace) {
		XEvent xev;
		XClientMessageEvent xcm;

		XSetCommand(dpy, owner, argv, argc);

		xcm.type = ClientMessage;
		xcm.serial = 0;
		xcm.display = dpy;
		xcm.window = owner;
		xcm.message_type = _XA_XDE_WATCH_COMMAND;
		xcm.format = 32;
		xcm.data.l[0] = XDE_WATCH_ARGV;
		xcm.data.l[1] = 0;
		xcm.data.l[2] = 0;
		xcm.data.l[3] = 0;
		xcm.data.l[4] = 0;

		DPRINTF("sending %s Argv to 0x%08lx\n",
			XGetAtomName(dpy, _XA_XDE_WATCH_COMMAND), owner);
		XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
		XSync(dpy, False);
		if (!XCheckIfEvent(dpy, &xev, owner_died_predicate, (XPointer) owner)) {
			XIfEvent(dpy, &xev, cmd_remove_predicate, (XPointer) owner);
			XDestroyWindow(dpy, mine);
			XCloseDisplay(dpy);
			exit(EXIT_SUCCESS);
		}
		XSetSelectionOwner(dpy, selection, mine, CurrentTime);
		XSync(dpy, False);
	}
#endif
	switch ((int) (long) do_startup()) {
	case XDE_WATCH_QUIT:
		exit(EXIT_SUCCESS);
		break;
	case XDE_WATCH_RESTART:
	{
		char **pargv = calloc(argc + 1, sizeof(*pargv));
		int i;

		for (i = 0; i < argc; i++)
			pargv[i] = argv[i];
		pargv[i] = 0;
		execv(pargv[0], pargv);
		EPRINTF("execv: %s\n", strerror(errno));
		break;
	}
	case XDE_WATCH_ARGV:
	{
		char **pargv = calloc(rargc + 1, sizeof(*pargv));
		int i;

		for (i = 0; i < rargc; i++)
			pargv[i] = rargv[i];
		pargv[i] = 0;
		execv(pargv[0], pargv);
		EPRINTF("execv: %s\n", strerror(errno));
		break;
	}
	case XDE_WATCH_RECHECK:
		EPRINTF("should not get here\n");
		break;
	}
	exit(EXIT_FAILURE);
}

static void
do_quit()
{
	char name[64] = { 0, };
	Atom selection;
	Window owner;

	xde_init_display();
	_XA_XDE_WATCH_COMMAND = XInternAtom(dpy, "_XDE_WATCH_COMMAND", False);
	snprintf(name, sizeof(name), "_XDE_WATCH_S%d", scr->screen);
	selection = XInternAtom(dpy, name, False);

	if ((owner = XGetSelectionOwner(dpy, selection))) {
		if (!options.dryrun) {
			XClientMessageEvent xcm;

			xcm.type = ClientMessage;
			xcm.serial = 0;
			xcm.display = dpy;
			xcm.window = owner;
			xcm.message_type = _XA_XDE_WATCH_COMMAND;
			xcm.format = 32;
			xcm.data.l[0] = XDE_WATCH_QUIT;
			xcm.data.l[1] = 0;
			xcm.data.l[2] = 0;
			xcm.data.l[3] = 0;
			xcm.data.l[4] = 0;

			DPRINTF("sending %s Quit to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_WATCH_COMMAND), owner);
			XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
		} else
			OPRINTF("would send %s Quit to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_WATCH_COMMAND), owner);

		XSync(dpy, False);
		XCloseDisplay(dpy);
		exit(EXIT_SUCCESS);
	} else {
		EPRINTF("No running instance of %s\n", NAME);
		exit(EXIT_FAILURE);
	}
}

static void
do_restart()
{
	char name[64] = { 0, };
	Atom selection;
	Window owner;

	xde_init_display();
	_XA_XDE_WATCH_COMMAND = XInternAtom(dpy, "_XDE_WATCH_COMMAND", False);
	snprintf(name, sizeof(name), "_XDE_WATCH_S%d", scr->screen);
	selection = XInternAtom(dpy, name, False);

	if ((owner = XGetSelectionOwner(dpy, selection))) {
		if (!options.dryrun) {
			XClientMessageEvent xcm;

			xcm.type = ClientMessage;
			xcm.serial = 0;
			xcm.display = dpy;
			xcm.window = owner;
			xcm.message_type = _XA_XDE_WATCH_COMMAND;
			xcm.format = 32;
			xcm.data.l[0] = XDE_WATCH_RESTART;
			xcm.data.l[1] = 0;
			xcm.data.l[2] = 0;
			xcm.data.l[3] = 0;
			xcm.data.l[4] = 0;

			DPRINTF("sending %s Restart to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_WATCH_COMMAND), owner);
			XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
		} else
			OPRINTF("would send %s Restart to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_WATCH_COMMAND), owner);
		XSync(dpy, False);
		XCloseDisplay(dpy);
		exit(EXIT_SUCCESS);
	} else {
		EPRINTF("No running instance of %s\n", NAME);
		exit(EXIT_FAILURE);
	}
}

static void
do_recheck()
{
	char name[64] = { 0, };
	Atom selection;
	Window owner;

	xde_init_display();
	_XA_XDE_WATCH_COMMAND = XInternAtom(dpy, "_XDE_WATCH_COMMAND", False);
	snprintf(name, sizeof(name), "_XDE_WATCH_S%d", scr->screen);
	selection = XInternAtom(dpy, name, False);

	if ((owner = XGetSelectionOwner(dpy, selection))) {
		if (!options.dryrun) {
			XClientMessageEvent xcm;

			xcm.type = ClientMessage;
			xcm.serial = 0;
			xcm.display = dpy;
			xcm.window = owner;
			xcm.message_type = _XA_XDE_WATCH_COMMAND;
			xcm.format = 32;
			xcm.data.l[0] = XDE_WATCH_RECHECK;
			xcm.data.l[1] = 0;
			xcm.data.l[2] = 0;
			xcm.data.l[3] = 0;
			xcm.data.l[4] = 0;

			DPRINTF("sending %s Recheck to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_WATCH_COMMAND), owner);
			XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
		} else
			OPRINTF("would send %s Recheck to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_WATCH_COMMAND), owner);
		XSync(dpy, False);
		XCloseDisplay(dpy);
		exit(EXIT_SUCCESS);
	} else {
		EPRINTF("No running instance of %s\n", NAME);
		exit(EXIT_FAILURE);
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
    %1$s [options] [-l,--replace]\n\
    %1$s [options] {-q,--quit}\n\
    %1$s [options] {-r,--restart}\n\
    %1$s [options] {-c,--recheck}\n\
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
    %1$s [options] [{-l,--replace}]\n\
    %1$s [options] {-q,--quit}\n\
    %1$s [options] {-r,--restart}\n\
    %1$s [options] {-c,--recheck}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Command options:\n\
    -q, --quit\n\
        ask running instance to quit\n\
    -r, --restart\n\
        ask running instance to restart\n\
    -c, --recheck\n\
        ask running instance to recheck everything\n\
    -l, --replace\n\
        replace running instance with this one\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
Options:\n\
    -R, --remove\n\
        also remove properties when changes occur\n\
    -f, --foreground\n\
        run in the foreground and debug to standard error\n\
    -n, --dryrun\n\
        do not change anything, just print what would be done\n\
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
			{"quit",	no_argument,		NULL, 'q'},
			{"restart",	no_argument,		NULL, 'r'},
			{"recheck",	no_argument,		NULL, 'c'},
			{"remove",	no_argument,		NULL, 'R'},
			{"foreground",	no_argument,		NULL, 'f'},
			{"dryrun",	no_argument,		NULL, 'n'},
			{"replace",	no_argument,		NULL, 'l'},

			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "qrcRfnlD::v::hVCH?", long_options,
				     &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "qrcRfnlDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			if (options.debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;

		case 'q':	/* -q, --quit */
			do_quit();
			exit(EXIT_SUCCESS);
		case 'r':	/* -r, --restart */
			do_restart();
			exit(EXIT_SUCCESS);
		case 'c':	/* -c, --recheck */
			do_recheck();
			exit(EXIT_SUCCESS);
		case 'R':	/* -R, --remove */
			options.remove = True;
			break;
		case 'f':	/* -f, --foreground */
			foreground = True;
			options.debug = 1;
			break;
		case 'n':	/* -n, --dryrun */
			options.dryrun = True;
			if (options.output < 2)
				options.output = 2;
			break;
		case 'l':	/* -l, --replace */
			options.replace = True;
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
			exit(EXIT_SUCCESS);
		case 'V':	/* -V, --version */
			if (options.debug)
				fprintf(stderr, "%s: printing version message\n",
					argv[0]);
			version(argc, argv);
			exit(EXIT_SUCCESS);
		case 'C':	/* -C, --copying */
			if (options.debug)
				fprintf(stderr, "%s: printing copying message\n",
					argv[0]);
			copying(argc, argv);
			exit(EXIT_SUCCESS);
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
	if (optind < argc)
		goto bad_nonopt;
	do_run(argc, argv);
	exit(EXIT_SUCCESS);
}

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
