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

/* Library functions for XDE. */

#define _GNU_SOURCE
#include "xde.h"
#include <dlfcn.h>

Display *dpy;
int screen;
Window root;
WindowManager *wm;
WmScreen *screens;
WmScreen *scr;
WmScreen *event_scr;
unsigned int nscr;
WmImage *images;
WmDesktop *dsk;

Options options = {
	.debug = 0,
	.output = 1,
	.current = True,
	.menu = False,
	.list = False,
	.set = False,
	.system = True,
	.user = True,
	.link = False,
	.theme = False,
	.dryrun = False,
	.reload = False,
	.screen = -1,
	.style = NULL,
	.wmname = NULL,
	.rcfile = NULL,
	.format = XDE_OUTPUT_HUMAN,
	.grab = False,
	.setroot = False,
	.nomonitor = False,
	.delay = 2000,
	.areas = False,
	.files = NULL,
	.remove = False,
};

static WindowManager *
ref_wm()
{
	if (!(wm = scr->wm)) {
		wm = scr->wm = calloc(1, sizeof(*wm));
		return ref_wm();
	}
	wm->refs++;
	return wm;
}

static void
delete_this_wm(WindowManager *w)
{
	if (w) {
		free(w->host);
		free(w->name);
		free(w->argv);
		if (w->cargv)
			XFreeStringList(w->cargv);
		if (w->ch.res_name)
			XFree(w->ch.res_name);
		if (w->ch.res_class)
			XFree(w->ch.res_class);
		free(w->rcfile);
		free(w->pdir);
		free(w->udir);
		free(w->sdir);
		free(w->edir);
		free(w->stylefile);
		free(w->style);
		free(w->stylename);
		free(w->menu);
		free(w->env);
		if (w->db)
			XrmDestroyDatabase(w->db);
		free(w->xdg_dirs);
		free(w->icon);
	}
}

void
__xde_wm_unref(WindowManager *w)
{
	if (w) {
		if (--w->refs <= 0) {
			delete_this_wm(w);
			w = NULL;
		}
	}
}

__asm__(".symver __xde_wm_unref,xde_wm_unref@@XDE_1.0");

static WindowManager *
unref_wm()
{
	if ((wm = scr->wm)) {
		xde_wm_unref(wm);
		wm = scr->wm = NULL;
	}
	return NULL;
}

static Bool xrm_initialized = False;

static void
init_xrm()
{
	if (!xrm_initialized) {
		xrm_initialized = True;
		XrmInitialize();
	}
}

Atom _XA_BB_THEME;
Atom _XA_BLACKBOX_PID;
Atom _XA_DT_WORKSPACE_CURRENT;
Atom _XA_DT_WORKSPACE_LIST;
Atom _XA_ESETROOT_PMAP_ID;
Atom _XA_GTK_READ_RCFILES;
Atom _XA_I3_CONFIG_PATH;
Atom _XA_I3_PID;
Atom _XA_I3_SHMLOG_PATH;
Atom _XA_I3_SOCKET_PATH;
Atom _XA_ICEWMBG_QUIT;
Atom _XA_MANAGER;
Atom _XA_MOTIF_WM_INFO;

Atom _XA_NET_ACTIVE_WINDOW;
Atom _XA_NET_CLIENT_LIST;
Atom _XA_NET_CLIENT_LIST_STACKING;
Atom _XA_NET_CURRENT_DESKTOP;
Atom _XA_NET_DESKTOP;
Atom _XA_NET_DESKTOP_GEOMETRY;
Atom _XA_NET_DESKTOP_LAYOUT;
Atom _XA_NET_DESKTOP_MASK;
Atom _XA_NET_DESKTOP_NAMES;
Atom _XA_NET_DESKTOP_PIXMAPS;
Atom _XA_NET_DESKTOP_VIEWPORT;
Atom _XA_NET_FULL_PLACEMENT;
Atom _XA_NET_FULLSCREEN_MONITORS;
Atom _XA_NET_HANDLED_ICONS;
Atom _XA_NET_ICON_GEOMETRY;
Atom _XA_NET_NUMBER_OF_DESKTOPS;
Atom _XA_NET_PROPERTIES;
Atom _XA_NET_SHOWING_DESKTOP;
Atom _XA_NET_SUPPORTED;
Atom _XA_NET_SUPPORTING_WM_CHECK;
Atom _XA_NET_VIRTUAL_POS;
Atom _XA_NET_VIRTUAL_ROOTS;
Atom _XA_NET_VISIBLE_DESKTOPS;
Atom _XA_NET_WM_NAME;
Atom _XA_NET_WM_PID;
Atom _XA_NET_WORKAREA;

Atom _XA_OB_THEME;
Atom _XA_OPENBOX_PID;

Atom _XA_WIN_AREA;
Atom _XA_WIN_AREA_COUNT;
Atom _XA_WIN_CLIENT_LIST;
Atom _XA_WIN_DESKTOP_BUTTON_PROXY;
Atom _XA_WIN_FOCUS;
Atom _XA_WIN_PROTOCOLS;
Atom _XA_WIN_SUPPORTING_WM_CHECK;
Atom _XA_WIN_WORKSPACE;
Atom _XA_WIN_WORKSPACE_COUNT;
Atom _XA_WIN_WORKSPACE_NAMES;
Atom _XA_WIN_WORKSPACES;

Atom _XA_WINDOWMAKER_NOTICEBOARD;

Atom _XA_WM_DESKTOP;
Atom _XA_XDE_THEME_NAME;
Atom _XA_XDE_WM_NAME;
Atom _XA_XDE_WM_NETWM_SUPPORT;
Atom _XA_XDE_WM_WINWM_SUPPORT;
Atom _XA_XDE_WM_MAKER_SUPPORT;
Atom _XA_XDE_WM_MOTIF_SUPPORT;
Atom _XA_XDE_WM_ICCCM_SUPPORT;
Atom _XA_XDE_WM_REDIR_SUPPORT;
Atom _XA_XDE_WM_PID;
Atom _XA_XDE_WM_HOST;
Atom _XA_XDE_WM_CLASS;
Atom _XA_XDE_WM_CMDLINE;
Atom _XA_XDE_WM_COMMAND;
Atom _XA_XDE_WM_RCFILE;
Atom _XA_XDE_WM_PRVDIR;
Atom _XA_XDE_WM_USRDIR;
Atom _XA_XDE_WM_SYSDIR;
Atom _XA_XDE_WM_ETCDIR;
Atom _XA_XDE_WM_STYLEFILE;
Atom _XA_XDE_WM_STYLE;
Atom _XA_XDE_WM_STYLENAME;
Atom _XA_XDE_WM_MENU;
Atom _XA_XDE_WM_ICON;
Atom _XA_XDE_WM_THEME;
Atom _XA_XDE_WM_THEMEFILE;
Atom _XA_XROOTPMAP_ID;
Atom _XA_XSETROOT_ID;

static Bool handle_BB_THEME(const XEvent *);
static Bool handle_BLACKBOX_PID(const XEvent *);
static Bool handle_DT_WORKSPACE_CURRENT(const XEvent *);
static Bool handle_DT_WORKSPACE_LIST(const XEvent *);
static Bool handle_ESETROOT_PMAP_ID(const XEvent *);
static Bool handle_GTK_READ_RCFILES(const XEvent *);
static Bool handle_I3_CONFIG_PATH(const XEvent *);
static Bool handle_I3_PID(const XEvent *);
static Bool handle_I3_SHMLOG_PATH(const XEvent *);
static Bool handle_I3_SOCKET_PATH(const XEvent *);
static Bool handle_ICEWMGB_QUIT(const XEvent *);
static Bool handle_MANAGER(const XEvent *);
static Bool handle_MOTIF_WM_INFO(const XEvent *);

static Bool handle_NET_ACTIVE_WINDOW(const XEvent *);
static Bool handle_NET_CLIENT_LIST(const XEvent *);
static Bool handle_NET_CLIENT_LIST_STACKING(const XEvent *);
static Bool handle_NET_CURRENT_DESKTOP(const XEvent *);
static Bool handle_NET_DESKTOP(const XEvent *);
static Bool handle_NET_DESKTOP_GEOMETRY(const XEvent *);
static Bool handle_NET_DESKTOP_LAYOUT(const XEvent *);
static Bool handle_NET_DESKTOP_MASK(const XEvent *);
static Bool handle_NET_DESKTOP_NAMES(const XEvent *);
static Bool handle_NET_DESKTOP_PIXMAPS(const XEvent *);
static Bool handle_NET_DESKTOP_VIEWPORT(const XEvent *);
static Bool handle_NET_FULL_PLACEMENT(const XEvent *);
static Bool handle_NET_FULLSCREEN_MONITORS(const XEvent *);
static Bool handle_NET_HANDLED_ICONS(const XEvent *);
static Bool handle_NET_ICON_GEOMETRY(const XEvent *);
static Bool handle_NET_NUMBER_OF_DESKTOPS(const XEvent *);
static Bool handle_NET_PROPERTIES(const XEvent *);
static Bool handle_NET_SHOWING_DESKTOP(const XEvent *);
static Bool handle_NET_SUPPORTED(const XEvent *);
static Bool handle_NET_SUPPORTING_WM_CHECK(const XEvent *);
static Bool handle_NET_VIRTUAL_POS(const XEvent *);
static Bool handle_NET_VIRTUAL_ROOTS(const XEvent *);
static Bool handle_NET_VISIBLE_DESKTOPS(const XEvent *);
static Bool handle_NET_WM_NAME(const XEvent *);
static Bool handle_NET_WM_PID(const XEvent *);
static Bool handle_NET_WORKAREA(const XEvent *);

static Bool handle_OB_THEME(const XEvent *);
static Bool handle_OPENBOX_PID(const XEvent *);

static Bool handle_WIN_AREA(const XEvent *);
static Bool handle_WIN_AREA_COUNT(const XEvent *);
static Bool handle_WIN_CLIENT_LIST(const XEvent *);
static Bool handle_WIN_DESKTOP_BUTTON_PROXY(const XEvent *);
static Bool handle_WIN_FOCUS(const XEvent *);
static Bool handle_WIN_PROTOCOLS(const XEvent *);
static Bool handle_WIN_SUPPORTING_WM_CHECK(const XEvent *);
static Bool handle_WIN_WORKSPACE(const XEvent *);
static Bool handle_WIN_WORKSPACE_COUNT(const XEvent *);
static Bool handle_WIN_WORKSPACE_NAMES(const XEvent *);
static Bool handle_WIN_WORKSPACES(const XEvent *);

static Bool handle_WINDOWMAKER_NOTICEBOARD(const XEvent *);
static Bool handle_WM_CLASS(const XEvent *);
static Bool handle_WM_CLIENT_MACHINE(const XEvent *);
static Bool handle_WM_COMMAND(const XEvent *);
static Bool handle_WM_DESKTOP(const XEvent *);
static Bool handle_WM_NAME(const XEvent *);
static Bool handle_XDE_THEME_NAME(const XEvent *);
static Bool handle_XROOTPMAP_ID(const XEvent *);
static Bool handle_XSETROOT_ID(const XEvent *);

typedef struct {
	char *name;
	Atom *atom;
	Bool (*handler) (const XEvent *);
	Atom value;
} Atoms;

static Atoms atoms[] = {
	/* *INDENT-OFF* */
	{"_BB_THEME",			&_XA_BB_THEME,			&handle_BB_THEME,			None			},
	{"_BLACKBOX_PID",		&_XA_BLACKBOX_PID,		&handle_BLACKBOX_PID,			None			},
	{"_DT_WORKSPACE_CURRENT",	&_XA_DT_WORKSPACE_CURRENT,	&handle_DT_WORKSPACE_CURRENT,		None			},
	{"_DT_WORKSPACE_LIST",		&_XA_DT_WORKSPACE_LIST,		&handle_DT_WORKSPACE_LIST,		None			},
	{"ESETROOT_PMAP_ID",		&_XA_ESETROOT_PMAP_ID,		&handle_ESETROOT_PMAP_ID,		None			},
	{"_GTK_READ_RCFILES",		&_XA_GTK_READ_RCFILES,		&handle_GTK_READ_RCFILES,		None			},
	{"I3_CONFIG_PATH",		&_XA_I3_CONFIG_PATH,		&handle_I3_CONFIG_PATH,			None			},
	{"I3_PID",			&_XA_I3_PID,			&handle_I3_PID,				None			},
	{"I3_SHMLOG_PATH",		&_XA_I3_SHMLOG_PATH,		&handle_I3_SHMLOG_PATH,			None			},
	{"I3_SOCKET_PATH",		&_XA_I3_SOCKET_PATH,		&handle_I3_SOCKET_PATH,			None			},
	{"_ICEWMBG_QUIT",		&_XA_ICEWMBG_QUIT,		&handle_ICEWMGB_QUIT,			None			},
	{"MANAGER",			&_XA_MANAGER,			&handle_MANAGER,			None			},
	{"_MOTIF_WM_INFO",		&_XA_MOTIF_WM_INFO,		&handle_MOTIF_WM_INFO,			None			},


	{"_NET_ACTIVE_WINDOW",		&_XA_NET_ACTIVE_WINDOW,		&handle_NET_ACTIVE_WINDOW,		None			},
	{"_NET_CLIENT_LIST",		&_XA_NET_CLIENT_LIST,		&handle_NET_CLIENT_LIST,		None			},
	{"_NET_CLIENT_LIST_STACKING",	&_XA_NET_CLIENT_LIST_STACKING,	&handle_NET_CLIENT_LIST_STACKING,	None			},
	{"_NET_CURRENT_DESKTOP",	&_XA_NET_CURRENT_DESKTOP,	&handle_NET_CURRENT_DESKTOP,		None			},
	{"_NET_DESKTOP",		&_XA_NET_DESKTOP,		&handle_NET_DESKTOP,			None			},
	{"_NET_DESKTOP_GEOMETRY",	&_XA_NET_DESKTOP_GEOMETRY,	&handle_NET_DESKTOP_GEOMETRY,		None			},
	{"_NET_DESKTOP_LAYOUT",		&_XA_NET_DESKTOP_LAYOUT,	&handle_NET_DESKTOP_LAYOUT,		None			},
	{"_NET_DESKTOP_MASK",		&_XA_NET_DESKTOP_MASK,		&handle_NET_DESKTOP_MASK,		None			},
	{"_NET_DESKTOP_NAMES",		&_XA_NET_DESKTOP_NAMES,		&handle_NET_DESKTOP_NAMES,		None			},
	{"_NET_DESKTOP_PIXMAPS",	&_XA_NET_DESKTOP_PIXMAPS,	&handle_NET_DESKTOP_PIXMAPS,		None			},
	{"_NET_DESKTOP_VIEWPORT",	&_XA_NET_DESKTOP_VIEWPORT,	&handle_NET_DESKTOP_VIEWPORT,		None			},
	{"_NET_FULL_PLACEMENT",		&_XA_NET_FULL_PLACEMENT,	&handle_NET_FULL_PLACEMENT,		None			},
	{"_NET_FULLSCREEN_MONITORS",	&_XA_NET_FULLSCREEN_MONITORS,	&handle_NET_FULLSCREEN_MONITORS,	None			},
	{"_NET_HANDLED_ICONS",		&_XA_NET_HANDLED_ICONS,		&handle_NET_HANDLED_ICONS,		None			},
	{"_NET_ICON_GEOMETRY",		&_XA_NET_ICON_GEOMETRY,		&handle_NET_ICON_GEOMETRY,		None			},
	{"_NET_NUMBER_OF_DESKTOPS",	&_XA_NET_NUMBER_OF_DESKTOPS,	&handle_NET_NUMBER_OF_DESKTOPS,		None			},
	{"_NET_PROPERTIES",		&_XA_NET_PROPERTIES,		&handle_NET_PROPERTIES,			None			},
	{"_NET_SHOWING_DESKTOP",	&_XA_NET_SHOWING_DESKTOP,	&handle_NET_SHOWING_DESKTOP,		None			},
	{"_NET_SUPPORTED",		&_XA_NET_SUPPORTED,		&handle_NET_SUPPORTED,			None			},
	{"_NET_SUPPORTING_WM_CHECK",	&_XA_NET_SUPPORTING_WM_CHECK,	&handle_NET_SUPPORTING_WM_CHECK,	None			},
	{"_NET_VIRTUAL_POS",		&_XA_NET_VIRTUAL_POS,		&handle_NET_VIRTUAL_POS,		None			},
	{"_NET_VIRTUAL_ROOTS",		&_XA_NET_VIRTUAL_ROOTS,		&handle_NET_VIRTUAL_ROOTS,		None			},
	{"_NET_VISIBLE_DESKTOPS",	&_XA_NET_VISIBLE_DESKTOPS,	&handle_NET_VISIBLE_DESKTOPS,		None			},
	{"_NET_WM_NAME",		&_XA_NET_WM_NAME,		&handle_NET_WM_NAME,			None			},
	{"_NET_WM_PID",			&_XA_NET_WM_PID,		&handle_NET_WM_PID,			None			},
	{"_NET_WORKAREA",		&_XA_NET_WORKAREA,		&handle_NET_WORKAREA,			None			},

	{"_OB_THEME",			&_XA_OB_THEME,			&handle_OB_THEME,			None			},
	{"_OPENBOX_PID",		&_XA_OPENBOX_PID,		&handle_OPENBOX_PID,			None			},

	{"_WIN_AREA",			&_XA_WIN_AREA,			&handle_WIN_AREA,			None			},
	{"_WIN_AREA_COUNT",		&_XA_WIN_AREA_COUNT,		&handle_WIN_AREA_COUNT,			None			},
	{"_WIN_CLIENT_LIST",		&_XA_WIN_CLIENT_LIST,		&handle_WIN_CLIENT_LIST,		None			},
	{"_WIN_DESKTOP_BUTTON_PROXY",	&_XA_WIN_DESKTOP_BUTTON_PROXY,	&handle_WIN_DESKTOP_BUTTON_PROXY,	None			},
	{"_WIN_FOCUS",			&_XA_WIN_FOCUS,			&handle_WIN_FOCUS,			None			},
	{"_WIN_PROTOCOLS",		&_XA_WIN_PROTOCOLS,		&handle_WIN_PROTOCOLS,			None			},
	{"_WIN_SUPPORTING_WM_CHECK",	&_XA_WIN_SUPPORTING_WM_CHECK,	&handle_WIN_SUPPORTING_WM_CHECK,	None			},
	{"_WIN_WORKSPACE",		&_XA_WIN_WORKSPACE,		&handle_WIN_WORKSPACE,			None			},
	{"_WIN_WORKSPACE_COUNT",	&_XA_WIN_WORKSPACE_COUNT,	&handle_WIN_WORKSPACE_COUNT,		None			},
	{"_WIN_WORKSPACE_NAMES",	&_XA_WIN_WORKSPACE_NAMES,	&handle_WIN_WORKSPACE_NAMES,		None			},
	{"_WIN_WORKSPACES",		&_XA_WIN_WORKSPACES,		&handle_WIN_WORKSPACES,			None			},

	{"_WINDOWMAKER_NOTICEBOARD",	&_XA_WINDOWMAKER_NOTICEBOARD,	&handle_WINDOWMAKER_NOTICEBOARD,	None			},
	{"WM_CLASS",			NULL,				&handle_WM_CLASS,			XA_WM_CLASS		},
	{"WM_CLIENT_MACHINE",		NULL,				&handle_WM_CLIENT_MACHINE,		XA_WM_CLIENT_MACHINE	},
	{"WM_COMMAND",			NULL,				&handle_WM_COMMAND,			XA_WM_COMMAND		},
	{"WM_DESKTOP",			&_XA_WM_DESKTOP,		&handle_WM_DESKTOP,			None			},
	{"WM_NAME",			NULL,				&handle_WM_NAME,			XA_WM_NAME		},
	{"_XDE_THEME_NAME",		&_XA_XDE_THEME_NAME,		&handle_XDE_THEME_NAME,			None			},
	{"_XDE_WM_NAME",		&_XA_XDE_WM_NAME,		NULL,					None			},
	{"_XDE_WM_NETWM_SUPPORT",	&_XA_XDE_WM_NETWM_SUPPORT,	NULL,					None			},
	{"_XDE_WM_WINWM_SUPPORT",	&_XA_XDE_WM_WINWM_SUPPORT,	NULL,					None			},
	{"_XDE_WM_MAKER_SUPPORT",	&_XA_XDE_WM_MAKER_SUPPORT,	NULL,					None			},
	{"_XDE_WM_MOTIF_SUPPORT",	&_XA_XDE_WM_MOTIF_SUPPORT,	NULL,					None			},
	{"_XDE_WM_ICCCM_SUPPORT",	&_XA_XDE_WM_ICCCM_SUPPORT,	NULL,					None			},
	{"_XDE_WM_REDIR_SUPPORT",	&_XA_XDE_WM_REDIR_SUPPORT,	NULL,					None			},
	{"_XDE_WM_PID",			&_XA_XDE_WM_PID,		NULL,					None			},
	{"_XDE_WM_HOST",		&_XA_XDE_WM_HOST,		NULL,					None			},
	{"_XDE_WM_CLASS",		&_XA_XDE_WM_CLASS,		NULL,					None			},
	{"_XDE_WM_CMDLINE",		&_XA_XDE_WM_CMDLINE,		NULL,					None			},
	{"_XDE_WM_COMMAND",		&_XA_XDE_WM_COMMAND,		NULL,					None			},
	{"_XDE_WM_RCFILE",		&_XA_XDE_WM_RCFILE,		NULL,					None			},
	{"_XDE_WM_PRVDIR",		&_XA_XDE_WM_PRVDIR,		NULL,					None			},
	{"_XDE_WM_USRDIR",		&_XA_XDE_WM_USRDIR,		NULL,					None			},
	{"_XDE_WM_SYSDIR",		&_XA_XDE_WM_SYSDIR,		NULL,					None			},
	{"_XDE_WM_ETCDIR",		&_XA_XDE_WM_ETCDIR,		NULL,					None			},
	{"_XDE_WM_STYLEFILE",		&_XA_XDE_WM_STYLEFILE,		NULL,					None			},
	{"_XDE_WM_STYLE",		&_XA_XDE_WM_STYLE,		NULL,					None			},
	{"_XDE_WM_STYLENAME",		&_XA_XDE_WM_STYLENAME,		NULL,					None			},
	{"_XDE_WM_THEME",		&_XA_XDE_WM_THEME,		NULL,					None			},
	{"_XDE_WM_THEMEFILE",		&_XA_XDE_WM_THEMEFILE,		NULL,					None			},
	{"_XDE_WM_MENU",		&_XA_XDE_WM_MENU,		NULL,					None			},
	{"_XDE_WM_ICON",		&_XA_XDE_WM_ICON,		NULL,					None			},
	{"_XROOTPMAP_ID",		&_XA_XROOTPMAP_ID,		&handle_XROOTPMAP_ID,			None			},
	{"_XSETROOT_ID",		&_XA_XSETROOT_ID,		&handle_XSETROOT_ID,			None			},
	{NULL,				NULL,				NULL,					None			}
	/* *INDENT-ON* */
};

static void
intern_atoms()
{
	int i, j, n;
	char **atom_names;
	Atom *atom_values;

	for (i = 0, n = 0; atoms[i].name; i++)
		if (atoms[i].atom)
			n++;
	atom_names = calloc(n + 1, sizeof(*atom_names));
	atom_values = calloc(n + 1, sizeof(*atom_values));
	for (i = 0, j = 0; j < n; i++)
		if (atoms[i].atom)
			atom_names[j++] = atoms[i].name;
	XInternAtoms(dpy, atom_names, n, False, atom_values);
	for (i = 0, j = 0; j < n; i++)
		if (atoms[i].atom)
			*atoms[i].atom = atom_values[j++];
	free(atom_names);
	free(atom_values);
}

XContext ScreenContext;

int
error_handler(Display *display, XErrorEvent *xev)
{
	if (options.debug) {
		char msg[80], req[80], num[80], def[80];

		snprintf(num, sizeof(num), "%d", xev->request_code);
		snprintf(def, sizeof(def), "[request_code=%d]", xev->request_code);
		XGetErrorDatabaseText(dpy, "libxde", num, def, req, sizeof(req));
		if (XGetErrorText(dpy, xev->error_code, msg, sizeof(msg)) != Success)
			msg[0] = '\0';
		fprintf(stderr, "X error %s(0x%lx): %s\n", req, xev->resourceid, msg);
	}
	return(0);
}

static void set_screen(WmScreen *s)
{
	scr = s;
	screen = scr->screen;
	root = scr->root;
	wm = scr->wm;
}

void
__xde_set_screen(int s)
{
	set_screen(screens + s);
}

__asm__(".symver __xde_set_screen,xde_set_screen@@XDE_1.0");

static Bool find_screen(Window window)
{
	WmScreen *scrn = NULL;

	if (XFindContext(dpy, window, ScreenContext, (XPointer *)&scrn) == Success && scrn)
		set_screen(scrn);
	return (scrn != NULL) ? True : False;
}

/** @brief initialize X windows display
  */
void
__xde_init_display()
{
	Window dw;
	unsigned int du;
	int i;

	if (!(dpy = XOpenDisplay(0))) {
		EPRINTF("%s\n", "cannot open display");
		exit(127);
	}
	XSetErrorHandler(error_handler);
	OPRINTF("%s\n", "opened display");
	ScreenContext = XUniqueContext();
	intern_atoms();
	nscr = ScreenCount(dpy);
	screens = calloc(nscr, sizeof(*screens));
	for (i = 0; i < nscr; i++) {
		scr = screens + i;
		scr->screen = i;
		scr->root = RootWindow(dpy, i);
		XGetGeometry(dpy, scr->root, &dw, &scr->x, &scr->y,
			     &scr->width, &scr->height, &du, &du);
		XSaveContext(dpy, scr->root, ScreenContext, (XPointer) scr);
		OPRINTF("screen %d root 0x%lx %dx%d+%d+%d\n", scr->screen,
			scr->root, scr->width, scr->height, scr->x, scr->y);
	}
	screen = DefaultScreen(dpy);
	scr = screens + screen;
	root = scr->root;
}

__asm__(".symver __xde_init_display,xde_init_display@@XDE_1.0");

static WmCallbacks *callbacks;
static int defer_timer;

/** @brief obtain defer timer file number
  */
int
__xde_defer_timer(void)
{
	if (!defer_timer) {
		if ((defer_timer = timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC)) == -1) {
			EPRINTF("timerfd_create: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	return(defer_timer);
}

__asm__(".symver __xde_defer_timer,xde_defer_timer@@XDE_1.0");

/** @brief initialize XDE library
  * @param cbs - callbacks to invoke
  */
void
__xde_init(WmCallbacks *cbs) {
	callbacks = cbs;
	xde_defer_timer();
	xde_init_display();
}

__asm__(".symver __xde_init,xde_init@@XDE_1.0");

static Bool
string_compare(char *a, char *b)
{
	if (a) {
		if (!b)
			return False;
		if (strcmp(a, b))
			return False;
	} else if (b)
		return False;
	return True;
}

/** @name Property retrieval functions
  *
  * @{ */

void
__xde_set_text_list(Window win, Atom prop, XICCEncodingStyle style, char **list, long n)
{
	if (!list || !n)
		XDeleteProperty(dpy, win, prop);
	else {
		XTextProperty tp = { NULL, };
		char **strings;
		int i;

		strings = calloc(n, sizeof(*strings));
		for (i = 0; i < n; i++)
			strings[i] = list[i] ? : "";

		if (Xutf8TextListToTextProperty(dpy, strings, n, style, &tp) != Success) {
			EPRINTF("error converting strings\n");
			free(strings);
			return;
		}
		XSetTextProperty(dpy, win, &tp, prop);
		if (tp.value)
			XFree(tp.value);
		free(strings);
	}
}

__asm__(".symver __xde_set_text_list,xde_set_text_list@@XDE_1.0");

char *
__xde_get_text(Window win, Atom prop)
{
	XTextProperty tp = { NULL, };

	XGetTextProperty(dpy, win, &tp, prop);
	if (tp.value) {
		tp.value[tp.nitems + 1] = '\0';
		return (char *) tp.value;
	}
	return NULL;
}

__asm__(".symver __xde_get_text,xde_get_text@@XDE_1.0");

void
__xde_set_text(Window win, Atom prop, XICCEncodingStyle style, char *text)
{
	if (!text)
		XDeleteProperty(dpy, win, prop);
	else {
		XTextProperty tp = { NULL, };

		if (Xutf8TextListToTextProperty(dpy, &text, 1, style, &tp) != Success) {
			EPRINTF("error converting string '%s'\n", text);
			return;
		}
		XSetTextProperty(dpy, win, &tp, prop);
		if (tp.value)
			XFree(tp.value);
	}
}

__asm__(".symver __xde_set_text,xde_set_text@@XDE_1.0");

long *
__xde_get_cardinals(Window win, Atom prop, Atom type, long *n)
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

__asm__(".symver __xde_get_cardinals,xde_get_cardinals@@XDE_1.0");

void
__xde_set_cardinals(Window win, Atom prop, Atom type, long *cards, long n)
{
	XChangeProperty(dpy, win, prop, type, 32, PropModeReplace,
			(unsigned char *)cards, n);
}

__asm__(".symver __xde_set_cardinals,xde_set_cardinals@@XDE_1.0");

Bool
__xde_get_cardinal(Window win, Atom prop, Atom type, long *card_ret)
{
	Bool result = False;
	long *data, n;

	if ((data = xde_get_cardinals(win, prop, type, &n)) && n > 0) {
		*card_ret = data[0];
		result = True;
	}
	if (data)
		XFree(data);
	return result;
}

__asm__(".symver __xde_get_cardinal,xde_get_cardinal@@XDE_1.0");

void
__xde_set_cardinal(Window win, Atom prop, Atom type, long card)
{
	xde_set_cardinals(win, prop, type, &card, 1);
}

__asm__(".symver __xde_set_cardinal,xde_set_cardinal@@XDE_1.0");

Window *
__xde_get_windows(Window win, Atom prop, Atom type, long *n)
{
	return (Window *) xde_get_cardinals(win, prop, type, n);
}

__asm__(".symver __xde_get_windows,xde_get_windows@@XDE_1.0");

void
__xde_set_windows(Window win, Atom prop, Atom type, Window *winds, long n)
{
	xde_set_cardinals(win, prop, type, (long *)winds, n);
}

__asm__(".symver __xde_set_windows,xde_set_windows@@XDE_1.0");

Bool
__xde_get_window(Window win, Atom prop, Atom type, Window *win_ret)
{
	return xde_get_cardinal(win, prop, type, (long *) win_ret);
}

__asm__(".symver __xde_get_window,xde_get_window@@XDE_1.0");

void
__xde_set_window(Window win, Atom prop, Atom type, Window wind)
{
	xde_set_cardinal(win, prop, type, wind);
}

__asm__(".symver __xde_set_window,xde_set_window@@XDE_1.0");

Time *
__xde_get_times(Window win, Atom prop, Atom type, long *n)
{
	return (Time *) xde_get_cardinals(win, prop, type, n);
}

__asm__(".symver __xde_get_times,xde_get_times@@XDE_1.0");

void
__xde_set_times(Window win, Atom prop, Atom type, Time *times, long n)
{
	xde_set_cardinals(win, prop, type, (long *)times, n);
}

__asm__(".symver __xde_set_times,xde_set_times@@XDE_1.0");

Bool
__xde_get_time(Window win, Atom prop, Atom type, Time * time_ret)
{
	return xde_get_cardinal(win, prop, type, (long *) time_ret);
}

__asm__(".symver __xde_get_time,xde_get_time@@XDE_1.0");

void
__xde_set_time(Window win, Atom prop, Atom type, Time time)
{
	xde_set_cardinal(win, prop, type, time);
}

__asm__(".symver __xde_set_time,xde_set_time@@XDE_1.0");

Atom *
__xde_get_atoms(Window win, Atom prop, Atom type, long *n)
{
	return (Atom *) xde_get_cardinals(win, prop, type, n);
}

__asm__(".symver __xde_get_atoms,xde_get_atoms@@XDE_1.0");

void
__xde_set_atoms(Window win, Atom prop, Atom type, Atom *atoms, long n)
{
	xde_set_cardinals(win, prop, type, (long *)atoms, n);
}

__asm__(".symver __xde_set_atoms,xde_set_atoms@@XDE_1.0");

Bool
__xde_get_atom(Window win, Atom prop, Atom type, Atom *atom_ret)
{
	return xde_get_cardinal(win, prop, type, (long *) atom_ret);
}

__asm__(".symver __xde_get_atom,xde_get_atom@@XDE_1.0");

void
__xde_set_atom(Window win, Atom prop, Atom type, Atom atom)
{
	xde_set_cardinal(win, prop, type, atom);
}

__asm__(".symver __xde_set_atom,xde_set_atom@@XDE_1.0");

Pixmap *
__xde_get_pixmaps(Window win, Atom prop, Atom type, long *n)
{
	return (Pixmap *) xde_get_cardinals(win, prop, type, n);
}

__asm__(".symver __xde_get_pixmaps,xde_get_pixmaps@@XDE_1.0");

void
__xde_set_pixmaps(Window win, Atom prop, Atom type, Pixmap *pixmaps, long n)
{
	xde_set_cardinals(win, prop, type, (long *)pixmaps, n);
}

__asm__(".symver __xde_set_pixmaps,xde_set_pixmaps@@XDE_1.0");

Bool
__xde_get_pixmap(Window win, Atom prop, Atom type, Pixmap * pixmap_ret)
{
	return xde_get_cardinal(win, prop, type, (long *) pixmap_ret);
}

__asm__(".symver __xde_get_pixmap,xde_get_pixmap@@XDE_1.0");

void
__xde_set_pixmap(Window win, Atom prop, Atom type, Pixmap pixmap)
{
	xde_set_cardinal(win, prop, type, pixmap);
}

__asm__(".symver __xde_set_pixmap,xde_set_pixmap@@XDE_1.0");

/** @brief Check for recursive window properties
  * @param atom - property name
  * @param type - property type
  * @return Window - the recursive window property or None
  */
Window
check_recursive(Atom atom, Atom type)
{
	Atom real;
	int format;
	unsigned long nitems, after;
	unsigned long *data = NULL;
	Window check;

	OPRINTF("recursive check for atom 0x%lx\n", atom);

	if (XGetWindowProperty(dpy, root, atom, 0L, 1L, False, type, &real,
			       &format, &nitems, &after,
			       (unsigned char **) &data) == Success && format != 0) {
		if (nitems > 0) {
			if ((check = data[0])) {
				XSelectInput(dpy, check,
					     PropertyChangeMask | StructureNotifyMask);
				XSaveContext(dpy, check, ScreenContext, (XPointer) scr);
			}
			XFree(data);
			data = NULL;
		} else {
			if (data)
				XFree(data);
			return None;
		}
		if (XGetWindowProperty(dpy, check, atom, 0L, 1L, False, type, &real,
				       &format, &nitems, &after,
				       (unsigned char **) &data) == Success
		    && format != 0) {
			if (nitems > 0) {
				if (check != (Window) data[0]) {
					XFree(data);
					return None;
				}
			} else {
				if (data)
					XFree(data);
				return None;
			}
			XFree(data);
		} else
			return None;
	} else
		return None;
	return check;
}

Window
check_nonrecursive(Atom atom, Atom type)
{
	Atom real;
	int format;
	unsigned long nitems, after;
	unsigned long *data = NULL;
	Window check = None;

	OPRINTF("non-recursive check for atom 0x%lx\n", atom);

	if (XGetWindowProperty(dpy, root, atom, 0L, 1L, False, type, &real,
			       &format, &nitems, &after,
			       (unsigned char **) &data) == Success && format != 0) {
		if (nitems > 0) {
			if ((check = data[0])) {
				XSelectInput(dpy, check,
					     PropertyChangeMask | StructureNotifyMask);
				XSaveContext(dpy, check, ScreenContext, (XPointer) scr);
			}
		}
		if (data)
			XFree(data);
	}
	return check;
}

static Bool
check_supported(Atom protocols, Atom supported)
{
	Atom real;
	int format;
	unsigned long nitems, after, num = 1;
	unsigned long *data = NULL;
	Bool result = False;

	OPRINTF("check for non-compliant NetWM\n");

      try_harder:
	if (XGetWindowProperty(dpy, root, protocols, 0L, num, False,
			       XA_ATOM, &real, &format, &nitems, &after,
			       (unsigned char **) &data)
	    == Success && format != 0) {
		if (after) {
			num += ((after + 1) >> 2);
			XFree(data);
			goto try_harder;
		}
		if (nitems > 0) {
			unsigned long i;
			Atom *atoms;

			result = True;
			atoms = (Atom *) data;
			for (i = 0; i < nitems; i++) {
				if (atoms[i] == supported) {
					result = False;
					break;
				}
			}
		}
		if (data)
			XFree(data);
	}
	return result;
}

/** @} */

/** @name Checks for window manager support.
  *
  * @{ */

/** @brief Check for a non-compliant EWMH/NetWM window manager.
  *
  * There are quite a few window managers that implement part of the EWMH/NetWM
  * specification but do not fill out _NET_SUPPORTING_WM_CHECK.  This is a big
  * annoyance.  One way to test this is whether there is a _NET_SUPPORTED on the
  * root window that does not include _NET_SUPPORTING_WM_CHECK in its atom list.
  *
  * The only window manager I know of that placed _NET_SUPPORTING_WM_CHECK in
  * the list and did not set the property on the root window was 2bwm, but it
  * has now been fixed.
  *
  * There are others that provide _NET_SUPPORTING_WM_CHECK on the root window
  * but fail to set it recursively.  When _NET_SUPPORTING_WM_CHECK is reported
  * as supported, relax the check to a non-recursive check.  (Case in point is
  * echinus(1)).
  */
static Window
check_netwm_supported()
{
	if (check_supported(_XA_NET_SUPPORTED, _XA_NET_SUPPORTING_WM_CHECK))
		return root;
	return check_nonrecursive(_XA_NET_SUPPORTING_WM_CHECK, XA_WINDOW);
}

/** @brief Check for a EWMH/NetWM compliant window manager.
  *
  */
static Window
check_netwm()
{
	int i = 0;

	do {
		wm->netwm_check = check_recursive(_XA_NET_SUPPORTING_WM_CHECK, XA_WINDOW);
	} while (i++ < 2 && !wm->netwm_check);

	if (wm->netwm_check) {
		XSelectInput(dpy, wm->netwm_check,
			     PropertyChangeMask | StructureNotifyMask);
		XSaveContext(dpy, wm->netwm_check, ScreenContext, (XPointer) scr);
	} else {
		wm->netwm_check = check_netwm_supported();
	}
	return wm->netwm_check;
}

/** @brief Check for a non-compliant GNOME/WinWM window manager.
  *
  * There are quite a few window managers that implement part of the GNOME/WinWM
  * specification but do not fill in the _WIN_SUPPORTING_WM_CHECK.  This is
  * another big annoyance.  One way to test this is whether there is a
  * _WIN_PROTOCOLS on the root window that does not include
  * _WIN_SUPPORTING_WM_CHECK in its list of atoms.
  */
static Window
check_winwm_supported()
{
	if (check_supported(_XA_WIN_PROTOCOLS, _XA_WIN_SUPPORTING_WM_CHECK))
		return root;
	return check_nonrecursive(_XA_WIN_SUPPORTING_WM_CHECK, XA_CARDINAL);
}

/** @brief Check for a GNOME1/WMH/WinWM compliant window manager.
  */
static Window
check_winwm()
{
	int i = 0;

	do {
		wm->winwm_check = check_recursive(_XA_WIN_SUPPORTING_WM_CHECK, XA_CARDINAL);
	} while (i++ < 2 && !wm->winwm_check);

	if (wm->winwm_check) {
		XSelectInput(dpy, wm->winwm_check,
			     PropertyChangeMask | StructureNotifyMask);
		XSaveContext(dpy, wm->winwm_check, ScreenContext, (XPointer) scr);
	} else {
		wm->winwm_check = check_winwm_supported();
	}
	return wm->winwm_check;
}

/** @brief Check for a WindowMaker compliant window manager.
  */
static Window
check_maker()
{
	int i = 0;

	do {
		wm->maker_check = check_recursive(_XA_WINDOWMAKER_NOTICEBOARD, XA_WINDOW);
	} while (i++ < 2 && !wm->maker_check);

	if (wm->maker_check) {
		XSelectInput(dpy, wm->maker_check,
			     PropertyChangeMask | StructureNotifyMask);
		XSaveContext(dpy, wm->maker_check, ScreenContext, (XPointer) scr);
	}
	return wm->maker_check;
}

/** @brief Check for an OSF/Motif compliant window manager.
  */
static Window
check_motif()
{
	int i = 0;
	long *data, n = 0;

	do {
		data = xde_get_cardinals(root, _XA_MOTIF_WM_INFO, AnyPropertyType, &n);
	} while (i++ < 2 && !data);

	if (data && n >= 2)
		wm->motif_check = data[1];
	if (wm->motif_check) {
		XSelectInput(dpy, wm->motif_check,
			     PropertyChangeMask | StructureNotifyMask);
		XSaveContext(dpy, wm->motif_check, ScreenContext, (XPointer) scr);
	}
	return wm->motif_check;
}

/** @brief Check for an ICCCM 2.0 compliant window manager.
  */
static Window
check_icccm()
{
	char buf[32];
	Atom atom;

	snprintf(buf, 32, "WM_S%d", screen);
	if ((atom = XInternAtom(dpy, buf, True)))
		wm->icccm_check = XGetSelectionOwner(dpy, atom);

	if (wm->icccm_check) {
		XSelectInput(dpy, wm->icccm_check,
			     PropertyChangeMask | StructureNotifyMask);
		XSaveContext(dpy, wm->icccm_check, ScreenContext, (XPointer) scr);

	}
	return wm->icccm_check;
}

/** @brief Check whether an ICCCM window manager is present.
  *
  * This pretty much assumes that any ICCCM window manager will select for
  * SubstructureRedirectMask on the root window.
  */
static Window
check_redir()
{
	XWindowAttributes wa;

	OPRINTF("checking direction for screen %d\n", screen);

	wm->redir_check = None;
	if (XGetWindowAttributes(dpy, root, &wa))
		if (wa.all_event_masks & SubstructureRedirectMask)
			wm->redir_check = root;
	return wm->redir_check;
}

static Bool
find_wm_comp()
{
	Bool have_wm = False;

	OPRINTF("checking wm compliance for screen %d\n", screen);

	OPRINTF("checking redirection\n");
	if (check_redir()) {
		have_wm = True;
		OPRINTF("redirection on window 0x%lx\n", wm->redir_check);
	}
	OPRINTF("checking ICCCM 2.0 compliance\n");
	if (check_icccm()) {
		have_wm = True;
		OPRINTF("ICCCM 2.0 window 0x%lx\n", wm->icccm_check);
	}
	OPRINTF("checking OSF/Motif compliance\n");
	if (check_motif()) {
		have_wm = True;
		OPRINTF("OSF/Motif window 0x%lx\n", wm->motif_check);
	}
	OPRINTF("checking WindowMaker compliance\n");
	if (check_maker()) {
		have_wm = True;
		OPRINTF("WindowMaker window 0x%lx\n", wm->maker_check);
	}
	OPRINTF("checking GNOME/WMH compliance\n");
	if (check_winwm()) {
		have_wm = True;
		OPRINTF("GNOME/WMH window 0x%lx\n", wm->winwm_check);
	}
	OPRINTF("checking NetWM/EWMH compliance\n");
	if (check_netwm()) {
		have_wm = True;
		OPRINTF("NetWM/EWMH window 0x%lx\n", wm->netwm_check);
	}

	return have_wm;
}

/** @} */

/** @name /proc filesystem utilities
  *
  * @{ */

static char *
get_proc_file(pid_t pid, char *name, size_t *size)
{
	char *file, *buf;
	FILE *f;
	size_t fsize, read, total;

	if (!pid)
		return NULL;
	OPRINTF("getting process file %s\n", name);
	file = calloc(64, sizeof(*file));
	snprintf(file, 64, "/proc/%d/%s", pid, name);

	if (!(f = fopen(file, "rb"))) {
		EPRINTF("%s\n", strerror(errno));
		free(file);
		*size = 0;
		return NULL;
	}
	for (fsize = 0; fgetc(f) != EOF; fsize++) ;
	OPRINTF("file %s size is %d bytes\n", file, (int) fsize);
	buf = calloc(fsize + 256, sizeof(*buf));
	if (!(f = freopen(file, "rb", f))) {
		EPRINTF("%s\n", strerror(errno));
		free(file);
		free(buf);
		*size = 0;
		return NULL;
	}
	free(file);
	/* read entire file into buffer */
	total = 0;
	while (total < fsize) {
		read = fread(buf + total, 1, fsize - total, f);
		total += read;
		if (total >= fsize)
			break;
		if (ferror(f)) {
			EPRINTF("%s: %s\n", file, strerror(errno));
			free(buf);
			fclose(f);
			*size = 0;
			return NULL;
		}
		if (feof(f))
			break;
	}
	fclose(f);
	*size = total;
	return buf;
}

static char *
get_proc_link(pid_t pid, char *name)
{
	char *link, *buf;

	if (!pid)
		return NULL;
	OPRINTF("getting process link %s\n", name);
	link = calloc(64, sizeof(*link));
	snprintf(link, 64, "/proc/%d/%s", pid, name);
	buf = calloc(PATH_MAX + 1, sizeof(*buf));
	if (readlink(link, buf, PATH_MAX)) {
		EPRINTF("%s: %s\n", link, strerror(errno));
		free(link);
		free(buf);
		return NULL;
	}
	free(link);
	link = strdup(buf);
	free(buf);
	return link;
}

char *
__xde_get_proc_environ(char *name)
{
	char *pos, *end;

	if (!wm->env && (wm->noenv || !wm->pid
			 || !(wm->env = get_proc_file(wm->pid, "environ", &wm->nenv)))) {
		wm->noenv = True;
		goto nope;
	}
	for (pos = wm->env, end = wm->env + wm->nenv; pos < end; pos += strlen(pos) + 1) {
		if (strstr(pos, name) == pos) {
			pos += strlen(name) + 1;	/* +1 for = sign */
			return pos;
		}
	}
      nope:
	return getenv(name);
}

__asm__(".symver __xde_get_proc_environ,xde_get_proc_environ@@XDE_1.0");

char *
get_proc_comm(pid_t pid)
{
	size_t size;
	char *comm;

	if ((comm = get_proc_file(pid, "comm", &size)))
		if (strrchr(comm, '\n'))
			*strrchr(comm, '\n') = '\0';
	return comm;
}

char *
get_proc_exe(pid_t pid)
{
	return get_proc_link(pid, "exe");
}

char *
get_proc_cwd(pid_t pid)
{
	return get_proc_link(pid, "cwd");
}

/** @brief Get the wm's idea of the $XDG_DATA_HOME:$XDG_DATA_DIRS.
  *
  * This is used for window managers that place their theme files in the XDG
  * themes directories, such as Openbox and Metacity, as well as for finding XDE
  * theme files.
  */
void
__xde_get_xdg_dirs()
{
	char *home, *xhome, *xdata, *dirs, *pos, *end, **dir;
	int len, n;

	home = xde_get_proc_environ("HOME") ? : ".";
	xhome = xde_get_proc_environ("XDG_DATA_HOME");
	xdata = xde_get_proc_environ("XDG_DATA_DIRS") ? : "/usr/local/share:/usr/share";

	len = (xhome ? strlen(xhome) : strlen(home) + strlen("/.local/share")) +
	    strlen(xdata) + 2;
	dirs = calloc(len, sizeof(*dirs));
	if (xhome)
		strcpy(dirs, xhome);
	else {
		strcpy(dirs, home);
		strcat(dirs, "/.local/share");
	}
	strcat(dirs, ":");
	strcat(dirs, xdata);
	end = dirs + strlen(dirs);
	for (n = 0, pos = dirs; pos < end;
	     n++, *strchrnul(pos, ':') = '\0', pos += strlen(pos) + 1) ;
	if (wm->xdg_dirs) {
		for (dir = wm->xdg_dirs; *dir; dir++)
			free(*dir);
		free(wm->xdg_dirs);
	}
	wm->xdg_dirs = calloc(n + 1, sizeof(*wm->xdg_dirs));
	for (n = 0, pos = dirs; pos < end; n++, pos += strlen(pos) + 1)
		wm->xdg_dirs[n] = strdup(pos);
	free(dirs);
	return;
}

__asm__(".symver __xde_get_xdg_dirs,xde_get_xdg_dirs@@XDE_1.0");

/** @brief Determine if XDE theme name exists for window manager.
  * @return Bool - True when theme exists; False otherwise.
  */
Bool
__xde_find_theme(char *name, char **filename)
{
	char **dir, *file;
	int len, nlen;
	struct stat st;
	static char *suffix = "/xde/theme.ini";
	static char *subdir = "/themes/";

	if (!wm->xdg_dirs)
		xde_get_xdg_dirs();
	if (!wm->xdg_dirs)
		return False;

	nlen = strlen(subdir) + strlen(name) + strlen(suffix);

	for (dir = wm->xdg_dirs; *dir; dir++) {
		len = strlen(*dir) + nlen + 1;
		file = calloc(len, sizeof(*file));
		strcpy(file, *dir);
		strcat(file, subdir);
		strcat(file, name);
		strcat(file, suffix);
		if (stat(file, &st)) {
			DPRINTF("%s: %s\n", file, strerror(errno));
			free(file);
			continue;
		}
		if (!S_ISREG(st.st_mode)) {
			DPRINTF("%s: not a file\n", file);
			free(file);
			continue;
		}
		if (filename)
			*filename = file;
		else
			free(file);
		return True;
	}
	return False;
}

__asm__(".symver __xde_find_theme,xde_find_theme@@XDE_1.0");

/** @} */

/** @name Window manager detection
  *
  * @{ */

/** @brief Check window for window manager name.
  *
  * Note that pekwm and openbox are setting a null WM_CLASS property on the
  * check window.  fvwm is setting WM_NAME and WM_CLASS property on the check
  * window.  Recent jwm, blackbox and icewm are properly setting _NET_WM_NAME
  * and WM_CLASS on the check window.
  */
static char *
check_name(Window check)
{
	char *name;

	OPRINTF("checking wm name on window 0x%lx\n", check);

	if (!check)
		return NULL;
	if ((name = xde_get_text(check, _XA_NET_WM_NAME)) && name[0])
		goto got_it_xfree;
	if (name)
		XFree(name);
	if ((name = xde_get_text(check, XA_WM_NAME)) && name[0])
		goto got_it_xfree;
	if (name)
		XFree(name);
	if (wm->ch.res_name) {
		XFree(wm->ch.res_name);
		wm->ch.res_name = NULL;
	}
	if (wm->ch.res_class) {
		XFree(wm->ch.res_class);
		wm->ch.res_class = NULL;
	}
	if (XGetClassHint(dpy, check, &wm->ch)) {
		if ((name = wm->ch.res_name) && name[0])
			goto got_it;
		if ((name = wm->ch.res_class) && name[0])
			goto got_it;
	}
	if (wm->cargv) {
		XFreeStringList(wm->cargv);
		wm->cargv = NULL;
		wm->cargc = 0;
	}
	if (XGetCommand(dpy, check, &wm->cargv, &wm->cargc)) {
		if ((name = wm->cargv[0]) && name[0]) {
			name = strrchr(wm->cargv[0], '/') ? : wm->cargv[0];
			goto got_it;
		}
	} else if (XGetCommand(dpy, root, &wm->cargv, &wm->cargc)) {
		if ((name = wm->cargv[0]) && name[0]) {
			name = strrchr(wm->cargv[0], '/') ? : wm->cargv[0];
			goto got_it;
		}
	}
	return NULL;
      got_it:
	wm->name = strdup(name);
	goto first_word;
      got_it_xfree:
	wm->name = strdup(name);
	XFree(name);
      first_word:
	if ((name = strpbrk(wm->name, " \t\n\r:;,.")) && name != wm->name)
		*name = '\0';
	for (name = wm->name; *name; name++)
		*name = tolower(*name);
	OPRINTF("got wm name %s\n", wm->name);
	return wm->name;
}

char *
find_wm_name()
{
	int i;

	OPRINTF("checking wm name on screen %d\n", screen);

	free(wm->name);
	wm->name = NULL;
	for (i = 0; i < CHECK_WINS; i++)
		if (check_name(wm->wins[i]))
			break;
	if (!wm->name) {
		if (wm->maker_check)
			wm->name = strdup("wmaker");
		else if (wm->motif_check)
			wm->name = strdup("mwm");
	} else {
		/* CTWM with the old GNOME support uses the workspace manager window as a 
		   check window. New CTWM is fully NewWM/EWMH compliant. */
		if (!strcmp(wm->name, "workspacemanager")) {
			free(wm->name);
			wm->name = strdup("ctwm");
		}
		/* Some versions of wmx have an error in that they only set the
		   _NET_WM_NAME to the first letter of wmx. */
		if (!strcmp(wm->name, "w")) {
			free(wm->name);
			wm->name = strdup("wmx");
		}
		/* Ahhhh, the strange naming of μwm...  Unfortunately there are several
		   ways to make a μ in utf-8!!! */
		if (!strcmp(wm->name, "\xce\xbcwm") || !strcmp(wm->name, "\xc2\xb5wm")) {
			free(wm->name);
			wm->name = strdup("uwm");
		}
	}
	if (!wm->name)
		OPRINTF("could not find wm name on screen %d\n", screen);
	return wm->name;
}

static char *
check_host(Window check)
{
	char *host;

	OPRINTF("checking wm host on window 0x%lx\n", check);

	if (!check)
		return NULL;
	if ((host = xde_get_text(check, XA_WM_CLIENT_MACHINE)) && host[0])
		goto got_it_xfree;
	if (host)
		XFree(host);
	return NULL;
      got_it_xfree:
	wm->host = strdup(host);
	XFree(host);
	OPRINTF("got wm host %s\n", wm->host);
	return wm->host;
}

static char *
check_same_host()
{
	char buf[66] = { 0, };
	int len1, len2;

	OPRINTF("checking wm same host on screen %d\n", screen);

	if (!wm->host)
		return wm->host;

	/* null out wm->host if it is the same as us */
	gethostname(buf, 64);
	len1 = strlen(wm->host);
	len2 = strlen(buf);
	if (len1 < len2) {
		if (!strncasecmp(wm->host, buf, len1) && buf[len1 + 1] == '.') {
			OPRINTF("%s\n", "host is local host");
			return NULL;
		}
	} else if (len2 > len1) {
		if (!strncasecmp(wm->host, buf, len2) && wm->host[len2 + 1] == '.') {
			OPRINTF("%s\n", "host is local host");
			return NULL;
		}
	} else {
		if (!strcasecmp(wm->host, buf)) {
			OPRINTF("%s\n", "host is local host");
			return NULL;
		}
	}
	OPRINTF("%s\n", "host is remote host");
	return wm->host;
}

static char *
find_wm_host()
{
	int i;

	OPRINTF("finding wm host on screen %d\n", screen);

	free(wm->host);
	wm->host = NULL;

	for (i = 0; i < CHECK_WINS; i++)
		if (check_host(wm->wins[i]))
			break;
	if (!wm->host)
		OPRINTF("could not find wm host on screen %d\n", screen);
	return wm->host;
}

static pid_t
check_pid(Window check)
{
	long pid;

	OPRINTF("checking wm pid on window 0x%lx\n", check);

	if (!check)
		return 0;
	if (xde_get_cardinal(check, _XA_NET_WM_PID, XA_CARDINAL, &pid) && pid)
		goto got_it;
	if (wm->name && !strcasecmp(wm->name, "fluxbox"))
		if (xde_get_cardinal(check, _XA_BLACKBOX_PID, XA_CARDINAL, &pid) && pid)
			goto got_it;
	if (wm->name && !strcasecmp(wm->name, "openbox"))
		if (xde_get_cardinal(check, _XA_OPENBOX_PID, XA_CARDINAL, &pid) && pid)
			goto got_it;
	if (wm->name && !strcasecmp(wm->name, "i3"))
		if (xde_get_cardinal(check, _XA_I3_PID, XA_CARDINAL, &pid) && pid)
			goto got_it;
	return 0;
      got_it:
	wm->pid = pid;
	OPRINTF("got wm pid %ld\n", pid);
	return pid;

}

static pid_t
find_wm_pid()
{
	int i;

	OPRINTF("finding wm pid on screen %d\n", screen);

	wm->pid = 0;
	for (i = 0; i < CHECK_WINS; i++)
		if (check_pid(wm->wins[i]))
			break;
	if (!wm->pid)
		OPRINTF("could not find wm pid on screen %d\n", screen);
	return wm->pid;
}

static char **
check_comm(Window check)
{
	char **argv;
	int argc;

	if (!check)
		return NULL;
	if (XGetCommand(dpy, check, &argv, &argc)) {
		if (wm->cargv)
			XFreeStringList(wm->cargv);
		wm->cargv = argv;
		wm->cargc = argc;
		OPRINTF("got wm command with %d args\n", argc);
		return argv;
	}
	return NULL;
}

static char **
find_wm_comm()
{
	int i;

	OPRINTF("finding wm command on screen %d\n", screen);

	if (wm->cargv) {
		XFreeStringList(wm->cargv);
		wm->cargv = NULL;
		wm->cargc = 0;
	}
	for (i = 0; i < CHECK_WINS; i++)
		if (check_comm(wm->wins[i]))
			break;
	if (!wm->cargv)
		OPRINTF("could not find wm command on screen %d\n", screen);
	return wm->cargv;
}

static char *wm_list[] = { "2bwm", "adwm", "aewm", "afterstep", "awesome",
	"blackbox", "bspwm", "ctwm", "cwm", "dtwm", "dwm", "e16", "echinus",
	"ede", "etwm", "evilwm", "failsafewm", "fluxbox", "flwm", "fvwm",
	"herbstluftwm", "i3", "icewm", "jwm", "kwm", "matwm2", "mcwm",
	"metacity", "mutter", "mwm", "openbox", "pawm", "pekwm", "perlwm",
	"spectrwm", "twm", "twobwm", "uwm", "velox", "vtwm", "waimea", "wind",
	"wm2", "wmaker", "wmii", "wmx", "xdwm", "xfwm", "yeahwm", NULL
};

/** @brief Find the wm process without name or pid.
  *
  * This might be more common that one thinks.  It may be possible to set the
  * style for a simple ICCCM < 2.0 compliant window manager but it needs to be
  * identified using means other than Xlib.  We search for a process with a
  * known window manager command name that has a DISPLAY environment that is
  * the same as ours.
  */
static Bool
find_wm_proc_any()
{
	DIR *dir;
	struct dirent *d;
	pid_t pid;
	char *name, *buf, *pos, *end, *dsp, *dspnum;
	size_t size;
	int i;

	if (wm->host || wm->pid)
		return False;

	if (!(dir = opendir("/proc"))) {
		EPRINTF("/proc: %s\n", strerror(errno));
		return False;
	}
	dsp = getenv("DISPLAY") ? : "";
	dspnum = strdup(dsp);
	if (strrchr(dspnum, '.') && strrchr(dspnum, '.') > strrchr(dspnum, ':'))
		*strrchr(dspnum, '.') = '\0';
	while ((d = readdir(dir))) {
		if (strspn(d->d_name, "0123456789") != strlen(d->d_name))
			continue;
		pid = atoi(d->d_name);
		if (!(name = get_proc_comm(pid))) {
			DPRINTF("no comm for pid %d\n", (int) pid);
			continue;
		}
		OPRINTF("checking if '%s' is a window manager\n", name);
		for (i = 0; wm_list[i]; i++)
			if (!strcasecmp(wm_list[i], name))
				break;
		if (!wm_list[i]) {
			OPRINTF("%s is not a window manager\n", name);
			free(name);
			continue;
		}
		OPRINTF("%s is a window manager\n", name);
		if (!(buf = get_proc_file(pid, "environ", &size))) {
			DPRINTF("no environ for pid %d\n", (int) pid);
			free(name);
			continue;
		}
		OPRINTF("checking DISPLAY for %s\n", name);
		for (pos = buf, end = buf + size; pos < end; pos += strlen(pos) + 1)
			if (!strncmp(pos, "DISPLAY=", 8))
				break;
		if (pos >= end) {
			free(buf);
			OPRINTF("no DISPLAY= for %s\n", name);
			free(name);
			continue;
		}
		OPRINTF("testing %s for %s\n", pos, name);
		pos += strlen("DISPLAY=");
		if (!strcmp(dsp, pos) || !strcmp(dspnum, pos)) {
			free(buf);
			free(wm->name);
			wm->name = name;
			wm->pid = pid;
			break;
		}
		OPRINTF("%s not on %s or %s\n", name, dsp, dspnum);
		free(buf);
		free(name);
	}
	closedir(dir);
	free(dspnum);
	return wm->pid ? True : False;
}

/** @brief Find the wm process by name.
  *
  * We have a window manager name by no process id on the local host.  Find a
  * process on the local host with an command of the same name that has a
  * DISPLAY environment variable and command arguments that indicate that it is
  * running on this screen.  Set the pid from that process.
  *
  * Unfortunately this is the common case for lesser window managers.  They may
  * properly set _NET_WM_NAME on the _NET_SUPPORTING_WM_CHECK window, but do
  * not set _NET_WM_PID.  Currently we just check whether the DISPLAY
  * environment variable of a process with the correct name is identical to
  * ours, which is the common case, particularly when we are run in the same
  * environment as the window manager.  This works in the general case, however,
  * if command arguments to the window manager can select the screen, the wm
  * might not necessarily be running on this screen.
  *
  */
static Bool
find_wm_proc_by_name()
{
	DIR *dir;
	struct dirent *d;
	pid_t pid;
	char *name, *buf, *pos, *end, *dsp, *dspnum;
	size_t size;

	if (!wm->name)
		return False;
	if (!(dir = opendir("/proc"))) {
		EPRINTF("/proc: %s\n", strerror(errno));
		return False;
	}
	dsp = getenv("DISPLAY") ? : "";
	dspnum = strdup(dsp);
	if (strrchr(dspnum, '.') && strrchr(dspnum, '.') > strrchr(dspnum, ':'))
		*strrchr(dspnum, '.') = '\0';
	while ((d = readdir(dir))) {
		if (strspn(d->d_name, "0123456789") != strlen(d->d_name))
			continue;
		pid = atoi(d->d_name);
		if (!(name = get_proc_comm(pid))) {
			DPRINTF("no comm for pid %d\n", (int) pid);
			continue;
		}
		OPRINTF("checking if '%s' is the window manager\n", name);
		if (strcasecmp(name, wm->name)) {
			OPRINTF("%s is not the window manager\n", name);
			free(name);
			continue;
		}
		OPRINTF("%s is the window manager\n", name);
		if (!(buf = get_proc_file(pid, "environ", &size))) {
			DPRINTF("no environ for pid %d\n", (int) pid);
			free(name);
			continue;
		}
		OPRINTF("checking DISPLAY for %s\n", name);
		for (pos = buf, end = buf + size; pos < end; pos += strlen(pos) + 1)
			if (!strncmp(pos, "DISPLAY=", 8))
				break;
		if (pos >= end) {
			free(buf);
			OPRINTF("no DISPLAY= for %s\n", name);
			free(name);
			continue;
		}
		OPRINTF("testing %s for %s\n", pos, name);
		pos += strlen("DISPLAY=");
		if (!strcmp(dsp, pos) || !strcmp(dspnum, pos)) {
			free(buf);
			wm->pid = pid;
			break;
		}
		OPRINTF("%s not on %s or %s\n", name, dsp, dspnum);
		free(buf);
		free(name);
	}
	closedir(dir);
	free(dspnum);
	return wm->pid ? True : False;
}

/** @brief Find the wm process by pid.
  *
  * We have a process id on the local host but no name.  Use the pid to complete
  * the name.
  */
static Bool
find_wm_proc_by_pid()
{
	char *buf;

	if ((buf = get_proc_comm(wm->pid))) {
		free(wm->name);
		wm->name = buf;
		return True;
	}
	return False;
}

/** @brief Check and fill out information for a window manager process.
  */
static Bool
check_proc()
{
	char *buf;
	size_t size;
	Bool have_proc = False;

	OPRINTF("checking wm process for screen %d\n", screen);

	if (!wm->name || (check_same_host() || !wm->pid)) {
		EPRINTF("%s\n", "not enough information to check wm process");
		return have_proc;	/* no can do */
	}
	/* fill out the command line of the process */
	if ((buf = get_proc_file(wm->pid, "cmdline", &size))) {
		char *pos, *end;
		int i;

		free(wm->argv);
		wm->argc = 0;
		for (pos = buf, end = buf + size; pos < end; pos += strlen(pos) + 1)
			wm->argc++;
		wm->argv = calloc(wm->argc + 1, sizeof(*wm->argv));
		for (i = 0, pos = buf, end = buf + size; pos < end;
		     pos += strlen(pos) + 1)
			wm->argv[i++] = strdup(pos);
		free(buf);
		have_proc = True;
		OPRINTF("got cmdline with %d args\n", wm->argc);
	} else
		DPRINTF("could not get process cmdline for pid %ld", wm->pid);
	/* fill out the environment of the process */
	if ((buf = get_proc_file(wm->pid, "environ", &size))) {
		free(wm->env);
		wm->env = buf;
		wm->nenv = size;
		have_proc = True;
		OPRINTF("got environ with %d bytes\n", (int) size);
	} else {
		wm->noenv = True;
		EPRINTF("could not get process environ for pid %ld", wm->pid);
	}
	/* fill out the host name */
	if (!wm->host) {
		wm->host = calloc(66, sizeof(*wm->host));
		gethostname(wm->host, 64);
	}
	if ((buf = xde_get_proc_environ("HOSTNAME"))) {
		free(wm->host);
		wm->host = strdup(buf);
	}
	return have_proc;
}

/** @brief Find the process associated with a window manager.
  */
static Bool
find_wm_proc()
{
	Bool have_proc = False;

	OPRINTF("finding wm process for screen %d\n", screen);

	if (wm->host && check_same_host()) {
		/* not on this host */
		OPRINTF("%s\n", "process is remote");
		have_proc = False;
	} else if (wm->name && wm->pid) {
		OPRINTF("%s\n", "process already detected");
		have_proc = True;
	} else if (wm->name && !wm->pid) {
		/* have a name but no pid */
		OPRINTF("%s\n", "need to find process by name");
		if (!(have_proc = find_wm_proc_by_name())) {
			/* Non-reparenting window managers often are set to mimic LG3D
			   which is the non-reparenting window manager written by Sun in
			   Java because the JVM only knows a fixed list of
			   non-reparenting window managers. spectrwm(1) does this by
			   default.  herbstluftwm(1) is often set to this.  The proper
			   way to handle this is when looking for the process.
			   Unfortunately, at least spectrwm(1) is not nice enough to set
			   _NET_WM_PID for us.

			   NOTE: this approach should also automagically handle some
			   other crazyness in naming; however, it is slightly dangerous
			   as window managers such as waimea(1) and wmaker(1) can hang
			   around after the session has died and be identified first. */
			OPRINTF("%s\n", "need to find process without name or pid");
			have_proc = find_wm_proc_any();
		}
	} else if (wm->name && wm->pid) {
		/* have a pid but no name */
		OPRINTF("%s\n", "need to find process by pid");
		have_proc = find_wm_proc_by_pid();
	} else if (!wm->name && !wm->pid) {
		/* have no pid or name */
		OPRINTF("%s\n", "need to find process without name or pid");
		have_proc = find_wm_proc_any();
	}
	if (have_proc) {
		/* fill out various things from proc */
		OPRINTF("found process for screen %d\n", screen);
		return check_proc();
	}
	return False;
}

/** @brief Check for window manager button proxy
  */
static Bool
find_wm_prox()
{
	OPRINTF("finding wm button proxy for screen %d\n", screen);

	if (xde_get_window(scr->root, _XA_WIN_DESKTOP_BUTTON_PROXY, None, &wm->proxy)) {
		/* might be left over from another wm */
		if (wm->netwm_check && check_supported(_XA_NET_SUPPORTED, _XA_WIN_DESKTOP_BUTTON_PROXY))
			return True;
		if (wm->winwm_check && check_supported(_XA_WIN_PROTOCOLS, _XA_WIN_DESKTOP_BUTTON_PROXY))
			return True;
		DPRINTF("proxy property exists, but no support\n");
		wm->proxy = None;
	}
	return False;
}

static WmOperations *get_wm_ops(void);

/** @brief Check for a window manager on the current screen.
  */
Bool
__xde_check_wm()
{
	OPRINTF("checking wm for screen %d\n", screen);
	unref_wm();
	ref_wm();

	if (find_wm_comp()) {	/* Check for window manager compliance */
		find_wm_name();	/* Check for window manager name */
		find_wm_host();	/* Check for window manager host */
		find_wm_pid();	/* Check for window manager pid */
		find_wm_comm();	/* Check for window manager command */
		find_wm_proc();	/* Check for window manager proc */
		find_wm_prox();	/* Check for window manager button proxy */

		wm->ops = get_wm_ops();
		if (wm->pid) {
			WmScreen *s;
			int i;

			OPRINTF("checking for duplicate wm for screen %d\n", screen);

			for (i = 0; i < nscr; i++) {
				if (i == screen)
					continue;
				s = screens + i;
				if (!s->wm)
					continue;
				if (wm->pid == s->wm->pid
				    && string_compare(wm->host, s->wm->host)) {
					OPRINTF("found duplicate wm on screen %d\n",
						s->screen);
					unref_wm();
					wm = scr->wm = s->wm;
					ref_wm();
				}
			}
		}
		return True;
	}
	unref_wm();
	return False;
}

__asm__(".symver __xde_check_wm,xde_check_wm@@XDE_1.0");

static void
xde_identify_wm_human()
{
	fprintf(stdout, "Name: %s\n", wm->name);
	if (wm->netwm_check)
		fprintf(stdout, "Supports EWMH/NetWM\n");
	if (wm->maker_check)
		fprintf(stdout, "Supports WindowMaker\n");
	if (wm->winwm_check)
		fprintf(stdout, "Supports GNOME/WinWM\n");
	if (wm->motif_check)
		fprintf(stdout, "Supports OSF/Motif\n");
	if (wm->icccm_check)
		fprintf(stdout, "Supports ICCCM 2.0\n");
	if (wm->redir_check)
		fprintf(stdout, "Supports ICCCM\n");
	if (wm->pid)
		fprintf(stdout, "Process Id: %ld\n", wm->pid);
	if (wm->host)
		fprintf(stdout, "Hostname: %s\n", wm->host);
	if (wm->ch.res_class)
		fprintf(stdout, "Resource class: %s\n", wm->ch.res_class);
	if (wm->ch.res_name)
		fprintf(stdout, "Resource name:  %s\n", wm->ch.res_name);
	if (wm->argv && wm->argc) {
		int i, len;
		char *cmd;

		for (len = 0, i = 0; i < wm->argc; i++)
			len += strlen(wm->argv[i]) + 5;
		cmd = calloc(len, sizeof(*cmd));
		for (i = 0; i < wm->argc; i++) {
			strcat(cmd, i ? " '" : "'");
			strcat(cmd, wm->argv[i]);
			strcat(cmd, "'");
		}
		fprintf(stdout, "Process command line: %s\n", cmd);
	}
	if (wm->cargv && wm->cargc) {
		int i, len;
		char *cmd;

		for (len = 0, i = 0; i < wm->cargc; i++)
			len += strlen(wm->cargv[i]) + 5;
		cmd = calloc(len, sizeof(*cmd));
		for (i = 0; i < wm->cargc; i++) {
			strcat(cmd, i ? " '" : "'");
			strcat(cmd, wm->cargv[i]);
			strcat(cmd, "'");
		}
		fprintf(stdout, "Session command: %s\n", cmd);
	}
	if (wm->rcfile)
		fprintf(stdout, "Runtime config file: %s\n", wm->rcfile);
	if (wm->pdir)
		fprintf(stdout, "Private directory: %s\n", wm->pdir);
	if (wm->udir)
		fprintf(stdout, "User directory: %s\n", wm->udir);
	if (wm->sdir)
		fprintf(stdout, "System directory: %s\n", wm->sdir);
	if (wm->edir)
		fprintf(stdout, "Config directory: %s\n", wm->edir);
	if (wm->stylefile)
		fprintf(stdout, "Style file: %s\n", wm->stylefile);
	if (wm->style)
		fprintf(stdout, "Style: %s\n", wm->style);
	if (wm->stylename)
		fprintf(stdout, "Style name: %s\n", wm->stylename);
	if (wm->menu)
		fprintf(stdout, "Menu file: %s\n", wm->menu);
	if (wm->icon)
		fprintf(stdout, "Icon name: %s\n", wm->icon);
	if (scr->theme)
		fprintf(stdout, "Theme: %s\n", scr->theme);
	if (scr->themefile)
		fprintf(stdout, "Theme file: %s\n", scr->themefile);
}

static void
xde_identify_wm_shell()
{
	fprintf(stdout, "XDE_WM_NAME=\"%s\"\n", wm->name);
	if (wm->netwm_check)
		fprintf(stdout, "XDE_WM_NETWM_SUPPORT=0x%lx\n", wm->netwm_check);
	if (wm->maker_check)
		fprintf(stdout, "XDE_WM_MAKER_SUPPORT=0x%lx\n", wm->maker_check);
	if (wm->winwm_check)
		fprintf(stdout, "XDE_WM_WINWM_SUPPORT=0x%lx\n", wm->winwm_check);
	if (wm->motif_check)
		fprintf(stdout, "XDE_WM_MOTIF_SUPPORT=0x%lx\n", wm->motif_check);
	if (wm->icccm_check)
		fprintf(stdout, "XDE_WM_ICCCM_SUPPORT=0x%lx\n", wm->icccm_check);
	if (wm->redir_check)
		fprintf(stdout, "XDE_WM_REDIR_SUPPORT=0x%lx\n", wm->redir_check);
	if (wm->pid)
		fprintf(stdout, "XDE_WM_PID=%ld\n", wm->pid);
	if (wm->host)
		fprintf(stdout, "XDE_WM_HOST=\"%s\"\n", wm->host);
	if (wm->ch.res_name)
		fprintf(stdout, "XDE_WM_RES_NAME=\"%s\"\n", wm->ch.res_name);
	if (wm->ch.res_class)
		fprintf(stdout, "XDE_WM_RES_CLASS=\"%s\"\n", wm->ch.res_class);
	if (wm->argv && wm->argc) {
		int i, len;
		char *cmd;

		for (len = 0, i = 0; i < wm->argc; i++)
			len += strlen(wm->argv[i]) + 5;
		cmd = calloc(len, sizeof(*cmd));
		for (i = 0; i < wm->argc; i++) {
			strcat(cmd, i ? " '" : "'");
			strcat(cmd, wm->argv[i]);
			strcat(cmd, "'");
		}
		fprintf(stdout, "XDE_WM_CMDLINE=(%s)\n", cmd);
	}
	if (wm->cargv && wm->cargc) {
		int i, len;
		char *cmd;

		for (len = 0, i = 0; i < wm->cargc; i++)
			len += strlen(wm->cargv[i]) + 5;
		cmd = calloc(len, sizeof(*cmd));
		for (i = 0; i < wm->cargc; i++) {
			strcat(cmd, i ? " '" : "'");
			strcat(cmd, wm->cargv[i]);
			strcat(cmd, "'");
		}
		fprintf(stdout, "XDE_WM_COMMAND=(%s)\n", cmd);
	}
	if (wm->rcfile)
		fprintf(stdout, "XDE_WM_RCFILE=\"%s\"\n", wm->rcfile);
	if (wm->pdir)
		fprintf(stdout, "XDE_WM_PRVDIR=\"%s\"\n", wm->pdir);
	if (wm->udir)
		fprintf(stdout, "XDE_WM_USRDIR=\"%s\"\n", wm->udir);
	if (wm->sdir)
		fprintf(stdout, "XDE_WM_SYSDIR=\"%s\"\n", wm->sdir);
	if (wm->edir)
		fprintf(stdout, "XDE_WM_ETCDIR=\"%s\"\n", wm->edir);
	if (wm->stylefile)
		fprintf(stdout, "XDE_WM_STYLEFILE=\"%s\"\n", wm->stylefile);
	if (wm->style)
		fprintf(stdout, "XDE_WM_STYLE=\"%s\"\n", wm->style);
	if (wm->stylename)
		fprintf(stdout, "XDE_WM_STYLENAME=\"%s\"\n", wm->stylename);
	if (wm->menu)
		fprintf(stdout, "XDE_WM_MENU=\"%s\"\n", wm->menu);
	if (wm->icon)
		fprintf(stdout, "XDE_WM_ICON=\"%s\"\n", wm->icon);
	if (scr->theme)
		fprintf(stdout, "XDE_WM_THEME=\"%s\"\n", scr->theme);
	if (scr->themefile)
		fprintf(stdout, "XDE_WM_THEMEFILE=\"%s\"\n", scr->themefile);
}

static void
xde_identify_wm_perl()
{
	fprintf(stdout, "{\n");
	fprintf(stdout, "\tXDE_WM_NAME =>'%s',\n", wm->name);
	if (wm->netwm_check)
		fprintf(stdout, "\tXDE_WM_NETWM_SUPPORT => 0x%lx,\n", wm->netwm_check);
	if (wm->maker_check)
		fprintf(stdout, "\tXDE_WM_MAKER_SUPPORT => 0x%lx,\n", wm->maker_check);
	if (wm->winwm_check)
		fprintf(stdout, "\tXDE_WM_WINWM_SUPPORT => 0x%lx,\n", wm->winwm_check);
	if (wm->motif_check)
		fprintf(stdout, "\tXDE_WM_MOTIF_SUPPORT => 0x%lx,\n", wm->motif_check);
	if (wm->icccm_check)
		fprintf(stdout, "\tXDE_WM_ICCCM_SUPPORT => 0x%lx,\n", wm->icccm_check);
	if (wm->redir_check)
		fprintf(stdout, "\tXDE_WM_REDIR_SUPPORT => 0x%lx,\n", wm->redir_check);
	if (wm->pid)
		fprintf(stdout, "\tXDE_WM_PID => %ld,\n", wm->pid);
	if (wm->host)
		fprintf(stdout, "\tXDE_WM_HOST => '%s',\n", wm->host);
	if (wm->ch.res_name)
		fprintf(stdout, "\tXDE_WM_RES_NAME => '%s',\n", wm->ch.res_name);
	if (wm->ch.res_class)
		fprintf(stdout, "\tXDE_WM_RES_CLASS => '%s',\n", wm->ch.res_class);
	if (wm->argv && wm->argc) {
		int i, len;
		char *cmd;

		for (len = 0, i = 0; i < wm->argc; i++)
			len += strlen(wm->argv[i]) + 5;
		cmd = calloc(len, sizeof(*cmd));
		for (i = 0; i < wm->argc; i++) {
			strcat(cmd, i ? ", '" : "'");
			strcat(cmd, wm->argv[i]);
			strcat(cmd, "'");
		}
		fprintf(stdout, "\tXDE_WM_CMDLINE => [ %s ],\n", cmd);
	}
	if (wm->cargv && wm->cargc) {
		int i, len;
		char *cmd;

		for (len = 0, i = 0; i < wm->cargc; i++)
			len += strlen(wm->cargv[i]) + 5;
		cmd = calloc(len, sizeof(*cmd));
		for (i = 0; i < wm->cargc; i++) {
			strcat(cmd, i ? ", '" : "'");
			strcat(cmd, wm->cargv[i]);
			strcat(cmd, "'");
		}
		fprintf(stdout, "\tXDE_WM_COMMAND => [ %s ],\n", cmd);
	}
	if (wm->rcfile)
		fprintf(stdout, "\tXDE_WM_RCFILE => '%s',\n", wm->rcfile);
	if (wm->pdir)
		fprintf(stdout, "\tXDE_WM_PRVDIR => '%s',\n", wm->pdir);
	if (wm->udir)
		fprintf(stdout, "\tXDE_WM_USRDIR => '%s',\n", wm->udir);
	if (wm->sdir)
		fprintf(stdout, "\tXDE_WM_SYSDIR => '%s',\n", wm->sdir);
	if (wm->edir)
		fprintf(stdout, "\tXDE_WM_ETCDIR => '%s',\n", wm->edir);
	if (wm->stylefile)
		fprintf(stdout, "\tXDE_WM_STYLEFILE => '%s',\n", wm->stylefile);
	if (wm->style)
		fprintf(stdout, "\tXDE_WM_STYLE => '%s',\n", wm->style);
	if (wm->stylename)
		fprintf(stdout, "\tXDE_WM_STYLENAME => '%s',\n", wm->stylename);
	if (wm->menu)
		fprintf(stdout, "\tXDE_WM_MENU => '%s',\n", wm->menu);
	if (wm->icon)
		fprintf(stdout, "\tXDE_WM_ICON => '%s',\n", wm->icon);
	if (scr->theme)
		fprintf(stdout, "\tXDE_WM_THEME => '%s',\n", scr->theme);
	if (scr->themefile)
		fprintf(stdout, "\tXDE_WM_THEMEFILE => '%s',\n", scr->themefile);
	fprintf(stdout, "}\n");
}

/** @brief set XDE properties on the root window for the current screen
  */
void
__xde_set_properties()
{
	if (options.remove) {
		if (!wm || !wm->name || strcasecmp(wm->name, "fluxbox")) {
			XDeleteProperty(dpy, scr->root, _XA_BLACKBOX_PID);
		}
		if (!wm || !wm->name || strcasecmp(wm->name, "blackbox")) {
			XDeleteProperty(dpy, scr->root, _XA_BB_THEME);
		}
		if (!wm || !wm->name || strcasecmp(wm->name, "openbox")) {
			XDeleteProperty(dpy, scr->root, _XA_OPENBOX_PID);
			XDeleteProperty(dpy, scr->root, _XA_OB_THEME);
		}
		if (!wm || !wm->name || strcasecmp(wm->name, "i3")) {
			XDeleteProperty(dpy, scr->root, _XA_I3_PID);
			XDeleteProperty(dpy, scr->root, _XA_I3_CONFIG_PATH);
			XDeleteProperty(dpy, scr->root, _XA_I3_SHMLOG_PATH);
			XDeleteProperty(dpy, scr->root, _XA_I3_SOCKET_PATH);
		}
		if (!wm || !wm->netwm_check) {
			XDeleteProperty(dpy, scr->root, _XA_NET_ACTIVE_WINDOW);
			XDeleteProperty(dpy, scr->root, _XA_NET_CLIENT_LIST);
			XDeleteProperty(dpy, scr->root, _XA_NET_CLIENT_LIST_STACKING);
			XDeleteProperty(dpy, scr->root, _XA_NET_CURRENT_DESKTOP);
			XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP);
			XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_GEOMETRY);
			XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_LAYOUT);
			XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_MASK);
			XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_NAMES);
			XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_PIXMAPS);
			XDeleteProperty(dpy, scr->root, _XA_NET_DESKTOP_VIEWPORT);
			XDeleteProperty(dpy, scr->root, _XA_NET_FULL_PLACEMENT);
			XDeleteProperty(dpy, scr->root, _XA_NET_FULLSCREEN_MONITORS);
			XDeleteProperty(dpy, scr->root, _XA_NET_HANDLED_ICONS);
			XDeleteProperty(dpy, scr->root, _XA_NET_ICON_GEOMETRY);
			XDeleteProperty(dpy, scr->root, _XA_NET_NUMBER_OF_DESKTOPS);
			XDeleteProperty(dpy, scr->root, _XA_NET_PROPERTIES);
			XDeleteProperty(dpy, scr->root, _XA_NET_SHOWING_DESKTOP);
			XDeleteProperty(dpy, scr->root, _XA_NET_SUPPORTED);
			XDeleteProperty(dpy, scr->root, _XA_NET_SUPPORTING_WM_CHECK);
			XDeleteProperty(dpy, scr->root, _XA_NET_VIRTUAL_POS);
			XDeleteProperty(dpy, scr->root, _XA_NET_VIRTUAL_ROOTS);
			XDeleteProperty(dpy, scr->root, _XA_NET_VISIBLE_DESKTOPS);
			XDeleteProperty(dpy, scr->root, _XA_NET_WM_NAME);
			XDeleteProperty(dpy, scr->root, _XA_NET_WM_PID);
			XDeleteProperty(dpy, scr->root, _XA_NET_WORKAREA);
		}
		if (!wm || !wm->winwm_check) {
			XDeleteProperty(dpy, scr->root, _XA_WIN_AREA);
			XDeleteProperty(dpy, scr->root, _XA_WIN_AREA_COUNT);
			XDeleteProperty(dpy, scr->root, _XA_WIN_CLIENT_LIST);
			XDeleteProperty(dpy, scr->root, _XA_WIN_DESKTOP_BUTTON_PROXY);
			XDeleteProperty(dpy, scr->root, _XA_WIN_FOCUS);
			XDeleteProperty(dpy, scr->root, _XA_WIN_PROTOCOLS);
			XDeleteProperty(dpy, scr->root, _XA_WIN_SUPPORTING_WM_CHECK);
			XDeleteProperty(dpy, scr->root, _XA_WIN_WORKSPACE);
			XDeleteProperty(dpy, scr->root, _XA_WIN_WORKSPACE_COUNT);
			XDeleteProperty(dpy, scr->root, _XA_WIN_WORKSPACE_NAMES);
			XDeleteProperty(dpy, scr->root, _XA_WIN_WORKSPACES);
		}
		if (!wm || !wm->maker_check) {
			XDeleteProperty(dpy, scr->root, _XA_WINDOWMAKER_NOTICEBOARD);
		}
		if (!wm || !wm->motif_check) {
			XDeleteProperty(dpy, scr->root, _XA_DT_WORKSPACE_CURRENT);
			XDeleteProperty(dpy, scr->root, _XA_DT_WORKSPACE_LIST);
			XDeleteProperty(dpy, scr->root, _XA_MOTIF_WM_INFO);
		}
	}
	if (wm) {
		xde_set_text(scr->root, XA_WM_NAME, XStringStyle, wm->name);
		xde_set_text(scr->root, _XA_NET_WM_NAME, XUTF8StringStyle, wm->name);
		xde_set_text(scr->root, _XA_XDE_WM_NAME, XUTF8StringStyle, wm->name);

		xde_set_text(scr->root, XA_WM_CLIENT_MACHINE, XStringStyle, wm->host);
		xde_set_text(scr->root, _XA_XDE_WM_HOST, XUTF8StringStyle, wm->host);

		if (wm->pid) {
			xde_set_cardinal(scr->root, _XA_NET_WM_PID, XA_CARDINAL, wm->pid);
			xde_set_cardinal(scr->root, _XA_XDE_WM_PID, XA_CARDINAL, wm->pid);
		} else {
			XDeleteProperty(dpy, scr->root, _XA_NET_WM_PID);
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_PID);
		}

		xde_set_text_list(scr->root, XA_WM_CLASS, XStringStyle, (char **) &wm->ch, 2);
		if (wm->cargv)
			xde_set_text_list(scr->root, XA_WM_COMMAND, XStringStyle, wm->cargv, wm->cargc);
		else if (wm->argv)
			xde_set_text_list(scr->root, XA_WM_COMMAND, XStringStyle, wm->argv, wm->argc);
		else
			XDeleteProperty(dpy, scr->root, XA_WM_COMMAND);


		if (wm->netwm_check)
			xde_set_window(scr->root, _XA_XDE_WM_NETWM_SUPPORT, XA_WINDOW,
				       wm->netwm_check);
		else
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_NETWM_SUPPORT);
		if (wm->winwm_check)
			xde_set_window(scr->root, _XA_XDE_WM_WINWM_SUPPORT, XA_WINDOW,
				       wm->winwm_check);
		else
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_WINWM_SUPPORT);
		if (wm->maker_check)
			xde_set_window(scr->root, _XA_XDE_WM_MAKER_SUPPORT, XA_WINDOW,
				       wm->maker_check);
		else
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MAKER_SUPPORT);
		if (wm->motif_check)
			xde_set_window(scr->root, _XA_XDE_WM_MOTIF_SUPPORT, XA_WINDOW,
				       wm->motif_check);
		else
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MOTIF_SUPPORT);
		if (wm->icccm_check)
			xde_set_window(scr->root, _XA_XDE_WM_ICCCM_SUPPORT, XA_WINDOW,
				       wm->icccm_check);
		else
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_ICCCM_SUPPORT);
		if (wm->redir_check)
			xde_set_window(scr->root, _XA_XDE_WM_REDIR_SUPPORT, XA_WINDOW,
				       wm->redir_check);
		else
			XDeleteProperty(dpy, scr->root, _XA_XDE_WM_REDIR_SUPPORT);
		xde_set_text_list(scr->root, _XA_XDE_WM_CLASS, XUTF8StringStyle, (char **) &wm->ch, 2);
		xde_set_text_list(scr->root, _XA_XDE_WM_COMMAND, XUTF8StringStyle, wm->cargv, wm->cargc);
		xde_set_text_list(scr->root, _XA_XDE_WM_CMDLINE, XUTF8StringStyle, wm->argv, wm->argc);
		xde_set_text(scr->root, _XA_XDE_WM_RCFILE, XUTF8StringStyle, wm->rcfile);
		xde_set_text(scr->root, _XA_XDE_WM_PRVDIR, XUTF8StringStyle, wm->pdir);
		xde_set_text(scr->root, _XA_XDE_WM_USRDIR, XUTF8StringStyle, wm->udir);
		xde_set_text(scr->root, _XA_XDE_WM_SYSDIR, XUTF8StringStyle, wm->sdir);
		xde_set_text(scr->root, _XA_XDE_WM_ETCDIR, XUTF8StringStyle, wm->edir);
		xde_set_text(scr->root, _XA_XDE_WM_STYLEFILE, XUTF8StringStyle, wm->stylefile);
		xde_set_text(scr->root, _XA_XDE_WM_STYLE, XUTF8StringStyle, wm->style);
		xde_set_text(scr->root, _XA_XDE_WM_STYLENAME, XUTF8StringStyle, wm->stylename);
		xde_set_text(scr->root, _XA_XDE_WM_MENU, XUTF8StringStyle, wm->menu);
		xde_set_text(scr->root, _XA_XDE_WM_ICON, XUTF8StringStyle, wm->icon);
	} else {
		XDeleteProperty(dpy, scr->root, XA_WM_NAME);
		XDeleteProperty(dpy, scr->root, XA_WM_CLIENT_MACHINE);
		XDeleteProperty(dpy, scr->root, XA_WM_CLASS);
		XDeleteProperty(dpy, scr->root, XA_WM_COMMAND);

		XDeleteProperty(dpy, scr->root, _XA_NET_WM_NAME);
		XDeleteProperty(dpy, scr->root, _XA_NET_WM_PID);

		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_NAME);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_NETWM_SUPPORT);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_WINWM_SUPPORT);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MAKER_SUPPORT);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MOTIF_SUPPORT);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_ICCCM_SUPPORT);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_REDIR_SUPPORT);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_PID);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_HOST);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_CLASS);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_CMDLINE);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_COMMAND);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_RCFILE);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_PRVDIR);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_USRDIR);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_SYSDIR);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_ETCDIR);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_STYLEFILE);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_STYLE);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_STYLENAME);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_MENU);
		XDeleteProperty(dpy, scr->root, _XA_XDE_WM_ICON);
	}
	xde_set_text(scr->root, _XA_XDE_WM_THEME, XUTF8StringStyle, scr->theme);
	xde_set_text(scr->root, _XA_XDE_WM_THEMEFILE, XUTF8StringStyle, scr->themefile);
}

__asm__(".symver __xde_set_properties,xde_set_properties@@XDE_1.0");

static void
xde_identify_wm_props()
{
	xde_set_properties();
}

void
__xde_identify_wm()
{
	switch (options.format) {
	case XDE_OUTPUT_HUMAN:
		xde_identify_wm_human();
		break;
	case XDE_OUTPUT_SHELL:
		xde_identify_wm_shell();
		break;
	case XDE_OUTPUT_PERL:
		xde_identify_wm_perl();
		break;
	case XDE_OUTPUT_PROPS:
		xde_identify_wm_props();
		break;
	}
}

__asm__(".symver __xde_identify_wm,xde_identify_wm@@XDE_1.0");

static void
show_wm()
{
	if (wm->netwm_check)
		OPRINTF("%d %s: NetWM 0x%lx\n", screen, wm->name, wm->netwm_check);
	if (wm->winwm_check)
		OPRINTF("%d %s: WinWM 0x%lx\n", screen, wm->name, wm->winwm_check);
	if (wm->maker_check)
		OPRINTF("%d %s: Maker 0x%lx\n", screen, wm->name, wm->maker_check);
	if (wm->motif_check)
		OPRINTF("%d %s: Motif 0x%lx\n", screen, wm->name, wm->motif_check);
	if (wm->icccm_check)
		OPRINTF("%d %s: ICCCM 0x%lx\n", screen, wm->name, wm->icccm_check);
	if (wm->redir_check)
		OPRINTF("%d %s: Redir 0x%lx\n", screen, wm->name, wm->redir_check);
	if (wm->pid)
		OPRINTF("%d %s: pid %ld\n", screen, wm->name, wm->pid);
	if (wm->host)
		OPRINTF("%d %s: host %s\n", screen, wm->name, wm->host);
	if (wm->argv && wm->argc) {
		int i, len;
		char *cmd;

		for (len = 0, i = 0; i < wm->argc; i++)
			len += strlen(wm->argv[i]) + 5;
		cmd = calloc(len, sizeof(*cmd));
		for (i = 0; i < wm->argc; i++) {
			strcat(cmd, i ? ", '" : "'");
			strcat(cmd, wm->argv[i]);
			strcat(cmd, "'");
		}
		OPRINTF("%d %s: cmdline %s\n", screen, wm->name, cmd);
	}
	if (wm->cargv && wm->cargc) {
		int i, len;
		char *cmd;

		for (len = 0, i = 0; i < wm->cargc; i++)
			len += strlen(wm->cargv[i]) + 5;
		cmd = calloc(len, sizeof(*cmd));
		for (i = 0; i < wm->cargc; i++) {
			strcat(cmd, i ? ", '" : "'");
			strcat(cmd, wm->cargv[i]);
			strcat(cmd, "'");
		}
		OPRINTF("%d %s: command %s\n", screen, wm->name, cmd);
	}
	if (wm->ch.res_name || wm->ch.res_class)
		OPRINTF("%d %s: '%s', '%s'\n", screen, wm->name,
			wm->ch.res_name ? : "(none)", wm->ch.res_class ? : "(none)");
	if (wm->rcfile)
		OPRINTF("%d %s: rcfile %s\n", screen, wm->name, wm->rcfile);
	if (wm->pdir)
		OPRINTF("%d %s: pdir %s\n", screen, wm->name, wm->pdir);
	if (wm->udir)
		OPRINTF("%d %s: udir %s\n", screen, wm->name, wm->udir);
	if (wm->sdir)
		OPRINTF("%d %s: sdir %s\n", screen, wm->name, wm->sdir);
	if (wm->edir)
		OPRINTF("%d %s: edir %s\n", screen, wm->name, wm->edir);
	if (wm->stylefile)
		OPRINTF("%d %s: stylefile %s\n", screen, wm->name, wm->stylefile);
	if (wm->style)
		OPRINTF("%d %s: style %s\n", screen, wm->name, wm->style);
	if (wm->stylename)
		OPRINTF("%d %s: stylename %s\n", screen, wm->name, wm->stylename);
	if (wm->menu)
		OPRINTF("%d %s: menu %s\n", screen, wm->name, wm->menu);
	if (wm->icon)
		OPRINTF("%d %s: icon %s\n", screen, wm->name, wm->icon);
	if (scr->theme)
		OPRINTF("%d %s: theme %s\n", screen, wm->name, scr->theme);
	if (scr->themefile)
		OPRINTF("%d %s: themefile %s\n", screen, wm->name, scr->themefile);
}

void
__xde_show_wms()
{
	int s;

	for (s = 0; s < nscr; s++) {
		screen = s;
		scr = screens + screen;
		root = scr->root;
		wm = scr->wm;
		if (wm)
			show_wm();
	}
}

__asm__(".symver __xde_show_wms,xde_show_wms@@XDE_1.0");

Bool
__xde_detect_wm()
{
	Bool have_wm = False;

	if (options.wmname) {
		screen = DefaultScreen(dpy);
		if (0 <= options.screen && options.screen < nscr)
			screen = options.screen;
		scr = screens + screen;
		root = scr->root;
		wm = scr->wm;
		unref_wm();
		ref_wm();
		wm->name = strdup(options.wmname);
		wm->ops = get_wm_ops();
		have_wm = True;
		if (options.output > 1)
			show_wm();
	} else {
		int s;

		for (s = 0; s < nscr; s++) {
			xde_set_screen(s);
			if (xde_check_wm()) {
				OPRINTF("found wm %s(%ld) for screen %d\n",
					wm->name, wm->pid, screen);
				have_wm = True;
				if (options.output > 1)
					show_wm();
			}
		}
		if (!have_wm)
			EPRINTF("%s\n", "could not find any window managers");
	}
	return have_wm;
}

__asm__(".symver __xde_detect_wm,xde_detect_wm@@XDE_1.0");

void
__xde_action_check_wm(XPointer dummy)
{
	xde_recheck_wm();
}

__asm__(".symver __xde_action_check_wm,xde_action_check_wm@@XDE_1.0");

/** @brief check whether a window manager has changed (or appeared)
  * @return Bool - true if changed
  */
void
__xde_recheck_wm()
{
	WindowManager *oldwm = wm;

	wm = scr->wm = NULL;
	xde_check_wm();
	xde_get_style();
	xde_get_menu();
	xde_get_icon();
	if (wm && oldwm) {
		do {
			if (wm->netwm_check != oldwm->netwm_check)
				break;
			if (wm->winwm_check != oldwm->winwm_check)
				break;
			if (wm->maker_check != oldwm->maker_check)
				break;
			if (wm->motif_check != oldwm->motif_check)
				break;
			if (wm->icccm_check != oldwm->icccm_check)
				break;
			if (wm->proxy != oldwm->proxy)
				break;
			if (wm->pid != oldwm->pid)
				break;
			if (!string_compare(wm->name, oldwm->name))
				break;
			if (!string_compare(wm->host, oldwm->host))
				break;
			goto no_wm_change;
		} while (0);
	} else if (!wm && oldwm) {
		DPRINTF("window manager disappeared\n");
	} else if (!oldwm && wm) {
		DPRINTF("window manager appeared\n");
	} else if (!oldwm && !wm) {
		DPRINTF("no window manager yet, test again in 2 seconds\n");
		xde_defer_wm_check(2000);
		goto no_wm;
	}
	if (callbacks && callbacks->wm_changed)
		(callbacks->wm_changed) (wm);
      no_wm_change:
	if (callbacks) {
		if (wm && oldwm) {
			if (callbacks->wm_style_changed &&
			    (!string_compare(wm->stylename, oldwm->stylename) ||
			     !string_compare(wm->style, oldwm->style) ||
			     !string_compare(wm->stylefile, oldwm->stylefile)))
				callbacks->wm_style_changed(wm->stylename,
							    wm->style, wm->stylefile);
			if (callbacks->wm_menu_changed &&
			    !string_compare(wm->menu, oldwm->menu))
				callbacks->wm_menu_changed(wm->menu);
			if (callbacks->wm_icon_changed &&
			    !string_compare(wm->icon, oldwm->icon))
				callbacks->wm_icon_changed(wm->icon);
		} else if (!wm && oldwm) {
			if (callbacks->wm_style_changed &&
			    (oldwm->stylename || oldwm->style || oldwm->stylefile))
				callbacks->wm_style_changed(NULL, NULL, NULL);
			if (callbacks->wm_menu_changed && (oldwm->menu))
				callbacks->wm_menu_changed(NULL);
			if (callbacks->wm_icon_changed && (oldwm->icon))
				callbacks->wm_icon_changed(NULL);
		} else if (!oldwm && wm) {
			if (callbacks->wm_style_changed &&
			    (wm->stylename || wm->style || wm->stylefile))
				callbacks->wm_style_changed(wm->stylename,
							    wm->style, wm->stylefile);
			if (callbacks->wm_menu_changed && (wm->menu))
				callbacks->wm_menu_changed(wm->menu);
			if (callbacks->wm_icon_changed && (wm->icon))
				callbacks->wm_icon_changed(wm->icon);
		}
	}
      no_wm:
	xde_wm_unref(oldwm);
	xde_check_theme();
}

__asm__(".symver __xde_recheck_wm,xde_recheck_wm@@XDE_1.0");

/** @} */


/** @name themes, styles and menus
  *
  * @{ */

static char *
get_optarg(char *optname)
{
	int i, l;
	char **argv;
	int argc;

	if (!optname)
		return NULL;

	l = strlen(optname);
	argv = wm->argv ? : wm->cargv ? : NULL;
	argc = wm->argv ? wm->argc : wm->cargv ? wm->cargc : 0;

	for (i = 1; i < argc; i++)
		if (strncmp(argv[i], optname, l) == 0)
			if ((argv[i][l] == '\0' && i < argc - 1) || argv[i][l] == '=')
				break;
	if (i < argc) {
		if (argv[i][l] == '=')
			return argv[i] + l + 1;
		return argv[i + 1];
	}
	return NULL;
}

char *
__xde_get_rcfile_optarg(char *optname)
{
	if (options.wmname)
		return options.rcfile;
	return get_optarg(optname);
}

__asm__(".symver __xde_get_rcfile_optarg,xde_get_rcfile_optarg@@XDE_1.0");

void
__xde_get_simple_dirs(char *wmname)
{
	char *home = xde_get_proc_environ("HOME") ? : ".";

	DPRINTF("pdir = '%s'\n", wm->pdir);
	free(wm->pdir);
	wm->pdir = strdup(wm->rcfile);
	DPRINTF("pdir = '%s'\n", wm->pdir);
	if (strrchr(wm->pdir, '/'))
		*strrchr(wm->pdir, '/') = '\0';
	free(wm->udir);
	wm->udir = calloc(strlen(home) + strlen(wmname) + 3, sizeof(*wm->udir));
	strcpy(wm->udir, home);
	strcat(wm->udir, "/.");
	strcat(wm->udir, wmname);
	free(wm->sdir);
	wm->sdir = calloc(strlen("/usr/share/") + strlen(wmname) + 1, sizeof(*wm->sdir));
	strcpy(wm->sdir, "/usr/share/");
	strcat(wm->sdir, wmname);
	free(wm->edir);
	wm->edir = calloc(strlen("/etc/") + strlen(wmname) + 1, sizeof(*wm->edir));
	strcpy(wm->edir, "/etc/");
	strcat(wm->edir, wmname);
	if (!strcmp(wm->pdir, home)) {
		free(wm->pdir);
		wm->pdir = strdup(wm->udir);
		DPRINTF("pdir = '%s'\n", wm->pdir);
	}
}

__asm__(".symver __xde_get_simple_dirs,xde_get_simple_dirs@@XDE_1.0");

void
__xde_get_rcfile_simple(char *wmname, char *rcname, char *option)
{
	char *home = xde_get_proc_environ("HOME") ? : ".";
	char *file = xde_get_rcfile_optarg(option);
	int len;
	struct stat st;

	free(wm->rcfile);
	if (file) {
		if (file[0] == '/')
			wm->rcfile = strdup(file);
		else {
			len = strlen(home) + strlen(file) + 2;
			wm->rcfile = calloc(len, sizeof(*wm->rcfile));
			strcpy(wm->rcfile, home);
			strcat(wm->rcfile, "/");
			strcat(wm->rcfile, file);
		}
	} else {
		len = strlen(home) + strlen(rcname) + 2;
		wm->rcfile = calloc(len, sizeof(*wm->rcfile));
		strcpy(wm->rcfile, home);
		strcat(wm->rcfile, "/");
		strcat(wm->rcfile, rcname);
		/* often this is symlinked into the actual directory */
		if (!lstat(wm->rcfile, &st) && S_ISLNK(st.st_mode)) {
			file = calloc(PATH_MAX + 1, sizeof(*file));
			if (readlink(wm->rcfile, file, PATH_MAX) == -1)
				EPRINTF("%s: %s\n", wm->rcfile, strerror(errno));
			if (file[0] == '/') {
				free(wm->rcfile);
				wm->rcfile = strdup(file);
			} else if (file[0]) {
				free(wm->rcfile);
				len = strlen(home) + strlen(file) + 2;
				wm->rcfile = calloc(len, sizeof(*wm->rcfile));
				strcpy(wm->rcfile, home);
				strcat(wm->rcfile, "/");
				strcat(wm->rcfile, file);
			}
		}
	}
	xde_get_simple_dirs(wmname);
}

__asm__(".symver __xde_get_rcfile_simple,xde_get_rcfile_simple@@XDE_1.0");

struct sortentry {
	char *stylename;
	char *filename;
};

static int
entry_compare(const void *a, const void *b)
{
	struct sortentry *A = (typeof(A)) a;
	struct sortentry *B = (typeof(B)) b;
	return strcmp(A->stylename, B->stylename);
}

void
__xde_list_dir_simple(char *xdir, char *dname, char *fname, char *suffix, char *style,
		      enum ListType type)
{
	DIR *dir;
	char *dirname, *file, *stylename, *p;
	struct dirent *d;
	struct stat st;
	struct sortentry *entries = NULL;
	int len, numb = 0;

	if (!xdir || !*xdir)
		return;
	len = strlen(xdir) + strlen(dname) + 2;
	dirname = calloc(len, sizeof(*dirname));
	strcpy(dirname, xdir);
	strcat(dirname, "/");
	strcat(dirname, dname);
	if (!(dir = opendir(dirname))) {
		DPRINTF("%s: %s\n", dirname, strerror(errno));
		free(dirname);
		return;
	}
	while ((d = readdir(dir))) {
		if (d->d_name[0] == '.')
			continue;
		len = strlen(dirname) + strlen(d->d_name) + strlen(fname) + 2;
		file = calloc(len, sizeof(*file));
		strcpy(file, dirname);
		strcat(file, "/");
		strcat(file, d->d_name);
		if (stat(file, &st)) {
			EPRINTF("%s: %s\n", file, strerror(errno));
			free(file);
			continue;
		}
		if (S_ISREG(st.st_mode)) {
			/* filename must end in suffix when specified */
			if (suffix[0]
			    && (!(p = strstr(d->d_name, suffix)) || p[strlen(suffix)])) {
				DPRINTF("%s has no %s suffix\n", d->d_name, suffix);
				free(file);
				continue;
			}
		} else if (!S_ISDIR(st.st_mode)) {
			DPRINTF("%s: not file or directory\n", file);
			free(file);
			continue;
		} else {
			strcat(file, fname);
			if (stat(file, &st)) {
				DPRINTF("%s: %s\n", file, strerror(errno));
				free(file);
				continue;
			}
			if (!S_ISREG(st.st_mode)) {
				DPRINTF("%s: not a file\n", file);
				free(file);
				continue;
			}
		}
		stylename = strdup(d->d_name);
		if (suffix[0] && (p = strstr(d->d_name, suffix))
		    && !p[strlen(suffix)])
			*p = '\0';
		if (!options.theme || xde_find_theme(stylename, NULL)) {
			entries = realloc(entries, (numb + 1) * sizeof(*entries));
			entries[numb].stylename = stylename;
			entries[numb].filename = file;
			numb++;
		} else {
			free(stylename);
			free(file);
		}
	}
	closedir(dir);
	free(dirname);
	if (numb && entries) {
		int i;
		struct sortentry *entry;

		qsort(entries, numb, sizeof(*entries), &entry_compare);
		for (i = 0, entry = entries; i < numb; i++, entry++) {
			switch (options.format) {
			case XDE_OUTPUT_PROPS:
			case XDE_OUTPUT_HUMAN:
				fprintf(stdout, "%s %s%s\n",
					entry->stylename, entry->filename,
					(style
					 && !strcmp(style, entry->filename)) ? " *" : "");
				break;
			case XDE_OUTPUT_SHELL:
				fprintf(stdout, "\t\'%s\t%s\t%s\'\n",
					entry->stylename, entry->filename,
					(style
					 && !strcmp(style, entry->filename)) ? "*" : "");
				break;
			case XDE_OUTPUT_PERL:
				fprintf(stdout, "\t\t'%s' => [ '%s', %d ],\n",
					entry->stylename, entry->filename,
					(style
					 && !strcmp(style, entry->filename)) ? 1 : 0);
				break;
			}
			free(entry->stylename);
			free(entry->filename);
		}
		free(entries);
	}
}

__asm__(".symver __xde_list_dir_simple,xde_list_dir_simple@@XDE_1.0");

void
__xde_list_styles_simple()
{
	char *style = wm->ops->get_style();

	switch (options.format) {
	case XDE_OUTPUT_HUMAN:
	case XDE_OUTPUT_PROPS:
	case XDE_OUTPUT_SHELL:
		break;
	case XDE_OUTPUT_PERL:
		fprintf(stdout, "{\n");
		break;
	}
	if (options.user) {
		switch (options.format) {
		case XDE_OUTPUT_HUMAN:
		case XDE_OUTPUT_PROPS:
			break;
		case XDE_OUTPUT_SHELL:
			fprintf(stdout, "XDE_WM_STYLES_USR=(\n");
			break;
		case XDE_OUTPUT_PERL:
			fprintf(stdout, "\tuser => {\n");
			break;
		}
		if (wm->pdir)
			wm->ops->list_dir(wm->pdir, style, XDE_LIST_PRIVATE);
		if (wm->udir && (!wm->pdir || strcmp(wm->pdir, wm->udir)))
			wm->ops->list_dir(wm->udir, style, XDE_LIST_USER);
		switch (options.format) {
		case XDE_OUTPUT_HUMAN:
		case XDE_OUTPUT_PROPS:
			break;
		case XDE_OUTPUT_SHELL:
			fprintf(stdout, ")\n");
			break;
		case XDE_OUTPUT_PERL:
			fprintf(stdout, "\t},\n");
			break;
		}
	}
	if (options.system) {
		switch (options.format) {
		case XDE_OUTPUT_HUMAN:
		case XDE_OUTPUT_PROPS:
			break;
		case XDE_OUTPUT_SHELL:
			fprintf(stdout, "XDE_WM_STYLES_SYS=(\n");
			break;
		case XDE_OUTPUT_PERL:
			fprintf(stdout, "\tsystem => {\n");
			break;
		}
		if (wm->sdir)
			wm->ops->list_dir(wm->sdir, style, XDE_LIST_SYSTEM);
		if (wm->edir)
			wm->ops->list_dir(wm->edir, style, XDE_LIST_GLOBAL);
		switch (options.format) {
		case XDE_OUTPUT_HUMAN:
		case XDE_OUTPUT_PROPS:
			break;
		case XDE_OUTPUT_SHELL:
			fprintf(stdout, ")\n");
			break;
		case XDE_OUTPUT_PERL:
			fprintf(stdout, "\t},\n");
			break;
		}
	}
	switch (options.format) {
	case XDE_OUTPUT_HUMAN:
	case XDE_OUTPUT_PROPS:
	case XDE_OUTPUT_SHELL:
		break;
	case XDE_OUTPUT_PERL:
		fprintf(stdout, "}\n");
		break;
	}
}

__asm__(".symver __xde_list_styles_simple,xde_list_styles_simple@@XDE_1.0");

void
__xde_gen_dir_simple(char *xdir, char *dname, char *fname, char *suffix, char *style,
		     enum ListType type)
{
	DIR *dir;
	char *dirname, *file, *stylename, *p;
	struct dirent *d;
	struct stat st;
	struct sortentry *entries = NULL;
	int len, numb = 0;

	if (!xdir || !*xdir)
		return;
	len = strlen(xdir) + strlen(dname) + 2;
	dirname = calloc(len, sizeof(*dirname));
	strcpy(dirname, xdir);
	strcat(dirname, "/");
	strcat(dirname, dname);
	if (!(dir = opendir(dirname))) {
		DPRINTF("%s: %s\n", dirname, strerror(errno));
		free(dirname);
		return;
	}
	while ((d = readdir(dir))) {
		if (d->d_name[0] == '.')
			continue;
		len = strlen(dirname) + strlen(d->d_name) + strlen(fname) + 2;
		file = calloc(len, sizeof(*file));
		strcpy(file, dirname);
		strcat(file, "/");
		strcat(file, d->d_name);
		if (stat(file, &st)) {
			EPRINTF("%s: %s\n", file, strerror(errno));
			free(file);
			continue;
		}
		if (S_ISREG(st.st_mode)) {
			/* filename must end in suffix when specified */
			if (suffix[0]
			    && (!(p = strstr(d->d_name, suffix)) || p[strlen(suffix)])) {
				DPRINTF("%s has no %s suffix\n", d->d_name, suffix);
				free(file);
				continue;
			}
		} else if (!S_ISDIR(st.st_mode)) {
			DPRINTF("%s: not file or directory\n", file);
			free(file);
			continue;
		} else {
			strcat(file, fname);
			if (stat(file, &st)) {
				DPRINTF("%s: %s\n", file, strerror(errno));
				free(file);
				continue;
			}
			if (!S_ISREG(st.st_mode)) {
				DPRINTF("%s: not a file\n", file);
				free(file);
				continue;
			}
		}
		stylename = strdup(d->d_name);
		if (suffix[0] && (p = strstr(d->d_name, suffix))
		    && !p[strlen(suffix)])
			*p = '\0';
		if (!options.theme || xde_find_theme(stylename, NULL)) {
			entries = realloc(entries, (numb + 1) * sizeof(*entries));
			entries[numb].stylename = stylename;
			entries[numb].filename = file;
			numb++;
		} else {
			free(stylename);
			free(file);
		}
	}
	closedir(dir);
	free(dirname);
	if (numb && entries) {
		int i;
		struct sortentry *entry;

		qsort(entries, numb, sizeof(*entries), &entry_compare);
		for (i = 0, entry = entries; i < numb; i++, entry++) {
			wm->ops->gen_item(style, type, entry->stylename, entry->filename);
			free(entry->stylename);
			free(entry->filename);
		}
		free(entries);
	}
}

__asm__(".symver __xde_gen_dir_simple,xde_gen_dir_simple@@XDE_1.0");

void
__xde_gen_menu_simple()
{
	char *style = wm->ops->get_style();

	if (options.user) {
		if (wm->pdir)
			wm->ops->gen_dir(wm->pdir, style, XDE_LIST_PRIVATE);
		if (wm->udir && (!wm->pdir || strcmp(wm->pdir, wm->udir)))
			wm->ops->gen_dir(wm->udir, style, XDE_LIST_USER);
	}
	if (options.system) {
		if (wm->sdir)
			wm->ops->gen_dir(wm->sdir, style, XDE_LIST_SYSTEM);
		if (wm->edir)
			wm->ops->gen_dir(wm->edir, style, XDE_LIST_GLOBAL);
	}
}

__asm__(".symver __xde_gen_menu_simple,xde_gen_menu_simple@@XDE_1.0");

void
__xde_list_styles_nostyle()
{
	char **xdir;
	static char *suffix = "/xde/theme.ini";
	static char *subdir = "/themes";
	char **seen, **saw;
	int numb;

	if (!wm->xdg_dirs)
		xde_get_xdg_dirs();
	if (!wm->xdg_dirs)
		return;

	numb = 0;
	seen = calloc(numb + 1, sizeof(*seen));
	seen[numb] = NULL;

	for (xdir = wm->xdg_dirs; *xdir; xdir++) {
		DIR *dir;
		char *dirname;
		struct dirent *d;
		int dlen;

		dlen = strlen(*xdir) + strlen(subdir) + 1;
		dirname = calloc(dlen, sizeof(*dirname));
		strcpy(dirname, *xdir);
		strcat(dirname, subdir);

		if (!(dir = opendir(dirname))) {
			DPRINTF("%s: %s\n", dirname, strerror(errno));
			free(dirname);
			continue;
		}
		while ((d = readdir(dir))) {
			int len;
			char *file, *stylename;
			struct stat st;

			if (d->d_name[0] == '.')
				continue;
			len = strlen(dirname) + strlen(d->d_name) + strlen(suffix) + 2;
			file = calloc(len, sizeof(*file));
			strcpy(file, dirname);
			strcat(file, "/");
			strcat(file, d->d_name);
			strcat(file, suffix);
			if (stat(file, &st)) {
				EPRINTF("%s: %s\n", file, strerror(errno));
				free(file);
				continue;
			}
			if (!S_ISREG(st.st_mode)) {
				DPRINTF("%s: not a file\n", file);
				free(file);
				continue;
			}
			for (saw = seen; *saw; saw++)
				if (!strcmp(*saw, d->d_name))
					break;
			if (*saw)
				continue;
			stylename = strdup(d->d_name);
			numb++;
			seen = realloc(seen, (numb + 1) * sizeof(*seen));
			seen[numb - 1] = stylename;
			seen[numb] = NULL;
			switch (options.format) {
			case XDE_OUTPUT_HUMAN:
			case XDE_OUTPUT_PROPS:
				fprintf(stdout, "%s %s%s\n",
					stylename, file,
					(scr->themefile
					 && !strcmp(scr->themefile, file)) ? " *" : "");
				break;
			case XDE_OUTPUT_SHELL:
				fprintf(stdout, "\t\'%s\t%s\t%s\'\n",
					stylename, file,
					(scr->themefile
					 && !strcmp(scr->themefile, file)) ? " *" : "");
				break;
			case XDE_OUTPUT_PERL:
				fprintf(stdout, "\t\t'%s' => [ '%s', %d ],\n",
					stylename, file,
					(scr->themefile
					 && !strcmp(scr->themefile, file)) ? 1 : 0);
				break;
			}
			free(file);
		}
		free(dirname);
		closedir(dir);
	}
	for (saw = seen; *saw; saw++)
		free(*saw);
	free(seen);
}

__asm__(".symver __xde_list_styles_nostyle,xde_list_styles_nostyle@@XDE_1.0");

char *
__xde_find_style_simple(char *dname, char *fname, char *suffix)
{
	char *path = NULL;
	int len, i;
	struct stat st;

	wm->ops->get_rcfile();
	if (options.style[0] == '/') {
		len = strlen(options.style) + strlen(fname) + 1;
		path = calloc(len, sizeof(*path));
		strcpy(path, options.style);
		if (stat(path, &st)) {
			EPRINTF("%s: %s\n", path, strerror(errno));
			free(path);
			return NULL;
		}
		if (S_ISDIR(st.st_mode)) {
			strcat(path, fname);
			if (stat(path, &st)) {
				EPRINTF("%s: %s\n", path, strerror(errno));
				free(path);
				return NULL;
			}
		} else if (!S_ISREG(st.st_mode)) {
			EPRINTF("%s: not directory or file\n", path);
			free(path);
			return NULL;
		}
	} else {
		int beg, end;

		if (options.user && !options.system) {
			beg = 0;
			end = 2;
		}
		else if (options.system && !options.user) {
			beg = 2;
			end = CHECK_DIRS;
		}
		else {
			beg = 0;
			end = CHECK_DIRS;
		}
		for (i = beg; i < end; i++) {
			if (!wm->dirs[i] || !wm->dirs[i][0])
				continue;
			len = strlen(wm->dirs[i]) + strlen(dname) +
			    strlen(options.style) + strlen(fname) +
			    strlen(suffix) + 4;
			path = calloc(len, sizeof(*path));
			strcpy(path, wm->dirs[i]);
			strcat(path, "/");
			strcat(path, dname);
			strcat(path, "/");
			strcat(path, options.style);
			if (stat(path, &st)) {
				if (suffix[0]) {
					strcat(path, suffix);
					if (stat(path, &st)) {
						DPRINTF("%s: %s\n", path, strerror(errno));
						free(path);
						path = NULL;
						continue;
					}
					if (!S_ISREG(st.st_mode)) {
						DPRINTF("%s: not a file\n", path);
						free(path);
						path = NULL;
						continue;
					}
				} else {
					DPRINTF("%s: %s\n", path, strerror(errno));
					free(path);
					path = NULL;
					continue;
				}
			}
			if (S_ISDIR(st.st_mode)) {
				strcat(path, fname);
				if (stat(path, &st)) {
					DPRINTF("%s: %s\n", path, strerror(errno));
					free(path);
					path = NULL;
					continue;
				}
			} else if (!S_ISREG(st.st_mode)) {
				DPRINTF("%s: not directory or file\n", path);
				free(path);
				path = NULL;
				continue;
			}
			break;
		}
	}
	return path;
}

__asm__(".symver __xde_find_style_simple,xde_find_style_simple@@XDE_1.0");

char *
__xde_get_menu_simple(char *fname, char *(*from_file) (char *))
{
	char *menurc = NULL, *menufile = NULL;
	int i, len;

	wm->ops->get_rcfile();

	for (i = 0; i < CHECK_DIRS; i++) {
		if (i == 1)
			continue;
		if (!wm->dirs[i] || !wm->dirs[i][0])
			continue;
		len = strlen(wm->dirs[i]) + strlen(fname) + 2;
		menurc = calloc(len, sizeof(*menurc));
		strcpy(menurc, wm->dirs[i]);
		strcat(menurc, "/");
		strcat(menurc, fname);
		if (!from_file) {
			menufile = menurc;
			menurc = NULL;
			break;
		}
		if (!xde_test_file(menurc)) {
			free(menurc);
			continue;
		}
		if (!(menufile = from_file(menurc))) {
			/* from_file should print its own errors */
			free(menurc);
			continue;
		}
		if (menufile[0] != '/') {
			/* WARNING: from_file must return a buffer large enough to add path prefix: 
			   recommend PATH_MAX + 1 */
			/* make absolute path */
			memmove(menufile + strlen(wm->dirs[i]) + 1, menufile, strlen(menufile) + 1);
			memcpy(menufile, menurc, strlen(wm->dirs[i]) + 1);
		}
		free(menurc);
		menurc = NULL;
		break;
	}
	if (menufile) {
		free(wm->menu);
		wm->menu = menufile;
	}
	return wm->menu;
}

__asm__(".symver __xde_get_menu_simple,xde_get_menu_simple@@XDE_1.0");

static char *
theme_replace(char *theme, char *themefile)
{
	if (!string_compare(theme, scr->theme)) {
		theme = strdup(theme);
		free(scr->theme);
		scr->theme = theme;
		xde_set_text(scr->root, _XA_XDE_THEME_NAME, XUTF8StringStyle, theme);
	} else
		theme = scr->theme;
	if (!string_compare(themefile, scr->themefile)) {
		free(scr->themefile);
		scr->themefile = themefile;
	} else
		free(themefile);
	return theme;
}

/** @brief Get the current XDE theme.
  * @return char * - The name of the current theme or NULL.
  *
  * In the absense of a known style system, we can still discover which XDE
  * theme is set by examining the _XDE_THEME_NAME property on the root window
  * and, barring that, check the ~/.gtkrc-2.0.xde file for a theme name.
  */
char *
__xde_get_theme()
{
	char *theme = NULL, *themefile = NULL;
	char *name, *files, *end, *file;
	int n;

	if (wm) {
		char *style = wm->ops->get_style();

		if (style && xde_find_theme(style, &themefile))
			return theme_replace(style, themefile);
	}
	name = xde_get_text(scr->root, _XA_XDE_THEME_NAME);
	if (name && xde_find_theme(name, &themefile))
		return theme_replace(name, themefile);
	if (name)
		XFree(name);
	files = calloc(PATH_MAX, sizeof(*files));
	strcpy(files, getenv("GTK2_RC_FILES") ? : "");
	if (*files)
		strcat(files, ":");
	strcat(files, getenv("HOME"));
	strcat(files, "/.gtkrc-2.0.xde");

	end = files + strlen(files);
	for (n = 0, file = files; file < end; n++,
	     *strchrnul(file, ':') = '\0', file += strlen(file) + 1) ;

	for (n = 0, file = files; file < end; n++, file += strlen(file) + 1) {
		struct stat st;
		FILE *f;
		char *buf, *b, *e;

		if (stat(file, &st)) {
			DPRINTF("%s: %s\n", file, strerror(errno));
			continue;
		}
		if (!S_ISREG(st.st_mode)) {
			DPRINTF("%s: not a file\n", file);
			continue;
		}
		if (!(f = fopen(file, "r"))) {
			DPRINTF("%s: %s\n", file, strerror(errno));
			continue;
		}
		buf = calloc(PATH_MAX + 1, sizeof(*buf));
		while (fgets(buf, PATH_MAX, f)) {
			b = buf;
			b += strspn(b, " \t");
			if (*b == '#' || *b == '\n')
				continue;
			if (strncmp(b, "gtk-theme-name", 14))
				continue;
			b += 14;
			b += strspn(b, " \t");
			if (*b != '=')
				continue;
			b += 1;
			b += strspn(b, " \t");
			if (*b != '"')
				continue;
			b += 1;
			e = b;
			while ((e = strchr(e, '"'))) {
				if (*(e - 1) != '\\')
					break;
				memmove(e - 1, e, strlen(e) + 1);
			}
			if (!e || b >= e)
				continue;
			*e = '\0';
			memmove(buf, b, strlen(b) + 1);
			if (xde_find_theme(buf, &themefile))
				theme = theme_replace(buf, themefile);
		}
		fclose(f);
		free(buf);
	}
	free(files);
	return theme;
}

__asm__(".symver __xde_get_theme,xde_get_theme@@XDE_1.0");

char *
xde_get_style()
{
	if (!wm || !wm->ops || !wm->ops->get_style)
		return NULL;
	return wm->ops->get_style();
}

__asm__(".symver __xde_get_style,xde_get_style@@XDE_1.0");

char *
xde_get_menu()
{
	if (!wm || !wm->ops || !wm->ops->get_menu)
		return NULL;
	return wm->ops->get_menu();
}

__asm__(".symver __xde_get_menu,xde_get_menu@@XDE_1.0");

char *
xde_get_icon()
{
	if (!wm || !wm->ops || !wm->ops->get_icon)
		return NULL;
	return wm->ops->get_icon();
}

__asm__(".symver __xde_get_icon,xde_get_icon@@XDE_1.0");

void
__xde_check_style()
{
	Bool changed;

	if (!wm || !wm->ops || !wm->ops->get_style)
		return;
	{
		char *oldname = wm->stylename ? strdup(wm->stylename) : NULL;
		char *oldstyle = wm->style ? strdup(wm->style) : NULL;
		char *oldfile = wm->stylefile ? strdup(wm->stylefile) : NULL;

		wm->ops->get_style();

		changed = (!string_compare(oldname, wm->stylename) ||
			   !string_compare(oldstyle, wm->style) ||
			   !string_compare(oldfile, wm->stylefile));
		free(oldname);
		free(oldstyle);
		free(oldfile);
	}
	if (changed && callbacks && callbacks->wm_style_changed)
		callbacks->wm_style_changed(wm->stylename, wm->style, wm->stylefile);
}

__asm__(".symver __xde_check_style,xde_check_style@@XDE_1.0");

void
__xde_check_menu()
{
	Bool changed;

	if (!wm || !wm->ops || !wm->ops->get_menu)
		return;
	{
		char *oldmenu = wm->menu ? strdup(wm->menu) : NULL;

		wm->ops->get_menu();
		changed = (!string_compare(oldmenu, wm->menu));
		free(oldmenu);
	}
	if (changed && callbacks && callbacks->wm_menu_changed)
		callbacks->wm_menu_changed(wm->menu);
}

__asm__(".symver __xde_check_menu,xde_check_menu@@XDE_1.0");

void
__xde_check_icon()
{
	Bool changed;

	if (!wm || !wm->ops || !wm->ops->get_icon)
		return;
	{
		char *oldicon = wm->icon ? strdup(wm->icon) : NULL;

		wm->ops->get_icon();
		changed = (!string_compare(oldicon, wm->icon));
		free(oldicon);
	}
	if (changed && callbacks && callbacks->wm_icon_changed)
		callbacks->wm_icon_changed(wm->icon);
}

__asm__(".symver __xde_check_icon,xde_check_icon@@XDE_1.0");

void
__xde_check_theme()
{
	Bool changed;

	{
		char *oldtheme = scr->theme ? strdup(scr->theme) : NULL;
		char *oldfile = scr->themefile ? strdup(scr->themefile) : NULL;

		xde_check_style();
		xde_get_theme();

		changed = (!string_compare(oldtheme, scr->theme) ||
			   !string_compare(oldfile, scr->themefile));
		free(oldtheme);
		free(oldfile);
	}
	if (changed && callbacks && callbacks->wm_theme_changed)
		(callbacks->wm_theme_changed) (scr->theme, scr->themefile);
}

__asm__(".symver __xde_check_theme,xde_check_theme@@XDE_1.0");

void
__xde_action_check_theme(XPointer dummy)
{
	xde_check_theme();
}

__asm__(".symver __xde_action_check_theme,xde_action_check_theme@@XDE_1.0");

void
__xde_set_theme(char *name)
{
	const char *suffix = "/.gtkrc-2.0.xde";
	char *theme, *file, *home, *buf;
	int len, done = 0;
	FILE *f;
	XEvent xev;

	if (!xde_find_theme(name, NULL)) {
		EPRINTF("cannot find theme %s\n", name);
		return;
	}
	if ((theme = xde_get_theme()) && !strcmp(theme, name)) {
		DPRINTF("theme %s has not changed\n", name);
		return;
	}
	home = getenv("HOME") ? : "~";
	len = strlen(home) + 1 + strlen(suffix) + 1;
	file = calloc(len, sizeof(*file));
	strcpy(file, home);
	strcat(file, "/");
	strcat(file, suffix);
	buf = malloc(BUFSIZ);
	if ((f = fopen(file, "r"))) {
		char *pos;
		int bytes;

		for (bytes = 0, pos = buf + bytes; fgets(pos, BUFSIZ, f);
		     buf = realloc(buf, BUFSIZ + bytes), pos = buf + bytes) {
			char *b = pos;

			b += strspn(b, " \t");
			if (*b == '#' || *b == '\n') {
				bytes += strlen(pos);
				continue;
			}
			if (strncmp(b, "gtk-theme-name", 14)) {
				bytes += strlen(pos);
				continue;
			}
			snprintf(pos, BUFSIZ, "gtk-theme-name=\"%s\"\n", name);
			done = 1;
			bytes += strlen(pos);
		}
		if (!done) {
			snprintf(pos, BUFSIZ, "gtk-theme-name=\"%s\"\n", name);
			bytes += strlen(pos);
		}
		fclose(f);
	} else {
		DPRINTF("%s: %s\n", file, strerror(errno));
		snprintf(buf, BUFSIZ, "gtk-theme-name=\"%s\"\n", name);
	}
	if (!(f = fopen(file, "w"))) {
		EPRINTF("%s: %s\n", file, strerror(errno));
		free(buf);
		free(file);
		return;
	}
	fprintf(f, "%s", buf);
	fclose(f);
	free(buf);
	free(file);

	memset(&xev, 0, sizeof(xev));
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = False;
	xev.xclient.display = dpy;
	xev.xclient.window = scr->root;
	xev.xclient.message_type = _XA_GTK_READ_RCFILES;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 0;
	xev.xclient.data.l[1] = 0;
	xev.xclient.data.l[2] = 0;
	xev.xclient.data.l[3] = 0;
	xev.xclient.data.l[4] = 0;
	XSendEvent(dpy, scr->root, False, StructureNotifyMask |
		   SubstructureRedirectMask | SubstructureNotifyMask, &xev);

	xde_set_text(scr->root, _XA_XDE_THEME_NAME, XUTF8StringStyle, name);
	return;
}

__asm__(".symver __xde_set_theme,xde_set_theme@@XDE_1.0");

char *
__xde_get_style_simple(char *fname, char *(*from_file) (char *))
{
	char *stylerc = NULL, *stylefile = NULL;
	int i, len, beg, end;
	struct stat st;

	wm->ops->get_rcfile();
	if (options.user && !options.system) {
		beg = 0;
		end = 2;
	} else if (options.system && !options.user) {
		beg = 2;
		end = CHECK_DIRS;
	} else {
		beg = 0;
		end = CHECK_DIRS;
	}
	for (i = beg; i < end; i++) {
		if (i == 1)
			continue;
		if (!wm->dirs[i] || !wm->dirs[i][0])
			continue;
		len = strlen(wm->dirs[i]) + strlen(fname) + 2;
		stylerc = calloc(len, sizeof(*stylerc));
		strcpy(stylerc, wm->dirs[i]);
		strcat(stylerc, "/");
		strcat(stylerc, fname);
		if (lstat(stylerc, &st)) {
			DPRINTF("%s: %s\n", stylerc, strerror(errno));
			free(stylerc);
			continue;
		}
		if (S_ISREG(st.st_mode)) {
			if (!from_file) {
				DPRINTF("%s: not a link\n", stylerc);
				free(stylerc);
				continue;
			}
			if (!(stylefile = from_file(stylerc))) {
				/* from_file should print its own errors */
				free(stylerc);
				continue;
			}
		} else if (S_ISLNK(st.st_mode)) {
			char *buf = calloc(PATH_MAX + 1, sizeof(*buf));

			errno = 0;
			if (readlink(stylerc, buf, PATH_MAX) <= 0) {
				DPRINTF("%s: %s\n", stylerc, strerror(errno));
				free(buf);
				free(stylerc);
				continue;
			}
			stylefile = buf;
		} else {
			DPRINTF("%s: not link or file\n", stylerc);
			free(stylerc);
			continue;
		}
		if (stylefile[0] != '/') {
			/* WARNING: from_file must return a buffer large enough to add a
			   path prefix: recommend PATH_MAX +1 */
			/* make absolute path */
			memmove(stylefile + strlen(wm->dirs[i]) + 1,
				stylefile, strlen(stylefile) + 1);
			memcpy(stylefile, stylerc, strlen(wm->dirs[i]) + 1);
		}
		free(stylerc);
		stylerc = NULL;
		break;
	}
	if (stylefile) {
		char *pos;

		free(wm->style);
		wm->style = strdup(stylefile);
		free(wm->stylename);
		/* trim off /fname */
		if ((pos = strrchr(stylefile, '/'))
		    && !strcmp(pos + 1, fname))
			*pos = '\0';
		/* trim off path */
		wm->stylename = (pos = strrchr(stylefile, '/')) ?
		    strdup(pos + 1) : strdup(stylefile);
		free(stylefile);
	}
	return wm->style;
}

__asm__(".symver __xde_get_style_simple,xde_get_style_simple@@XDE_1.0");

/** @brief Get menu from resource database.
  *
  * This method is shared by blackbox(1), fluxbox(1) and waimea(1).
  */
char *
__xde_get_menu_database(char *name, char *clas)
{
	XrmValue value;
	char *type;

	wm->ops->get_rcfile();
	if (!xde_test_file(wm->rcfile)) {
		EPRINTF("rcfile %s does not exist\n", wm->rcfile);
		return NULL;
	}
	init_xrm();
	if (!wm->db && !(wm->db = XrmGetFileDatabase(wm->rcfile))) {
		EPRINTF("cannot read database file %s\n", wm->rcfile);
		return NULL;
	}
	if (!XrmGetResource(wm->db, name, clas, &type, &value)) {
		EPRINTF("no %s resource in database %s\n", name, wm->rcfile);
		return NULL;
	}
	free(wm->menu);
	/* watch out for tilde expansion */
	if (*(char *) value.addr == '~') {
		char *home = xde_get_proc_environ("HOME") ? : ".";

		wm->menu = calloc(strlen(home) + value.size, sizeof(*wm->menu));
		strcpy(wm->menu, home);
		strncat(wm->menu, (char *)value.addr + 1, value.size - 1);
	} else {
		wm->menu = strndup((char *) value.addr, value.size);
	}
	return wm->menu;
}

__asm__(".symver __xde_get_menu_database,xde_get_menu_database@@XDE_1.0");

/** @brief Get style from resource database.
  *
  * This method is shared by blackbox(1), fluxbox(1) and waimea(1).
  */
char *
__xde_get_style_database(char *name, char *clas)
{
	XrmValue value;
	char *type, *pos;

	wm->ops->get_rcfile();
	if (!xde_test_file(wm->rcfile)) {
		EPRINTF("rcfile %s does not exist\n", wm->rcfile);
		return NULL;
	}
	init_xrm();
	if (!wm->db && !(wm->db = XrmGetFileDatabase(wm->rcfile))) {
		EPRINTF("cannot read database file %s\n", wm->rcfile);
		return NULL;
	}
	if (!XrmGetResource(wm->db, name, clas, &type, &value)) {
		EPRINTF("no %s resource in database %s\n", name, wm->rcfile);
		return NULL;
	}
	free(wm->style);
	wm->style = strndup((char *) value.addr, value.size);
	free(wm->stylename);
	wm->stylename = (pos = strrchr(wm->style, '/')) ?
	    strdup(pos + 1) : strdup(wm->style);
	if ((pos = strstr(wm->stylename, ".style")) && pos[6] == '\0')
		*pos = '\0';
	return wm->style;
}

__asm__(".symver __xde_get_style_database,xde_get_style_database@@XDE_1.0");

void
__xde_set_style_simple(char *rcname, void (*to_file) (char *, char *))
{
	char *stylefile, *style, *stylerc;
	int len;
	struct stat st;
	Bool link = !to_file ? True : options.link;

	if (!(stylefile = wm->ops->find_style())) {
		EPRINTF("cannot find %s style '%s'\n", wm->name, options.style);
		return;
	}
	if ((style = wm->ops->get_style()) && !strcmp(style, stylefile)) {
		DPRINTF("%s style is already %s\n", wm->name, options.style);
		free(stylefile);
		return;
	}
	len = strlen(wm->pdir) + strlen(rcname) + 2;
	stylerc = calloc(len, sizeof(*stylerc));
	strcpy(stylerc, wm->pdir);
	strcat(stylerc, "/");
	strcat(stylerc, rcname);
	if (lstat(stylerc, &st))
		DPRINTF("%s: %s\n", stylerc, strerror(errno));
	else if (S_ISLNK(st.st_mode))
		link = True;
	if (options.dryrun) {
		if (link)
			OPRINTF("would link %s -> %s\n", stylerc, stylefile);
		else {
			OPRINTF("would write to %s the following:\n", stylerc);
			to_file("/dev/stderr", stylefile);
		}
		if (options.reload)
			OPRINTF("%s", "would reload window manager\n");
	} else {
		unlink(stylerc);
		if (link) {
			if (symlink(stylefile, stylerc)) {
				EPRINTF("%s -> %s: %s\n", stylerc, stylefile,
					strerror(errno));
				free(stylerc);
				return;
			}
		} else {
			to_file(stylerc, stylefile);
		}
		if (options.reload) {
			if (wm->ops->reload_style)
				wm->ops->reload_style();
			else
				EPRINTF("cannot reload %s\n", wm->name);
		}
	}
}

__asm__(".symver __xde_set_style_simple,xde_set_style_simple@@XDE_1.0");

/** @brief Set the session.styleFile resource in the rcfile.
  *
  * This method is shared by blackbox(1), fluxbox(1) and waimea(1).
  */
void
__xde_set_style_database(char *name)
{
	char *stylefile, *line, *style;
	int len;

	if (options.reload && !wm->pid) {
		EPRINTF("cannot reload %s without a pid\n", wm->name);
		return;
	}
	if (!(stylefile = wm->ops->find_style())) {
		EPRINTF("cannot find style %s\n", options.style);
		return;
	}
	if ((style = wm->ops->get_style()) && !strcmp(style, stylefile))
		goto no_change;
	init_xrm();
	if (!wm->db && !(wm->db = XrmGetFileDatabase(wm->rcfile))) {
		EPRINTF("cannot read database file %s\n", wm->rcfile);
		goto no_db;
	}
	len = strlen(stylefile) + strlen(name) + strlen(":\t\t") + 1;
	line = calloc(len, sizeof(*line));
	snprintf(line, len, "%s:\t\t%s", name, stylefile);
	XrmPutLineResource(&wm->db, line);
	free(line);
	if (options.dryrun) {
		OPRINTF("would write database to %s as follows:\n", wm->rcfile);
		XrmPutFileDatabase(wm->db, "/dev/stderr");
		if (options.reload)
			OPRINTF("would reload %s\n", wm->name);
	} else {
		XrmPutFileDatabase(wm->db, wm->rcfile);
		if (options.reload)
			wm->ops->reload_style();
	}
      no_change:
	if (wm->db) {
		XrmDestroyDatabase(wm->db);
		wm->db = NULL;
	}
      no_db:
	free(stylefile);
	return;
}

__asm__(".symver __xde_set_style_database,xde_set_style_database@@XDE_1.0");

/** @brief get an icon for the window manager
  *
  * Basically search the XDG directories for an xsession .desktop file with the
  * same name as the window manager.  If one is found, use the Icon= field;
  * otherwise, use the fallback if provided; otherwise, use the window manager
  * name if available; otherwise, return NULL.
  *
  */
char *
__xde_get_icon_simple(const char *fallback)
{
	char **xdir, *dirname, *file;
	char *icon = NULL;
	static char *subdir = "/xsessions";
	int nlen, len;
	DIR *dir;
	struct dirent *d;
	FILE *f;
	struct stat st;
	char *buf, *maybe, *b;
	Bool match;

	if (!wm->name) {
		DPRINTF("No window manager name, falling back\n");
		goto fallback;
	}
	if (!wm->xdg_dirs)
		xde_get_xdg_dirs();
	if (!wm->xdg_dirs) {
		DPRINTF("No XDG directories, falling back\n");
		goto fallback;
	}
	nlen = strlen(subdir);
	for (xdir = wm->xdg_dirs; *xdir; xdir++) {
		DPRINTF("Checking XDG directory '%s'\n", *xdir);
		len = strlen(*xdir) + nlen + 1;
		dirname = calloc(len, sizeof(*dirname));
		strcpy(dirname, *xdir);
		strcat(dirname, subdir);
		if (!(dir = opendir(dirname))) {
			DPRINTF("%s: %s\n", dirname, strerror(errno));
			free(dirname);
			continue;
		}
		while ((d = readdir(dir))) {
			if (d->d_name[0] == '.')
				continue;
			if (!strstr(d->d_name, ".desktop")) {
				DPRINTF("%s: does not end in .desktop\n", d->d_name);
				continue;
			}
			len = strlen(dirname) + strlen(d->d_name) + 2;
			file = calloc(len, sizeof(*file));
			strcpy(file, dirname);
			strcat(file, "/");
			strcat(file, d->d_name);
			if (stat(file, &st)) {
				DPRINTF("%s: %s\n", file, strerror(errno));
				free(file);
				continue;
			}
			if (!S_ISREG(st.st_mode)) {
				DPRINTF("%s: not file\n", file);
				free(file);
				continue;
			}
			if (!(f = fopen(file, "r"))) {
				DPRINTF("%s: %s\n", file, strerror(errno));
				free(file);
				continue;
			}
			DPRINTF("Checking file '%s'\n", file);
			buf = calloc(PATH_MAX + 1, sizeof(*buf));
			maybe = NULL;
			b = strrchr(file, '/') + 1;
			*strstr(b, ".desktop") = '\0';
			match = strcasecmp(b, wm->name) ? False : True;
			while (fgets(buf, PATH_MAX, f)) {
				b = buf;
				b += strspn(b, " \t");
				if (*b == '#' || *b == '\n')
					continue;
				*strchrnul(b, '\n') = '\0';
				if (!strncmp(b, "Icon=", 5)) {
					b += 5;
					DPRINTF("Possible icon: %s\n", b);
					free(maybe);
					maybe = strdup(b);
					if (match) {
						icon = maybe;
						break;
					}
					continue;
				}
				if (!strncmp(b, "X-GNOME-WMName=", 15)) {
					b += 15;
					if (!strcasecmp(b, wm->name)) {
						DPRINTF("Matched wmname: %s\n", b);
						match = True;
						if (maybe) {
							icon = maybe;
							break;
						}
					}
				}
				/* might want to keep looking for Hidden=true */
			}
			fclose(f);
			free(file);
			if (icon)
				break;
		}
		closedir(dir);
		free(dirname);
		if (icon)
			break;
	}
      fallback:
	if (!icon && fallback) {
		DPRINTF("Could not find icon: falling back to %s\n", fallback);
		icon = strdup(fallback);
	}
	if (icon) {
		free(wm->icon);
		wm->icon = icon;
	}
	return icon;
}

__asm__(".symver __xde_get_icon_simple,xde_get_icon_simple@@XDE_1.0");

Bool
__xde_test_file(char *path)
{
	struct stat st;

	if (!path) {
		EPRINTF("%s", "null path\n");
		return False;
	}

	OPRINTF("testing file '%s' for existence\n", path);

	if (!stat(path, &st)) {
		if (S_ISREG(st.st_mode)) {
			if (!access(path, R_OK))
				return True;
			else
				DPRINTF("cannot read %s\n", path);
		} else
			DPRINTF("%s not regular file\n", path);
	} else
		EPRINTF("%s: %s\n", path, strerror(errno));
	return False;
}

__asm__(".symver __xde_test_file,xde_test_file@@XDE_1.0");

/** @} */

/* @name XTWM
 *
 * There is really no window manager named xtwm, but twm, ctwm, vtwm and etwm
 * are all similar so this section contains factored routines.
 */
/** @{ */

void
__xde_get_rcfile_XTWM(char *xtwm)
{
	char *home = xde_get_proc_environ("HOME") ? : ".";
	char *file = xde_get_rcfile_optarg("-f");
	int len;

	free(wm->rcfile);
	if (file) {
		if (file[0] == '/')
			wm->rcfile = strdup(file);
		else {
			len = strlen(home) + strlen(file) + 2;
			wm->rcfile = calloc(len, sizeof(*wm->rcfile));
			strcpy(wm->rcfile, home);
			strcat(wm->rcfile, "/");
			strcat(wm->rcfile, file);
		}
	} else {
		char names[4][16], *rcfile = NULL;
		struct stat st;
		int i;

		/* check if ~/.%src.%d exists first where %d is the screen number */
		snprintf(names[0], sizeof(names[0]), "/.%src.%d", xtwm, screen);
		snprintf(names[1], sizeof(names[1]), "/.%src", xtwm);
		/* then check ~/.twmrc.%d and ~/.twmrc */
		snprintf(names[2], sizeof(names[2]), "/.%src.%d", "twm", screen);
		snprintf(names[3], sizeof(names[3]), "/.%src", "twm");

		for (i = 0; i < 4; i++) {
			len = strlen(home) + strlen(names[i]) + 1;
			rcfile = calloc(len, sizeof(*rcfile));
			strcpy(rcfile, home);
			strcat(rcfile, names[i]);
			errno = 0;
			if (!stat(rcfile, &st) && S_ISREG(st.st_mode))
				break;
			free(rcfile);
			rcfile = NULL;
			DPRINTF("%s: %s\n", rcfile, strerror(errno));
		}
		if (rcfile) {
			/* often this is symlinked into the actual directory */
			if (!lstat(rcfile, &st) && S_ISLNK(st.st_mode)) {
				file = calloc(PATH_MAX + 1, sizeof(*file));
				if (readlink(rcfile, file, PATH_MAX) == -1)
					EPRINTF("%s: %s\n", rcfile, strerror(errno));
				if (file[0] == '/') {
					free(rcfile);
					rcfile = strdup(file);
				} else if (file[0]) {
					free(rcfile);
					len = strlen(home) + strlen(file) + 2;
					rcfile = calloc(len, sizeof(*rcfile));
					strcpy(rcfile, home);
					strcat(rcfile, "/");
					strcat(rcfile, file);
				}
			}
			wm->rcfile = rcfile;
		} else {
			len = strlen(home) + strlen(names[1]) + 1;
			wm->rcfile = calloc(len, sizeof(*wm->rcfile));
			strcpy(wm->rcfile, home);
			strcat(wm->rcfile, names[1]);
		}
	}
	xde_get_simple_dirs(xtwm);
	{
		static const char *dirs[] = {
			"/etc/X11/",
			"/usr/share/X11/",
			"/etc/",
			"/usr/share/",
			NULL
		};
		const char **dirp;

		for (dirp = dirs; *dirp; dirp++) {
			size_t plen;
			struct stat st;
			char *path;
			
			plen = strlen(*dirp) + strlen(xtwm) + strlen("/system.")
				+ strlen(xtwm) + strlen("rc") + 1;
			path = calloc(plen, sizeof(*path));
			strcpy(path, *dirp);
			strcat(path, xtwm);
			strcat(path, "/system.");
			strcat(path, xtwm);
			strcat(path, "rc");
			if (!stat(path, &st) && S_ISREG(st.st_mode)) {
				*strrchr(path, '/') = '\0';
				free(wm->edir);
				wm->edir = strdup(path);
				free(path);
				return;

			}

		}
	}
}

__asm__(".symver __xde_get_rcfile_XTWM,xde_get_rcfile_XTWM@@XDE_1.0");

/** @} */

/** @name NONE
  */
/** @{ */

static void
get_rcfile_NONE()
{
}

static char *
find_style_NONE()
{
	get_rcfile_NONE();
	return NULL;
}

static char *
get_style_NONE()
{
	get_rcfile_NONE();
	return NULL;
}

static void
set_style_NONE()
{
	char *stylefile;

	if (!(stylefile = find_style_NONE())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		return;
	}
}

static void
reload_style_NONE()
{
}

static void
list_dir_NONE(char *xdir, char *style, enum ListType type)
{
}

static void
list_styles_NONE()
{
}

static char *
get_menu_NONE()
{
	get_rcfile_NONE();
	return NULL;
}

static WmOperations wm_ops_NONE = {
	"none",
	VERSION,
	&get_rcfile_NONE,
	&find_style_NONE,
	&get_style_NONE,
	&set_style_NONE,
	&reload_style_NONE,
	&list_dir_NONE,
	&list_styles_NONE,
	&get_menu_NONE
};

/** @} */

/** @name UNKNOWN
  */
/** @{ */

static void
get_rcfile_UNKNOWN()
{
}

static char *
find_style_UNKNOWN()
{
	get_rcfile_UNKNOWN();
	return NULL;
}

static char *
get_menu_UNKNOWN()
{
	get_rcfile_UNKNOWN();
	return NULL;
}

static char *
get_style_UNKNOWN()
{
	get_rcfile_UNKNOWN();
	return NULL;
}

static void
set_style_UNKNOWN()
{
	char *stylefile;

	if (!(stylefile = find_style_UNKNOWN())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		return;
	}
}

static void
reload_style_UNKNOWN()
{
}

static void
list_dir_UNKNOWN(char *xdir, char *style, enum ListType type)
{
}

static void
list_styles_UNKNOWN()
{
}

static WmOperations wm_ops_UNKNOWN = {
	"unknown",
	VERSION,
	&get_rcfile_UNKNOWN,
	&find_style_UNKNOWN,
	&get_style_UNKNOWN,
	&set_style_UNKNOWN,
	&reload_style_UNKNOWN,
	&list_dir_UNKNOWN,
	&list_styles_UNKNOWN,
	&get_menu_UNKNOWN
};

/** @} */

/** @name window manager specific operations
  *
  * @{ */

WmOperations *wm_ops[] = {
	&wm_ops_NONE,
	&wm_ops_UNKNOWN,
	NULL
};

static WmOperations *
get_wm_ops()
{
	WmOperations **ops, *loaded;
	char dlfile[256];
	void *handle;

	if (!wm)
		return &wm_ops_NONE;
	if (!wm->name)
		return &wm_ops_UNKNOWN;

	for (ops = wm_ops; *ops; ops++)
		if (!strcmp((*ops)->name, wm->name))
			break;
	if (*ops)
		return *ops;

	DPRINTF("wm name %s not found in operations list\n", wm->name);
	snprintf(dlfile, sizeof(dlfile), "xde-%s.so", wm->name);
	DPRINTF("attempting to dlopen %s\n", dlfile);
	if ((handle = dlopen(dlfile, RTLD_NOW | RTLD_LOCAL))) {
		DPRINTF("dlopen of %s succeeded\n", dlfile);
		if ((loaded = dlsym(handle, "xde_wm_ops"))) {
			Dl_info info;

			DPRINTF("module version is %s\n", loaded->version);
			if (dladdr(loaded, &info)) {
				DPRINTF("dli_fname = %s\n", info.dli_fname);
				DPRINTF("dli_fbase = %p\n", info.dli_fbase);
				DPRINTF("dli_sname = %s\n", info.dli_sname);
				DPRINTF("dli_saddr = %p\n", info.dli_saddr);
			}
		}
		return loaded;
	} else {
		DPRINTF("dlopen of %s failed: %s\n", dlfile, dlerror());
		/* we could not load window manager specific operations by name, however, 
		   we can still use a set of generic operations based on NetWM/EWMH
		   support, Gnome1/WMH support, Maker, Motif or even ICCCM for that
		   matter. */
		return &wm_ops_UNKNOWN;
	}
}

/** @} */

/** @brief handle DestroyNotify event
  *
  * If we receive a destroy notification for any of the windows that are
  * controlled by the window manager, we need to defer a check to see whether
  * the window manager changed.
  */
Bool
handle_DestroyNotify(const XEvent *e)
{
	if (wm) {
		Window win = e->xdestroywindow.window;
		int i;

		for (i = 0; i < CHECK_WINS; i++)
			if (win == wm->wins[i])
				goto check;
		if (win == wm->proxy)
			goto check;
	}
	return False;
      check:
	xde_defer_wm_check(250);
	return True;
}

/** @name Event Handlers
  *
  * The following are event handlers for detecting various things.
  *
  * @{ */

/** @brief handle an Xlib event
  */
Bool
__xde_handle_event(const XEvent *ev)
{
	int i;

	if (!find_screen(ev->xany.window))
		return False;
	switch (ev->type) {
	case PropertyNotify:
		for (i = 0; atoms[i].name; i++) {
			if (atoms[i].value == ev->xproperty.atom) {
				if (atoms[i].handler)
					return (atoms[i].handler) (ev);
				break;
			}
		}
		break;
	case ClientMessage:
		for (i = 0; atoms[i].name; i++) {
			if (atoms[i].value == ev->xclient.message_type) {
				if (atoms[i].handler)
					return (atoms[i].handler) (ev);
				break;
			}
		}
		break;
	case DestroyNotify:
		return handle_DestroyNotify(ev);
	}
	return False;
}

__asm__(".symver __xde_handle_event,xde_handle_event@@XDE_1.0");

WmDeferred *deferred_wait;
WmDeferred *deferred_done;
WmDeferred **deferred_tail = &deferred_done;

/** @brief defer and action for later
  * @param action - action to perform later
  * @param delay - amount of time to delay before queuing action
  * @parma data - client data to pass to the action
  *
  * Defers and action to be processed after all pending X events have been
  * handled.  When #delay is zero (0), the action will be invoked immediately
  * after all pending X events have been processed.  When #delay is non-zero,
  * wait at least #delay millseconds before before processing the deferred
  * action, and then only after all pending X events have been handled.
  */
void
__xde_defer_action(void (*action) (XPointer), Time delay, XPointer data)
{
	WmDeferred *d;

	d = calloc(1, sizeof(*d));
	clock_gettime(CLOCK_MONOTONIC, &d->when);

	d->screen = scr->screen;
	d->data = data;

	if (delay) {
		WmDeferred *def, **prev;

		d->when.tv_sec += delay / 1000;
		delay = delay % 1000;
		if ((d->when.tv_nsec += delay * 1000000) > 1000000000) {
			d->when.tv_sec += 1;
			d->when.tv_nsec -= 1000000000;
		}
		for (prev = &deferred_wait, def = *prev; def;
		     prev = &def->next, def = *prev) {
			if (def->when.tv_sec < d->when.tv_sec)
				continue;
			if (def->when.tv_sec == d->when.tv_sec)
				if (def->when.tv_nsec < d->when.tv_nsec)
					continue;
			break;
		}
		d->next = def;
		*prev = d;
		if (deferred_wait == d) {
			struct itimerspec when;

			when.it_interval.tv_sec = 0;
			when.it_interval.tv_nsec = 0;
			when.it_value.tv_sec = d->when.tv_sec;
			when.it_value.tv_nsec = d->when.tv_nsec;
			timerfd_settime(defer_timer, TFD_TIMER_ABSTIME, &when, NULL);
		}
	} else {
		d->next = NULL;
		*deferred_tail = d;
		deferred_tail = &d->next;
	}
}

__asm__(".symver __xde_defer_action,xde_defer_action@@XDE_1.0");

/** @brief defer and action for later once
  * @param action - action to perform once later
  * @param delay - amount of time to delay before queuing action
  * @param data - client data to pass to the action
  * @return Bool - False when action already deferred; True otherwise.
  *
  * Like xde_defer_action(), except if the action has already been deferred with
  * the same client data, return false and do not queue another instance of the
  * same action.
  */
Bool
__xde_defer_once(void (*action)(XPointer), Time delay, XPointer data)
{
	WmDeferred *d;

	if (delay) {
		for (d = deferred_wait; d ; d = d->next)
			if (d->action == action && d->data == data)
				return False;
	}
	for (d = deferred_done; d ; d = d->next)
		if (d->action == action && d->data == data)
			return False;

	xde_defer_action(action, delay, data);
	return True;
}

__asm__(".symver __xde_defer_once,xde_defer_once@@XDE_1.0");


static int signum;

/** @brief function to invoke from within signal handler
  * @param sig - signal number
  *
  * Can be used as a function to signal() to handle a signal not otherwise
  * handled by the library.
  */
void
__xde_sig_handler(int sig)
{
	signum = sig;
}

__asm__(".symver __xde_sig_handler,xde_sig_handler@@XDE_1.0");

XPointer retval;
int shutting_down;

/** @brief quit the main loop
  * @param value - value to return from xde_main_loop()
  *
  * Quits the main loop and causes xde_main_loop() to return the value
  * specified.
  */
void
__xde_main_quit(XPointer value)
{
	retval = value;
	shutting_down = 1;
}

__asm__(".symver __xde_main_quit,xde_main_quit@@XDE_1.0");

/** @brief process timeouts
  *
  * Used to process timeouts on the defer timer file descriptor.  You should
  * only need to call this if you are writing your own event loop.
  */
void
__xde_process_timeouts()
{
	unsigned long long count, i;

	if (read(defer_timer, &count, sizeof(count)) < sizeof(count))
		return;

	for (i = 0; i < count; i++) {
		WmDeferred *d;

		if ((d = deferred_wait)) {
			deferred_wait = d->next;
			d->next = *deferred_tail;
			deferred_tail = &d->next;
		}
	}
}

__asm__(".symver __xde_process_timeouts,xde_process_timeouts@@XDE_1.0");

/** @brief process X library events
  *
  * Use to process X library events.  You should only need to call this if you
  * are writing your own event loop.
  */
void
__xde_process_xevents()
{
	do {
		XEvent ev;

		while (XPending(dpy)) {
			XNextEvent(dpy, &ev);
			if (!callbacks || !callbacks->wm_event
			    || !(callbacks->wm_event) (&ev))
				xde_handle_event(&ev);
		}
		if (deferred_done)
			XSync(dpy, False);
	}
	while (deferred_done && XPending(dpy));
}

__asm__(".symver __xde_process_xevents,xde_process_xevents@@XDE_1.0");

/** @brief process deferred events
  *
  * Process all deferred events that have expired their waiting intervals.  You
  * should only need to call this if you are writing your own event loop.
  */
void
__xde_process_deferred(void)
{
	WmDeferred *d;

	while ((d = deferred_done)) {
		xde_set_screen(d->screen);
		(d->action) (d->data);
		deferred_done = d->next;
		free(d);
	}
	deferred_tail = &deferred_done;
}

__asm__(".symver __xde_process_deferred,xde_process_deferred@@XDE_1.0");

/** @brief handle signals
  *
  * Process signals within the event loop (instead of the signal handler).  You
  * should only need to call this if you are writing your own event loop.
  */
void
__xde_handle_signal(int sig)
{
	switch (sig) {
	case SIGINT:
	case SIGHUP:
	case SIGTERM:
	case SIGQUIT:
		xde_main_quit(NULL);
		break;
	}
}

__asm__(".symver __xde_handle_signal,xde_handle_signal@@XDE_1.0");

/** @brief main event loop for XDE library
  * @return XPointer - value passed by xde_main_quit()
  */
XPointer
__xde_main_loop(void)
{
	int tfd = xde_defer_timer();
	int xfd = ConnectionNumber(dpy);

	signal(SIGINT, &xde_sig_handler);
	signal(SIGHUP, &xde_sig_handler);
	signal(SIGTERM, &xde_sig_handler);
	signal(SIGQUIT, &xde_sig_handler);

	for (;;) {
		struct pollfd pfd[] = {
			{tfd, POLLIN | POLLERR | POLLHUP, 0},
			{xfd, POLLIN | POLLERR | POLLHUP, 0}
		};

		if (shutting_down)
			break;

		if (signum) {
			if (!callbacks || !callbacks->wm_signal ||
			    !(callbacks->wm_signal) (signum))
				xde_handle_signal(signum);
			signum = 0;
		}

		pfd[0].revents = 0;
		pfd[1].revents = 0;

		XFlush(dpy);

		if (shutting_down)
			break;

		switch (poll(pfd, 2, -1)) {
		case -1:
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART)
				continue;
			EPRINTF("poll: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		case 0:	/* timeout */
			continue;
		case 1:
		case 2:
			if (pfd[0].revents & POLLIN) {
				xde_process_timeouts();
				if (shutting_down)
					break;
			}
			if (pfd[1].revents & POLLIN) {
				xde_process_xevents();
				if (shutting_down)
					break;
			}
			if ((pfd[0].
			     revents | pfd[1].revents) & (POLLNVAL | POLLHUP | POLLERR)) {
				EPRINTF("fatal error on poll\n");
				exit(EXIT_FAILURE);
			}
		}
		xde_process_deferred();
		if (shutting_down)
			break;
	}
	return (retval);
}

__asm__(".symver __xde_main_loop,xde_main_loop@@XDE_1.0");

/** @brief defer a window manager recheck
  */
void
__xde_defer_wm_check(Time delay)
{
	xde_defer_once(xde_action_check_wm, 250, NULL);
}

__asm__(".symver __xde_defer_wm_check,xde_defer_wm_check@@XDE_1.0");

/** @brief defer a window manager theme check
  */

void
__xde_defer_theme_check(Time delay)
{
	xde_defer_once(xde_action_check_theme, delay, NULL);
}

__asm__(".symver __xde_defer_theme_check,xde_defer_theme_check@@XDE_1.0");

static Bool
is_wm_window(Window win)
{
	if (win == scr->root)
		return True;
	if (wm) {
		int i;

		for (i = 0; i < CHECK_WINS; i++)
			if (win == wm->wins[i])
				return True;
		if (win == wm->proxy)
			return True;
	}
	return False;
}

static Bool
is_wm_property(const XEvent *e)
{
	if (!e || e->type != PropertyNotify || !is_wm_window(e->xproperty.window))
		return False;
	if (!wm)
		xde_defer_wm_check(250);
	return True;
}

static Bool
is_netwm_property(const XEvent *e)
{
	if (!e || e->type != PropertyNotify || !is_wm_window(e->xproperty.window))
		return False;
	if (e->xproperty.state != PropertyDelete)
		if (!wm || !wm->netwm_check)
			xde_defer_wm_check(250);
	return True;
}

static Bool
is_winwm_property(const XEvent *e)
{
	if (!e || e->type != PropertyNotify || !is_wm_window(e->xproperty.window))
		return False;
	if (e->xproperty.state != PropertyDelete)
		if (!wm || !wm->winwm_check)
			xde_defer_wm_check(250);
	return True;
}

static Bool
is_maker_property(const XEvent *e)
{
	if (!e || e->type != PropertyNotify || !is_wm_window(e->xproperty.window))
		return False;
	if (e->xproperty.state != PropertyDelete)
		if (!wm || !wm->maker_check)
			xde_defer_wm_check(250);
	return True;
}

static Bool
is_motif_property(const XEvent *e)
{
	if (!e || e->type != PropertyNotify || !is_wm_window(e->xproperty.window))
		return False;
	if (e->xproperty.state != PropertyDelete)
		if (!wm || !wm->motif_check)
			xde_defer_wm_check(250);
	return True;
}

static Bool
is_root_property(const XEvent *e)
{
	if (!e || e->type != PropertyNotify || e->xproperty.window != scr->root)
		return False;
	return True;
}


/** @brief handle _BB_THEME property notification
  *
  * Our blackbox(1) theme files have a rootCommand that changes the _BB_THEME
  * property on the root window.  Check the theme again when it changes.
  */
static Bool
handle_BB_THEME(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_theme_check(0);
	return True;
}

/** @brief handle _BLACKBOX_PID property notification
  *
  * When fluxbox(1) restarts, it does not change the _NET_SUPPORTING_WM_CHECK
  * window, but it does change the _BLACKBOX_PID cardinal (with our setup), even
  * if it is just to replace it with the same value.  When restarting, recheck
  * the theme.
  */
static Bool
handle_BLACKBOX_PID(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_DT_WORKSPACE_CURRENT(const XEvent *e)
{
	if (!is_motif_property(e))
		return False;
	return False;
}

static Bool
handle_DT_WORKSPACE_LIST(const XEvent *e)
{
	if (!is_motif_property(e))
		return False;
	return False;
}

/** @brief handle ESETROOT_PMAP_ID property changes
  *
  * We do not really process this because all proper root setters now set the
  * _XROOTPMAP_ID property which we handle above.  However, it is used to
  * trigger recheck of the theme needed by some window managers such as
  * blackbox(1).  If it means we check 3 times after a theme switch, so be it.
  */
static Bool
handle_ESETROOT_PMAP_ID(const XEvent *e)
{
	if (!is_root_property(e))
		return False;
	xde_defer_theme_check(0);
	return True;
}

static Bool
handle_GTK_READ_RCFILES(const XEvent *e)
{
	if (!e || e->type != ClientMessage)
		return False;
	xde_defer_theme_check(0);
	return True;
}

static Bool
handle_I3_CONFIG_PATH(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_I3_PID(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_I3_SHMLOG_PATH(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_I3_SOCKET_PATH(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_ICEWMGB_QUIT(const XEvent *e)
{
	if (!e || e->type != ClientMessage)
		return False;
	return False;
}

/** @brief handle MANAGER client messages
  */
static Bool
handle_MANAGER(const XEvent *e)
{
	if (!e || e->type != ClientMessage)
		return False;
	return False;
}

/** @brief handle _MOTIF_WM_INFO property change
  *
  * This property is set by window managers that support Motif/MWMH.  It only
  * changes when the window manager changes, so recheck the window manager.
  */
static Bool
handle_MOTIF_WM_INFO(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_NET_ACTIVE_WINDOW(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_CLIENT_LIST(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_CLIENT_LIST_STACKING(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

/** @brief handle _NET_CURRENT_DESKTOP property change
  *
  * Handle when _NET_CURRENT_DESKTOP property changes on the root window of any
  * screen.  This is how we determine that the desktop has changed.
  */
static Bool
handle_NET_CURRENT_DESKTOP(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_DESKTOP(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_DESKTOP_GEOMETRY(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_DESKTOP_LAYOUT(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_DESKTOP_MASK(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_DESKTOP_NAMES(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_DESKTOP_PIXMAPS(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_DESKTOP_VIEWPORT(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_FULL_PLACEMENT(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_FULLSCREEN_MONITORS(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_HANDLED_ICONS(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_ICON_GEOMETRY(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_NUMBER_OF_DESKTOPS(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_PROPERTIES(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_SHOWING_DESKTOP(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_SUPPORTED(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return True;
}

static Bool
handle_NET_SUPPORTING_WM_CHECK(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return True;
}

static Bool
handle_NET_VIRTUAL_POS(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_VIRTUAL_ROOTS(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_VISIBLE_DESKTOPS(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_NET_WM_NAME(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_NET_WM_PID(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_NET_WORKAREA(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_OB_THEME(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	return False;
}

static Bool
handle_OPENBOX_PID(const XEvent *e)
{
	if (!is_netwm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_WIN_AREA(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	return False;
}

static Bool
handle_WIN_AREA_COUNT(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	return False;
}

static Bool
handle_WIN_CLIENT_LIST(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	return False;
}

static Bool
handle_WIN_DESKTOP_BUTTON_PROXY(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_WIN_FOCUS(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	return False;
}

static Bool
handle_WIN_PROTOCOLS(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_WIN_SUPPORTING_WM_CHECK(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_WIN_WORKSPACE(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	return False;
}

static Bool
handle_WIN_WORKSPACE_COUNT(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	return False;
}

static Bool
handle_WIN_WORKSPACE_NAMES(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	return False;
}

static Bool
handle_WIN_WORKSPACES(const XEvent *e)
{
	if (!is_winwm_property(e))
		return False;
	return False;
}

static Bool
handle_WINDOWMAKER_NOTICEBOARD(const XEvent *e)
{
	if (!is_maker_property(e))
		return False;
	return False;
}

static Bool
handle_WM_CLASS(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return False;
}

static Bool
handle_WM_CLIENT_MACHINE(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_WM_COMMAND(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return False;
}

static Bool
handle_WM_DESKTOP(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	return False;
}

static Bool
handle_WM_NAME(const XEvent *e)
{
	if (!is_wm_property(e))
		return False;
	xde_defer_wm_check(250);
	return True;
}

static Bool
handle_XDE_THEME_NAME(const XEvent *e)
{
	if (!is_root_property(e))
		return False;
	xde_defer_theme_check(0);
	return False;
}

static Bool
handle_XROOTPMAP_ID(const XEvent *e)
{
	if (!is_root_property(e))
		return False;
	return False;
}

static Bool
handle_XSETROOT_ID(const XEvent *e)
{
	if (!is_root_property(e))
		return False;
	return False;
}

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
