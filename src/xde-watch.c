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

#if 0
Atom _XA_XDE_WM_HOST;
Atom _XA_XDE_WM_PID;
Atom _XA_XDE_WM_CLASS;
Atom _XA_XDE_WM_CMDLINE;
Atom _XA_XDE_WM_COMMAND;
Atom _XA_XDE_WM_NETWM_SUPPORT;
Atom _XA_XDE_WM_WINWM_SUPPORT;
Atom _XA_XDE_WM_MAKER_SUPPORT;
Atom _XA_XDE_WM_MOTIF_SUPPORT;
Atom _XA_XDE_WM_ICCCM_SUPPORT;
Atom _XA_XDE_WM_REDIR_SUPPORT;
Atom _XA_XDE_WM_RCFILE;
Atom _XA_XDE_WM_PRVDIR;
Atom _XA_XDE_WM_USRDIR;
Atom _XA_XDE_WM_SYSDIR;
Atom _XA_XDE_WM_ETCDIR;
Atom _XA_XDE_WM_STYLENAME;
Atom _XA_XDE_WM_STYLE;
Atom _XA_XDE_WM_STYLEFILE;
Atom _XA_XDE_WM_MENU;
Atom _XA_XDE_WM_ICON;
Atom _XA_XDE_WM_THEME;
Atom _XA_XDE_WM_THEMEFILE;

struct atom_descrip {
	char *name;
	Atom *atom;
};

static struct atom_descrip atom_list[] = {
	{"_XDE_WATCH_COMMAND", &_XA_XDE_WATCH_COMMAND},
	{"_XDE_WM_HOST", &_XA_XDE_WM_HOST},
	{"_XDE_WM_PID", &_XA_XDE_WM_PID},
	{"_XDE_WM_CLASS", &_XA_XDE_WM_CLASS},
	{"_XDE_WM_CMDLINE", &_XA_XDE_WM_CMDLINE},
	{"_XDE_WM_COMMAND", &_XA_XDE_WM_COMMAND},
	{"_XDE_WM_NETWM_SUPPORT", &_XA_XDE_WM_NETWM_SUPPORT},
	{"_XDE_WM_WINWM_SUPPORT", &_XA_XDE_WM_WINWM_SUPPORT},
	{"_XDE_WM_MAKER_SUPPORT", &_XA_XDE_WM_MAKER_SUPPORT},
	{"_XDE_WM_MOTIF_SUPPORT", &_XA_XDE_WM_MOTIF_SUPPORT},
	{"_XDE_WM_ICCCM_SUPPORT", &_XA_XDE_WM_ICCCM_SUPPORT},
	{"_XDE_WM_REDIR_SUPPORT", &_XA_XDE_WM_REDIR_SUPPORT},
	{"_XDE_WM_RCFILE", &_XA_XDE_WM_RCFILE},
	{"_XDE_WM_PRVDIR", &_XA_XDE_WM_PRVDIR},
	{"_XDE_WM_USRDIR", &_XA_XDE_WM_USRDIR},
	{"_XDE_WM_SYSDIR", &_XA_XDE_WM_SYSDIR},
	{"_XDE_WM_ETCDIR", &_XA_XDE_WM_ETCDIR},
	{"_XDE_WM_STYLENAME", &_XA_XDE_WM_STYLENAME},
	{"_XDE_WM_STYLE", &_XA_XDE_WM_STYLE},
	{"_XDE_WM_STYLEFILE", &_XA_XDE_WM_STYLEFILE},
	{"_XDE_WM_MENU", &_XA_XDE_WM_MENU},
	{"_XDE_WM_ICON", &_XA_XDE_WM_ICON},
	{"_XDE_WM_THEME", &_XA_XDE_WM_THEME},
	{"_XDE_WM_THEMEFILE", &_XA_XDE_WM_THEMEFILE},
	{NULL, NULL}
};

static void
intern_atoms()
{
	int i, j, n;
	char **atom_names;
	Atom *atom_values;

	for (i = 0, n = 0; atom_list[i].name; i++)
		if (atom_list[i].atom)
			n++;
	atom_names = calloc(n + 1, sizeof(*atom_names));
	atom_values = calloc(n + 1, sizeof(*atom_values));
	for (i = 0, j = 0; j < n; i++)
		if (atom_list[i].atom)
			atom_names[j++] = atom_list[i].name;
	XInternAtoms(dpy, atom_names, n, False, atom_values);
	for (i = 0, j = 0; j < n; i++)
		if (atom_list[i].atom)
			*atom_list[i].atom = atom_values[j++];
	free(atom_names);
	free(atom_values);
}
#endif

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
			switch (e->xclient.data.l[0]) {
			case XDE_WATCH_RECHECK:
				xde_defer_wm_check(0);
				return XDE_EVENT_STOP;
			case XDE_WATCH_QUIT:
				xde_main_quit((XPointer) XDE_WATCH_QUIT);
				return XDE_EVENT_STOP;
			case XDE_WATCH_RESTART:
				xde_main_quit((XPointer) XDE_WATCH_RESTART);
				return XDE_EVENT_STOP;
			case XDE_WATCH_ARGV:
				if (XGetCommand(dpy, e->xclient.window, &rargv, &rargc)) {
					xde_main_quit((XPointer) XDE_WATCH_ARGV);
					return XDE_EVENT_STOP;
				}
				break;
			}
		}
		break;
	}
	return False;
}

static Bool
wm_signal(int signum)
{
	return XDE_EVENT_PROPAGATE;
}

static Bool
have_property(Atom *list, int n, Atom prop)
{
	int i;

	for (i = 0; i < n; i++)
		if (list[i] == prop)
			return True;
	return False;
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
	int n = 0;
	Atom *props;

	if ((props = XListProperties(dpy, scr->root, &n))) {
		if (!wm || !wm->name || strcasecmp(wm->name, "fluxbox"))
			if (have_property(props, n, _XA_BLACKBOX_PID))
				XDeleteProperty(dpy, scr->root, _XA_BLACKBOX_PID);
		if (!wm || !wm->name || strcasecmp(wm->name, "blackbox"))
			if (have_property(props, n, _XA_BB_THEME))
				XDeleteProperty(dpy, scr->root, _XA_BB_THEME);
		if (!wm || !wm->name || strcasecmp(wm->name, "openbox")) {
			if (have_property(props, n, _XA_OPENBOX_PID))
				XDeleteProperty(dpy, scr->root, _XA_OPENBOX_PID);
			if (have_property(props, n, _XA_OB_THEME))
				XDeleteProperty(dpy, scr->root, _XA_OB_THEME);
		}
		if (!wm || !wm->name || strcasecmp(wm->name, "i3")) {
			if (have_property(props, n, _XA_I3_PID))
				XDeleteProperty(dpy, scr->root, _XA_I3_PID);
			if (have_property(props, n, _XA_I3_CONFIG_PATH))
				XDeleteProperty(dpy, scr->root, _XA_I3_CONFIG_PATH);
			if (have_property(props, n, _XA_I3_SHMLOG_PATH))
				XDeleteProperty(dpy, scr->root, _XA_I3_SHMLOG_PATH);
			if (have_property(props, n, _XA_I3_SOCKET_PATH))
				XDeleteProperty(dpy, scr->root, _XA_I3_SOCKET_PATH);
		}
		if (!wm || !wm->netwm_check) {
			if (have_property(props, n, _XA_NET_ACTIVE_WINDOW))
				XDeleteProperty(dpy, scr->root, _XA_NET_ACTIVE_WINDOW);
			if (have_property(props, n, _XA_NET_CLIENT_LIST))
				XDeleteProperty(dpy, scr->root, _XA_NET_CLIENT_LIST);
			if (have_property(props, n, _XA_NET_CLIENT_LIST_STACKING))
				XDeleteProperty(dpy, scr->root, _XA_NET_CLIENT_LIST_STACKING);
			if (have_property(props, n, _XA_NET_CURRENT_DESKTOP))
				XDeleteProperty(dpy, scr->root, _XA_NET_CURRENT_DESKTOP);
			if (have_property(props, n, _XA_NET_DESKTOP))
				XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP);
			if (have_property(props, n, _XA_NET_DESKTOP_GEOMETRY))
				XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_GEOMETRY);
			if (have_property(props, n, _XA_NET_DESKTOP_LAYOUT))
				XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_LAYOUT);
			if (have_property(props, n, _XA_NET_DESKTOP_MASK))
				XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_MASK);
			if (have_property(props, n, _XA_NET_DESKTOP_NAMES))
				XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_NAMES);
			if (have_property(props, n, _XA_NET_DESKTOP_PIXMAPS))
				XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_PIXMAPS);
			if (have_property(props, n, _XA_NET_DESKTOP_VIEWPORT))
				XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_VIEWPORT);
			if (have_property(props, n, _XA_NET_FULL_PLACEMENT))
				XDeleteProperty(dpy, scr->root, _XA_NET_FULL_PLACEMENT);
			if (have_property(props, n, _XA_NET_FULLSCREEN_MONITORS))
				XDeleteProperty(dpy, scr->root, _XA_NET_FULLSCREEN_MONITORS);
			if (have_property(props, n, _XA_NET_HANDLED_ICONS))
				XDeleteProperty(dpy, scr->root, _XA_NET_HANDLED_ICONS);
			if (have_property(props, n, _XA_NET_ICON_GEOMETRY))
				XDeleteProperty(dpy, scr->root, _XA_NET_ICON_GEOMETRY);
			if (have_property(props, n, _XA_NET_NUMBER_OF_DESKTOPS))
				XDeleteProperty(dpy, scr->root, _XA_NET_NUMBER_OF_DESKTOPS);
			if (have_property(props, n, _XA_NET_PROPERTIES))
				XDeleteProperty(dpy, scr->root, _XA_NET_PROPERTIES);
			if (have_property(props, n, _XA_NET_SHOWING_DESKTOP))
				XDeleteProperty(dpy, scr->root, _XA_NET_SHOWING_DESKTOP);
			if (have_property(props, n, _XA_NET_SUPPORTED))
				XDeleteProperty(dpy, scr->root, _XA_NET_SUPPORTED);
			if (have_property(props, n, _XA_NET_SUPPORTING_WM_CHECK))
				XDeleteProperty(dpy, scr->root, _XA_NET_SUPPORTING_WM_CHECK);
			if (have_property(props, n, _XA_NET_VIRTUAL_POS))
				XDeleteProperty(dpy, scr->root, _XA_NET_VIRTUAL_POS);
			if (have_property(props, n, _XA_NET_VIRTUAL_ROOTS))
				XDeleteProperty(dpy, scr->root, _XA_NET_VIRTUAL_ROOTS);
			if (have_property(props, n, _XA_NET_VISIBLE_DESKTOPS))
				XDeleteProperty(dpy, scr->root, _XA_NET_VISIBLE_DESKTOPS);
			if (have_property(props, n, _XA_NET_WM_NAME))
				XDeleteProperty(dpy, scr->root, _XA_NET_WM_NAME);
			if (have_property(props, n, _XA_NET_WM_PID))
				XDeleteProperty(dpy, scr->root, _XA_NET_WM_PID);
			if (have_property(props, n, _XA_NET_WORKAREA))
				XDeleteProperty(dpy, scr->root, _XA_NET_WORKAREA);
		}
		if (!wm || !wm->winwm_check) {
			if (have_property(props, n, _XA_WIN_AREA))
				XDeleteProperty(dpy, scr->root, _XA_WIN_AREA);
			if (have_property(props, n, _XA_WIN_AREA_COUNT))
				XDeleteProperty(dpy, scr->root, _XA_WIN_AREA_COUNT);
			if (have_property(props, n, _XA_WIN_CLIENT_LIST))
				XDeleteProperty(dpy, scr->root, _XA_WIN_CLIENT_LIST);
			if (have_property(props, n, _XA_WIN_DESKTOP_BUTTON_PROXY))
				XDeleteProperty(dpy, scr->root, _XA_WIN_DESKTOP_BUTTON_PROXY);
			if (have_property(props, n, _XA_WIN_FOCUS))
				XDeleteProperty(dpy, scr->root, _XA_WIN_FOCUS);
			if (have_property(props, n, _XA_WIN_PROTOCOLS))
				XDeleteProperty(dpy, scr->root, _XA_WIN_PROTOCOLS);
			if (have_property(props, n, _XA_WIN_SUPPORTING_WM_CHECK))
				XDeleteProperty(dpy, scr->root, _XA_WIN_SUPPORTING_WM_CHECK);
			if (have_property(props, n, _XA_WIN_WORKSPACE))
				XDeleteProperty(dpy, scr->root, _XA_WIN_WORKSPACE);
			if (have_property(props, n, _XA_WIN_WORKSPACE_COUNT))
				XDeleteProperty(dpy, scr->root, _XA_WIN_WORKSPACE_COUNT);
			if (have_property(props, n, _XA_WIN_WORKSPACE_NAMES))
				XDeleteProperty(dpy, scr->root, _XA_WIN_WORKSPACE_NAMES);
			if (have_property(props, n, _XA_WIN_WORKSPACES))
				XDeleteProperty(dpy, scr->root, _XA_WIN_WORKSPACES);
		}
		if (!wm || !wm->maker_check) {
			if (have_property(props, n, _XA_WINDOWMAKER_NOTICEBOARD))
				XDeleteProperty(dpy, scr->root, _XA_WINDOWMAKER_NOTICEBOARD);
		}
		if (!wm || !wm->motif_check) {
			if (have_property(props, n, _XA_DT_WORKSPACE_CURRENT))
				XDeleteProperty(dpy, scr->root, _XA_DT_WORKSPACE_CURRENT);
			if (have_property(props, n, _XA_DT_WORKSPACE_LIST))
				XDeleteProperty(dpy, scr->root, _XA_DT_WORKSPACE_LIST);
			if (have_property(props, n, _XA_MOTIF_WM_INFO))
				XDeleteProperty(dpy, scr->root, _XA_MOTIF_WM_INFO);
		}
	}
	if (wm) {
		xde_set_text(scr->root, XA_WM_NAME, XStringStyle, wm->name);
		xde_set_text(scr->root, _XA_NET_WM_NAME, XUTF8StringStyle, wm->name);

		xde_set_text(scr->root, XA_WM_CLIENT_MACHINE, XStringStyle, wm->host);
		xde_set_text(scr->root, _XA_XDE_WM_HOST, XUTF8StringStyle, wm->host);

		if (wm->pid) {
			xde_set_cardinal(scr->root, _XA_NET_WM_PID, XA_CARDINAL, wm->pid);
			xde_set_cardinal(scr->root, _XA_XDE_WM_PID, XA_CARDINAL, wm->pid);
		} else {
			if (have_property(props, n, _XA_NET_WM_PID))
				XDeleteProperty(dpy, scr->root, _XA_NET_WM_PID);
			if (have_property(props, n, _XA_XDE_WM_PID))
				XDeleteProperty(dpy, scr->root, _XA_XDE_WM_PID);
		}

		xde_set_text_list(scr->root, XA_WM_CLASS, XStringStyle, (char **)&wm->ch, 2);
		xde_set_text_list(scr->root, _XA_XDE_WM_CLASS, XUTF8StringStyle, (char **) &wm->ch, 2);

		if (wm->cargv)
			xde_set_text_list(scr->root, _XA_XDE_WM_CMDLINE, XUTF8StringStyle, wm->cargv, wm->cargc);
		if (wm->argv)
			xde_set_text_list(scr->root, _XA_XDE_WM_COMMAND, XUTF8StringStyle, wm->argv, wm->argc);
		if (wm->cargv)
			xde_set_text_list(scr->root, XA_WM_COMMAND, XStringStyle, wm->cargv, wm->cargc);
		else if (wm->argv)
			xde_set_text_list(scr->root, XA_WM_COMMAND, XStringStyle, wm->argv, wm->argc);
		else if (have_property(props, n, XA_WM_COMMAND))
			XDeleteProperty(dpy, scr->root, XA_WM_COMMAND);

		if (wm->netwm_check)
			xde_set_window(scr->root, _XA_XDE_WM_NETWM_SUPPORT, XA_WINDOW, wm->netwm_check);
		else if (have_property(props, n, _XA_XDE_WM_NETWM_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_NETWM_SUPPORT);
		if (wm->winwm_check)
			xde_set_window(scr->root, _XA_XDE_WM_WINWM_SUPPORT, XA_WINDOW, wm->winwm_check);
		else if (have_property(props, n, _XA_XDE_WM_WINWM_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_WINWM_SUPPORT);
		if (wm->maker_check)
			xde_set_window(scr->root, _XA_XDE_WM_MAKER_SUPPORT, XA_WINDOW, wm->maker_check);
		else if (have_property(props, n, _XA_XDE_WM_MAKER_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MAKER_SUPPORT);
		if (wm->motif_check)
			xde_set_window(scr->root, _XA_XDE_WM_MOTIF_SUPPORT, XA_WINDOW, wm->motif_check);
		else if (have_property(props, n, _XA_XDE_WM_MOTIF_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MOTIF_SUPPORT);
		if (wm->icccm_check)
			xde_set_window(scr->root, _XA_XDE_WM_ICCCM_SUPPORT, XA_WINDOW, wm->icccm_check);
		else if (have_property(props, n, _XA_XDE_WM_ICCCM_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_ICCCM_SUPPORT);
		if (wm->redir_check)
			xde_set_window(scr->root, _XA_XDE_WM_REDIR_SUPPORT, XA_WINDOW, wm->redir_check);
		else if (have_property(props, n, _XA_XDE_WM_REDIR_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_REDIR_SUPPORT);

	} else {
		if (have_property(props, n, XA_WM_NAME))
			XDeleteProperty(dpy, scr->root, XA_WM_NAME);
		if (have_property(props, n, XA_WM_CLIENT_MACHINE))
			XDeleteProperty(dpy, scr->root, XA_WM_CLIENT_MACHINE);
		if (have_property(props, n, XA_WM_CLASS))
			XDeleteProperty(dpy, scr->root, XA_WM_CLASS);
		if (have_property(props, n, XA_WM_COMMAND))
			XDeleteProperty(dpy, scr->root, XA_WM_COMMAND);

		if (have_property(props, n, _XA_NET_WM_NAME))
			XDeleteProperty(dpy, scr->root, _XA_NET_WM_NAME);
		if (have_property(props, n, _XA_NET_WM_PID))
			XDeleteProperty(dpy, scr->root, _XA_NET_WM_PID);
		if (have_property(props, n, _XA_XDE_WM_NAME))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_NAME);
		if (have_property(props, n, _XA_XDE_WM_NETWM_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_NETWM_SUPPORT);
		if (have_property(props, n, _XA_XDE_WM_WINWM_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_WINWM_SUPPORT);
		if (have_property(props, n, _XA_XDE_WM_MAKER_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MAKER_SUPPORT);
		if (have_property(props, n, _XA_XDE_WM_MOTIF_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MOTIF_SUPPORT);
		if (have_property(props, n, _XA_XDE_WM_ICCCM_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_ICCCM_SUPPORT);
		if (have_property(props, n, _XA_XDE_WM_REDIR_SUPPORT))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_REDIR_SUPPORT);
		if (have_property(props, n, _XA_XDE_WM_PID))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_PID);
		if (have_property(props, n, _XA_XDE_WM_HOST))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_HOST);
		if (have_property(props, n, _XA_XDE_WM_CLASS))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_CLASS);
		if (have_property(props, n, _XA_XDE_WM_CMDLINE))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_CMDLINE);
		if (have_property(props, n, _XA_XDE_WM_COMMAND))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_COMMAND);
		if (have_property(props, n, _XA_XDE_WM_RCFILE))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_RCFILE);
		if (have_property(props, n, _XA_XDE_WM_PRVDIR))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_PRVDIR);
		if (have_property(props, n, _XA_XDE_WM_USRDIR))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_USRDIR);
		if (have_property(props, n, _XA_XDE_WM_SYSDIR))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_SYSDIR);
		if (have_property(props, n, _XA_XDE_WM_ETCDIR))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_ETCDIR);
		if (have_property(props, n, _XA_XDE_WM_STYLEFILE))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_STYLEFILE);
		if (have_property(props, n, _XA_XDE_WM_STYLE))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_STYLE);
		if (have_property(props, n, _XA_XDE_WM_STYLENAME))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_STYLENAME);
		if (have_property(props, n, _XA_XDE_WM_MENU))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MENU);
		if (have_property(props, n, _XA_XDE_WM_ICON))
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_ICON);
	}
	if (props)
		XFree(props);
}

/** @brief window manager style change callback
  *
  * Invoked whenever the window manager style changes, or whenever the window
  * manager changes (regardless of whether the style actually changed).
  */
static void
wm_style_changed(char *newname, char *newstyle, char *newfile)
{
	setting = settings + scr->screen;
	free(setting->stylename);
	setting->stylename = newname ? strdup(newname) : NULL;
	free(setting->style);
	setting->style = newstyle ? strdup(newstyle) : NULL;
	free(setting->stylefile);
	setting->stylefile = newfile ? strdup(newfile) : NULL;
}

/** @brief window manager root menu changed callback
  *
  * Invoked whenever the window manager root menu path changes, or whenever the
  * window manager changes (regardless of whether the path actually changed).
  */
static void
wm_menu_changed(char *newmenu)
{
	setting = settings + scr->screen;
	free(setting->menu);
	setting->menu = newmenu ? strdup(newmenu) : NULL;
}

/** @brief window manager icon changed callback
  *
  * Invoked whenever the window manager icon changes, or whenever the window
  * manager changes (regardless of whether the icon actually changed).
  */
static void
wm_icon_changed(char *newicon)
{
	setting = settings + scr->screen;
	free(setting->icon);
	setting->icon = newicon ? strdup(newicon) : NULL;
}

/** @brief window manager (XDE really) theme changed callback
  *
  * Invoked whenever the theme changes, or whenever the window manager changes
  * (regardless of whether the theme actually changed).
  */
static void
wm_theme_changed(char *newtheme, char *newfile)
{
	setting = settings + scr->screen;
	free(setting->theme);
	setting->theme = newtheme ? strdup(newtheme) : NULL;
	free(setting->themefile);
	setting->themefile = newfile ? strdup(newfile) : NULL;
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
	settings = calloc(nscr, sizeof(settings));
	for (s = 0; s < nscr; s++) {
		xde_set_screen(s);
		xde_recheck_wm();
	}
	return xde_main_loop();
}

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

static void
do_run(int argc, char *argv[])
{
	char name[64] = { 0, };
	Atom selection;
	Window owner;
	Window mine;

//	intern_atoms();
	xde_init(&wm_callbacks);
	snprintf(name, sizeof(name), "_XDE_WATCH_S%d", scr->screen);
	selection = XInternAtom(dpy, name, False);

	mine = XCreateSimpleWindow(dpy, scr->root, 0, 0,
				   1, 1, 0, BlackPixel(dpy, scr->screen),
				   BlackPixel(dpy, scr->screen));
	XSelectInput(dpy, mine, PropertyChangeMask);
	XSetCommand(dpy, mine, argv, argc);

	XGrabServer(dpy);
	if ((owner = XGetSelectionOwner(dpy, selection)))
		XSelectInput(dpy, owner, StructureNotifyMask);
	else
		XSetSelectionOwner(dpy, selection, mine, CurrentTime);
	XSync(dpy, False);
	XUngrabServer(dpy);

	if (owner) {
		XClientMessageEvent xcm;
		XEvent xev;

		xcm.type = ClientMessage;
		xcm.serial = 0;
		xcm.display = dpy;
		xcm.window = mine;
		xcm.message_type = _XA_XDE_WATCH_COMMAND;
		xcm.format = 32;
		xcm.data.l[0] = XDE_WATCH_ARGV;
		xcm.data.l[1] = 0;
		xcm.data.l[2] = 0;
		xcm.data.l[3] = 0;
		xcm.data.l[4] = 0;

		XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
		XSync(dpy, False);
		if (!XCheckIfEvent(dpy, &xev, owner_died_predicate, (XPointer) owner)) {
			XIfEvent(dpy, &xev, cmd_remove_predicate, (XPointer) mine);
			XDestroyWindow(dpy, mine);
			XCloseDisplay(dpy);
			exit(EXIT_SUCCESS);
		}
		XSetSelectionOwner(dpy, selection, mine, CurrentTime);
		XSync(dpy, False);
	}
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
	snprintf(name, sizeof(name), "_XDE_WATCH_S%d", scr->screen);
	selection = XInternAtom(dpy, name, False);

	if ((owner = XGetSelectionOwner(dpy, selection))) {
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

		XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
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

	_XA_XDE_WATCH_COMMAND = XInternAtom(dpy, "_XDE_WATCH_COMMAND", False);
	xde_init_display();
	snprintf(name, sizeof(name), "_XDE_WATCH_S%d", scr->screen);
	selection = XInternAtom(dpy, name, False);

	if ((owner = XGetSelectionOwner(dpy, selection))) {
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

		XSendEvent(dpy, owner, False, NoEventMask, (XEvent *) &xcm);
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
    %1$s [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Command options:\n\
    -q, --quit\n\
        ask running instance to quit\n\
    -r, --restart\n\
        ask running instance to restart\n\
    -R, --remove\n\
        also remove properties when changes occur\n\
    -f, --foreground\n\
        run in the foreground and debug to standard error\n\
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

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"quit",	no_argument,		NULL, 'q'},
			{"restart",	no_argument,		NULL, 'r'},
			{"remove",	no_argument,		NULL, 'R'},
			{"foreground",	no_argument,		NULL, 'f'},

			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "qrRfD::v::hVCH?", long_options,
				     &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "qrRfDvhVC?");
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
		case 'R':	/* -R, --remove */
			options.remove = True;
			break;
		case 'f':	/* -f, --foreground */
			foreground = True;
			options.debug = 1;
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
