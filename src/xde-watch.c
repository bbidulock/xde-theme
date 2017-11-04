/*****************************************************************************

 Copyright (c) 2010-2017  Monavacon Limited <http://www.monavacon.com/>
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

static char **saveArgv;
static int saveArgc;

Atom _XA_XDE_DESKTOP_COMMAND;

Bool foreground = True;

typedef enum {
	CommandDefault,
	CommandRun,
	CommandQuit,
	CommandRestart,
	CommandRecheck,
	CommandHelp,
	CommandVersion,
	CommandCopying,
} CommandType;

CommandType command;

enum {
	XDE_DESKTOP_QUIT,
	XDE_DESKTOP_RESTART,
	XDE_DESKTOP_RECHECK,
	XDE_DESKTOP_ARGV,
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

static void
xdeSetProperties(SmcConn smcConn, SmPointer data)
{
	char userID[20], procID[12];
	int i, j, argc = saveArgc;
	char **argv = saveArgv;
	char *cwd = NULL;
	char hint;
	struct passwd *pw;
	SmPropValue *penv = NULL, *prst = NULL, *pcln = NULL;
	SmPropValue propval[11];
	SmProp prop[11];

	SmProp *props[11] = {
		&prop[0], &prop[1], &prop[2], &prop[3], &prop[4], &prop[5],
		&prop[6], &prop[7], &prop[8], &prop[9], &prop[10],
	};

	j = 0;

	/* CloneCommand */
	prop[j].name = SmCloneCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = pcln = calloc(argc, sizeof(*pcln));
	prop[j].num_vals = 0;
	props[j] = &prop[j];
	for (i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-clientId") || !strcmp(argv[i], "-saveFile"))
			i++;
		else {
			prop[j].vals[prop[j].num_vals].value = (SmPointer) argv[i];
			prop[j].vals[prop[j].num_vals++].length = strlen(argv[i]);
		}
	}
	j++;

#if 1
	/* Current Directory */
	prop[j].name = SmCurrentDirectory;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	propval[j].value = NULL;
	propval[j].length = 0;
	cwd = calloc(PATH_MAX + 1, sizeof(propval[j].value[0]));
	if (getcwd(cwd, PATH_MAX)) {
		propval[j].value = cwd;
		propval[j].length = strlen(propval[j].value);
		j++;
	} else {
		free(cwd);
		cwd = NULL;
	}
#endif

#if 0
	/* DiscardCommand */
	prop[j].name = SmDiscardCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	propval[j].value = "/bin/true";
	propval[j].length = strlen("/bin/true");
	j++;
#endif

#if 0
	/* Environment */
	/* XXX: we might want to filter a few out */
	for (i = 0, env = environ; *env; i += 2, env++) ;
	prop[j].name = SmEnvironment;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = penv = calloc(i, sizeof(*penv));
	prop[j].num_vals = i;
	props[j] = &prop[j];
	for (i = 0, env = environ; *env; i += 2, env++) {
		char *equal;
		int len;

		equal = strchrnul(*env, '=');
		len = (int) (*env - equal);
		if (*equal)
			equal++;
		prop[j].vals[i].value = *env;
		prop[j].vals[i].length = len;
		prop[j].vals[i + 1].value = equal;
		prop[j].vals[i + 1].length = strlen(equal);
	}
	j++;
#endif

#if 1
	/* ProcessID */
	prop[j].name = SmProcessID;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	snprintf(procID, sizeof(procID), "%ld", (long) getpid());
	propval[j].value = procID;
	propval[j].length = strlen(procID);
	j++;
#endif

	/* Program */
	prop[j].name = SmProgram;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	propval[j].value = argv[0];
	propval[j].length = strlen(argv[0]);
	j++;

	/* RestartCommand */
	prop[j].name = SmRestartCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = prst = calloc(argc + 4, sizeof(*prst));
	prop[j].num_vals = 0;
	props[j] = &prop[j];
	for (i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-clientId") || !strcmp(argv[i], "-saveFile"))
			i++;
		else {
			prop[j].vals[prop[j].num_vals].value = (SmPointer) argv[i];
			prop[j].vals[prop[j].num_vals++].length = strlen(argv[i]);
		}
	}
	prop[j].vals[prop[j].num_vals].value = (SmPointer) "-clientId";
	prop[j].vals[prop[j].num_vals++].length = 9;
	prop[j].vals[prop[j].num_vals].value = (SmPointer) options.clientId;
	prop[j].vals[prop[j].num_vals++].length = strlen(options.clientId);

	prop[j].vals[prop[j].num_vals].value = (SmPointer) "-saveFile";
	prop[j].vals[prop[j].num_vals++].length = 9;
	prop[j].vals[prop[j].num_vals].value = (SmPointer) options.saveFile;
	prop[j].vals[prop[j].num_vals++].length = strlen(options.saveFile);
	j++;

#if 0
	/* ResignCommand */
	prop[j].name = SmResignCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	propval[j].value = NAME " -q";
	propval[j].length = strlen(NAME " -q");
	j++;
#endif

	/* RestartStyleHint */
	prop[j].name = SmRestartStyleHint;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[0];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	hint = SmRestartImmediately;
	propval[j].value = &hint;
	propval[j].length = 1;
	j++;

#if 0
	/* ShutdownCommand */
	prop[j].name = SmShutdownCommand;
	prop[j].type = SmLISTofARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	propval[j].value = "/bin/true";
	propval[j].length = strlen("/bin/true");
	j++;
#endif

	/* UserID */
	errno = 0;
	prop[j].name = SmUserID;
	prop[j].type = SmARRAY8;
	prop[j].vals = &propval[j];
	prop[j].num_vals = 1;
	props[j] = &prop[j];
	if ((pw = getpwuid(getuid())))
		strncpy(userID, pw->pw_name, sizeof(userID) - 1);
	else {
		EPRINTF("%s: %s\n", "getpwuid()", strerror(errno));
		snprintf(userID, sizeof(userID), "%ld", (long) getuid());
	}
	propval[j].value = userID;
	propval[j].length = strlen(userID);
	j++;

	SmcSetProperties(smcConn, j, props);

	free(cwd);
	free(pcln);
	free(prst);
	free(penv);
}

static Bool saving_yourself;
static Bool session_shutdown;

static void
xdeSaveYourselfPhase2CB(SmcConn smcConn, SmPointer data)
{
	xdeSetProperties(smcConn, data);
	SmcSaveYourselfDone(smcConn, True);
}

static void
xdeSaveYourselfCB(SmcConn smcConn, SmPointer data, int saveType, Bool shutdown,
		  int interactStyle, Bool fast)
{
	if (!(session_shutdown = shutdown)) {
		if (!SmcRequestSaveYourselfPhase2(smcConn, xdeSaveYourselfPhase2CB, data))
			SmcSaveYourselfDone(smcConn, False);
		return;
	}
	/* FIXME: actually save state */
	xdeSetProperties(smcConn, data);
	SmcSaveYourselfDone(smcConn, True);
}

static void
xdeDieCB(SmcConn smcConn, SmPointer data)
{
	SmcCloseConnection(smcConn, 0, NULL);
	session_shutdown = False;
	xde_main_quit((XPointer) XDE_DESKTOP_QUIT);
}

static void
xdeSaveCompleteCB(SmcConn smcConn, SmPointer data)
{
	if (saving_yourself)
		saving_yourself = False;
}

static void
xdeShutdownCancelledCB(SmcConn smcConn, SmPointer data)
{
	session_shutdown = False;
}

static unsigned long xdeCBMask =
    SmcSaveYourselfProcMask | SmcDieProcMask |
    SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask;

static SmcCallbacks xdeCBs = {
	.save_yourself = {
			  .callback = &xdeSaveYourselfCB,
			  .client_data = NULL,
			  },
	.die = {
		.callback = &xdeDieCB,
		.client_data = NULL,
		},
	.save_complete = {
			  .callback = &xdeSaveCompleteCB,
			  .client_data = NULL,
			  },
	.shutdown_cancelled = {
			       .callback = &xdeShutdownCancelledCB,
			       .client_data = NULL,
			       },
};

static void
smc_init(void)
{
	char err[256] = { 0, };
	char *env;

	if (!(env = getenv("SESSION_MANAGER"))) {
		if (options.clientId)
			EPRINTF("clientId provided but no SESSION_MANAGER\n");
		return;
	}
	smcConn = SmcOpenConnection(env, NULL, SmProtoMajor, SmProtoMinor,
				    xdeCBMask, &xdeCBs, options.clientId, &options.clientId,
				    sizeof(err), err);
	if (!smcConn)
		EPRINTF("SmcOpenConnection: %s\n", err);
}

static Bool
wm_event(const XEvent *e)
{
	switch (e->type) {
	case ClientMessage:
		if (e->xclient.message_type == _XA_XDE_DESKTOP_COMMAND) {
			DPRINTF("got _XDE_DESKTOP_COMMAND\n");
			switch (e->xclient.data.l[0]) {
			case XDE_DESKTOP_RECHECK:
				DPRINTF("got _XDE_DESKTOP_COMMAND(Recheck)\n");
				xde_defer_wm_check(0);
				return XDE_EVENT_STOP;
			case XDE_DESKTOP_QUIT:
				DPRINTF("got _XDE_DESKTOP_COMMAND(Quit)\n");
				xde_main_quit((XPointer) XDE_DESKTOP_QUIT);
				return XDE_EVENT_STOP;
			case XDE_DESKTOP_RESTART:
				DPRINTF("got _XDE_DESKTOP_COMMAND(Restart)\n");
				xde_main_quit((XPointer) XDE_DESKTOP_RESTART);
				return XDE_EVENT_STOP;
			case XDE_DESKTOP_ARGV:
				DPRINTF("got _XDE_DESKTOP_COMMAND(Argv)\n");
				if (XGetCommand(dpy, e->xclient.window, &rargv, &rargc)) {
					XDeleteProperty(dpy, e->xclient.window, XA_WM_COMMAND);
					xde_main_quit((XPointer) XDE_DESKTOP_ARGV);
					return XDE_EVENT_STOP;
				}
				break;
			}
		}
		break;
	case SelectionClear:
		if (e->xselectionclear.window == scr->selwin
		    && e->xselectionclear.selection == scr->selection) {
			DPRINTF("%s selection cleared\n", XGetAtomName(dpy, scr->selection));
			xde_main_quit((XPointer) XDE_DESKTOP_QUIT);
			return XDE_EVENT_STOP;

		}
		break;
	}
	return XDE_EVENT_PROPAGATE;
}

static Bool
wm_signal(int signum)
{
	switch (signum) {
	case SIGINT:
		DPRINTF("got SIGINT, shutting down\n");
		break;
	case SIGHUP:
		DPRINTF("got SIGHUP, shutting down\n");
		break;
	case SIGTERM:
		DPRINTF("got SIGTERM, shutting down\n");
		break;
	case SIGQUIT:
		DPRINTF("got SIGQUIT, shutting down\n");
		break;
	default:
		return XDE_EVENT_PROPAGATE;
	}
	xde_main_quit((XPointer) XDE_DESKTOP_QUIT);
	return XDE_EVENT_STOP;
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
			   SubstructureRedirectMask | SubstructureNotifyMask, (XEvent *) &xcm);
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
	smc_init();
	xde_init(&wm_callbacks);
	_XA_XDE_DESKTOP_COMMAND = XInternAtom(dpy, "_XDE_DESKTOP_COMMAND", False);
	for (screen = 0; screen < nscr; screen++) {
		char name[64] = { 0, };

		xde_set_screen(screen);
		snprintf(name, sizeof(name), "_XDE_DESKTOP_S%d", screen);
		scr->selection = XInternAtom(dpy, name, False);
		scr->selwin = XCreateSimpleWindow(dpy, scr->root,
						  DisplayWidth(dpy, screen),
						  DisplayHeight(dpy, screen), 1, 1, 0,
						  BlackPixel(dpy, screen), BlackPixel(dpy, screen));
		XSaveContext(dpy, scr->selwin, ScreenContext, (XPointer) scr);
		XSelectInput(dpy, scr->selwin, StructureNotifyMask | PropertyChangeMask);

		XGrabServer(dpy);
		if ((scr->owner = XGetSelectionOwner(dpy, scr->selection))) {
			XSelectInput(dpy, scr->owner, StructureNotifyMask | PropertyChangeMask);
			XSaveContext(dpy, scr->owner, ScreenContext, (XPointer) scr);
			XSync(dpy, False);
		}
		XUngrabServer(dpy);

		if (!scr->owner || options.replace)
			XSetSelectionOwner(dpy, scr->selection, scr->selwin,
					options.timestamp);
		else {
			EPRINTF("another instance of %s already on screen %d\n", NAME, scr->screen);
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
		ev.xclient.data.l[0] = options.timestamp;	/* FIXME */
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
		xcm.message_type = _XA_XDE_DESKTOP_COMMAND;
		xcm.format = 32;
		xcm.data.l[0] = XDE_DESKTOP_ARGV;
		xcm.data.l[1] = 0;
		xcm.data.l[2] = 0;
		xcm.data.l[3] = 0;
		xcm.data.l[4] = 0;

		DPRINTF("sending %s Argv to 0x%08lx\n",
			XGetAtomName(dpy, _XA_XDE_DESKTOP_COMMAND), owner);
		XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
		XSync(dpy, False);
		if (!XCheckIfEvent(dpy, &xev, owner_died_predicate, (XPointer) owner)) {
			XIfEvent(dpy, &xev, cmd_remove_predicate, (XPointer) owner);
			XDestroyWindow(dpy, mine);
			XCloseDisplay(dpy);
			exit(EXIT_SUCCESS);
		}
		XSetSelectionOwner(dpy, selection, mine, options.timestamp);
		XSync(dpy, False);
	}
#endif
	if (options.startup_id) {
		int l, len = 12 + strlen(options.startup_id);
		XEvent ev = { 0, };
		Window root = DefaultRootWindow(dpy);
		char *msg, *p;

		msg = calloc(len + 1, sizeof(*msg));
		snprintf(msg, len, "remove: ID=%s", options.startup_id);

		ev.xclient.type = ClientMessage;
		ev.xclient.serial = 0;
		ev.xclient.send_event = False;
		ev.xclient.display = dpy;
		ev.xclient.message_type = _XA_NET_STARTUP_INFO_BEGIN;
		ev.xclient.window = scr->selwin;
		ev.xclient.format = 8;

		l = strlen((p = msg)) + 1;
		while (l > 0) {
			strncpy(ev.xclient.data.b, p, 20);
			p += 20;
			l -= 20;
			if (!XSendEvent(dpy, root, False,
					StructureNotifyMask |
					SubstructureNotifyMask |
					SubstructureRedirectMask |
					PropertyChangeMask, &ev))
				EPRINTF("XSendEvent: failed!\n");
			ev.xclient.message_type = _XA_NET_STARTUP_INFO;
		}
		XSync(dpy, False);
	}
	switch ((int) (long) do_startup()) {
	case XDE_DESKTOP_QUIT:
		xde_del_properties();
		exit(EXIT_SUCCESS);
	case XDE_DESKTOP_RESTART:
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
	case XDE_DESKTOP_ARGV:
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
	case XDE_DESKTOP_RECHECK:
		EPRINTF("should not get here\n");
		break;
	}
	xde_del_properties();
	exit(EXIT_FAILURE);
}

void
do_quit(int argc, char *argv[])
{
	char name[64] = { 0, };
	Atom selection;
	Window owner;

	xde_init_display();
	_XA_XDE_DESKTOP_COMMAND = XInternAtom(dpy, "_XDE_DESKTOP_COMMAND", False);
	snprintf(name, sizeof(name), "_XDE_DESKTOP_S%d", scr->screen);
	selection = XInternAtom(dpy, name, False);

	if ((owner = XGetSelectionOwner(dpy, selection))) {
		if (!options.dryrun) {
			XClientMessageEvent xcm;

			xcm.type = ClientMessage;
			xcm.serial = 0;
			xcm.display = dpy;
			xcm.window = owner;
			xcm.message_type = _XA_XDE_DESKTOP_COMMAND;
			xcm.format = 32;
			xcm.data.l[0] = XDE_DESKTOP_QUIT;
			xcm.data.l[1] = 0;
			xcm.data.l[2] = 0;
			xcm.data.l[3] = 0;
			xcm.data.l[4] = 0;

			DPRINTF("sending %s Quit to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_DESKTOP_COMMAND), owner);
			XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
		} else
			OPRINTF("would send %s Quit to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_DESKTOP_COMMAND), owner);

		XSync(dpy, False);
		XCloseDisplay(dpy);
		exit(EXIT_SUCCESS);
	} else {
		EPRINTF("No running instance of %s\n", NAME);
		exit(EXIT_FAILURE);
	}
}

void
do_restart(int argc, char *argv[])
{
	char name[64] = { 0, };
	Atom selection;
	Window owner;

	xde_init_display();
	_XA_XDE_DESKTOP_COMMAND = XInternAtom(dpy, "_XDE_DESKTOP_COMMAND", False);
	snprintf(name, sizeof(name), "_XDE_DESKTOP_S%d", scr->screen);
	selection = XInternAtom(dpy, name, False);

	if ((owner = XGetSelectionOwner(dpy, selection))) {
		if (!options.dryrun) {
			XClientMessageEvent xcm;

			xcm.type = ClientMessage;
			xcm.serial = 0;
			xcm.display = dpy;
			xcm.window = owner;
			xcm.message_type = _XA_XDE_DESKTOP_COMMAND;
			xcm.format = 32;
			xcm.data.l[0] = XDE_DESKTOP_RESTART;
			xcm.data.l[1] = 0;
			xcm.data.l[2] = 0;
			xcm.data.l[3] = 0;
			xcm.data.l[4] = 0;

			DPRINTF("sending %s Restart to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_DESKTOP_COMMAND), owner);
			XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
		} else
			OPRINTF("would send %s Restart to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_DESKTOP_COMMAND), owner);
		XSync(dpy, False);
		XCloseDisplay(dpy);
		exit(EXIT_SUCCESS);
	} else {
		EPRINTF("No running instance of %s\n", NAME);
		exit(EXIT_FAILURE);
	}
}

void
do_recheck(int argc, char *argv[])
{
	char name[64] = { 0, };
	Atom selection;
	Window owner;

	xde_init_display();
	_XA_XDE_DESKTOP_COMMAND = XInternAtom(dpy, "_XDE_DESKTOP_COMMAND", False);
	snprintf(name, sizeof(name), "_XDE_DESKTOP_S%d", scr->screen);
	selection = XInternAtom(dpy, name, False);

	if ((owner = XGetSelectionOwner(dpy, selection))) {
		if (!options.dryrun) {
			XClientMessageEvent xcm;

			xcm.type = ClientMessage;
			xcm.serial = 0;
			xcm.display = dpy;
			xcm.window = owner;
			xcm.message_type = _XA_XDE_DESKTOP_COMMAND;
			xcm.format = 32;
			xcm.data.l[0] = XDE_DESKTOP_RECHECK;
			xcm.data.l[1] = 0;
			xcm.data.l[2] = 0;
			xcm.data.l[3] = 0;
			xcm.data.l[4] = 0;

			DPRINTF("sending %s Recheck to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_DESKTOP_COMMAND), owner);
			XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
		} else
			OPRINTF("would send %s Recheck to 0x%08lx\n",
				XGetAtomName(dpy, _XA_XDE_DESKTOP_COMMAND), owner);
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
Copyright (c) 2010-2017  Monavacon Limited <http://www.monavacon.com/>\n\
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
Copyright (c) 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017  Monavacon Limited.\n\
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
    -a, --assist\n\
        assist a non-conforming window manager\n\
    -b, --background\n\
        run in the background and suppress debugging\n\
    -n, --dryrun\n\
        do not change anything, just print what would be done\n\
    -d, --delay DELAY\n\
	delete DELAY milliseconds after a theme changes before\n\
	applying the theme\n\
    -w, --wait WAIT\n\
        wait WAIT milliseconds after window manager appears or\n\
	changes before applying themes\n\
    -D, --debug [LEVEL]\n\
        increment or set debug LEVEL [default: 0]\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL [default: 1]\n\
        this option may be repeated.\n\
", argv[0]);
}

void
set_defaults(void)
{
	const char *env, *p, *q;
	char *endptr = NULL;
	Time timestamp;
	int level, monitor;

	if ((env = getenv("DESKTOP_STARTUP_ID"))) {
		free(options.startup_id);
		options.startup_id = strdup(env);
		unsetenv("DESKTOP_STARTUP_ID"); /* take away from GTK */
		/* we can get the timestamp from the startup id */
		if ((p = strstr(env, "_TIME"))) {
			timestamp = strtoul(p + 5, &endptr, 10);
			if (!*endptr)
				options.timestamp = timestamp;
		}
		/* we can get the monitor number from the startup id */
		if ((p = strstr(env, "xdg-launch/")) == env &&
		    (p = strchr(p + 11, '/')) && (p = strchr(p + 1, '-')) &&
		    (q = strchr(p + 1, '-'))) {
			monitor = strtoul(p, &endptr, 10);
			if (endptr == q)
				options.monitor = monitor;
		}
	}
	if ((level = strtoul(getenv("XDE_DEBUG") ? : "0", NULL, 0)))
		options.debug = level;
}

int
main(int argc, char *argv[])
{
	CommandType cmd = CommandDefault;

	saveArgc = argc;
	saveArgv = argv;

	set_defaults();

	while (1) {
		int c, val;
		char *endptr = NULL;

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"quit",	no_argument,		NULL, 'q'},
			{"restart",	no_argument,		NULL, 'r'},
			{"recheck",	no_argument,		NULL, 'c'},
			{"remove",	no_argument,		NULL, 'R'},
			{"background",	no_argument,		NULL, 'b'},
			{"dryrun",	no_argument,		NULL, 'n'},
			{"replace",	no_argument,		NULL, 'l'},
			{"assist",	no_argument,		NULL, 'a'},
			{"delay",	required_argument,	NULL, 'd'},
			{"wait",	required_argument,	NULL, 'w'},

			{"clientId",	required_argument,	NULL, '0'},
			{"saveFile",	required_argument,	NULL, '1'},

			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "qrcRbnlad:w:D::v::hVCH?", long_options,
				     &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "qrcRbnlad:w:DvhVC?");
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
			if (command != CommandDefault)
				goto bad_option;
			if (cmd == CommandDefault)
				cmd = CommandQuit;
			command = CommandQuit;
			break;
		case 'r':	/* -r, --restart */
			if (command != CommandDefault)
				goto bad_option;
			if (cmd == CommandDefault)
				cmd = CommandRestart;
			command = CommandRestart;
			break;
		case 'c':	/* -c, --recheck */
			if (command != CommandRecheck)
				goto bad_option;
			if (cmd == CommandDefault)
				cmd = CommandRecheck;
			command = CommandRecheck;
			break;
		case 'R':	/* -R, --remove */
			options.remove = True;
			break;
		case 'b':	/* -b, --background */
			foreground = False;
			options.debug = 0;
			break;
		case 'n':	/* -n, --dryrun */
			options.dryrun = True;
			if (options.output < 2)
				options.output = 2;
			break;
		case 'l':	/* -l, --replace */
			options.replace = True;
			break;
		case 'a':	/* -a, --assist */
			options.assist = True;
			break;
		case 'd':	/* -d, --delay DELAY */
			val = strtoul(optarg, &endptr, 0);
			if (*endptr)
				goto bad_option;
			options.delay = val;
			break;
		case 'w':	/* -w, --wait */
			val = strtoul(optarg, &endptr, 0);
			if (*endptr)
				goto bad_option;
			options.wait = val;
			break;

		case '0':	/* -clientID CLIENTID */
			free(options.clientId);
			options.clientId = strdup(optarg);
			break;
		case '1':	/* -saveFile SAVEFILE */
			free(options.saveFile);
			options.saveFile = strdup(optarg);
			break;

		case 'D':	/* -D, --debug [level] */
			if (options.debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
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
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
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
			cmd = CommandHelp;
			break;
		case 'V':	/* -V, --version */
			if (command != CommandDefault)
				goto bad_option;
			if (cmd == CommandDefault)
				cmd = CommandVersion;
			command = CommandVersion;
			break;
		case 'C':	/* -C, --copying */
			if (command != CommandDefault)
				goto bad_option;
			if (cmd == CommandDefault)
				cmd = CommandCopying;
			command = CommandCopying;
			break;
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
		      bad_nonopt:
			if (options.output || options.debug) {
				if (optind < argc) {
					fprintf(stderr, "%s: syntax error near '", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
					fprintf(stderr, "'\n");
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
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
	switch (cmd) {
	case CommandDefault:
		command = CommandRun;
	case CommandRun:
		DPRINTF("%s: running default\n", argv[0]);
		do_run(argc, argv);
		break;
	case CommandQuit:
		DPRINTF("%s: running quit\n", argv[0]);
		do_quit(argc, argv);
		break;
	case CommandRestart:
		DPRINTF("%s: running restart\n", argv[0]);
		do_restart(argc, argv);
		break;
	case CommandRecheck:
		DPRINTF("%s: running recheck\n", argv[0]);
		do_recheck(argc, argv);
		break;
	case CommandHelp:
		DPRINTF("%s: printing help message\n", argv[0]);
		help(argc, argv);
		break;
	case CommandVersion:
		DPRINTF("%s: printing version message\n", argv[0]);
		version(argc, argv);
		break;
	case CommandCopying:
		DPRINTF("%s: printing copying message\n", argv[0]);
		copying(argc, argv);
		break;
	}
	exit(EXIT_SUCCESS);
}

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
