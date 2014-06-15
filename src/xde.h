/*****************************************************************************

 Copyright (c) 2008-2013  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

#ifndef __XDE_H__
#define __XDE_H__

#ifdef HAVE_CONFIG_H
#include "autoconf.h"
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>

#include <assert.h>
#include <locale.h>
#include <stdarg.h>
#include <strings.h>
#include <regex.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#ifdef XRANDR
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/randr.h>
#endif
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#ifdef STARTUP_NOTIFICATION
#define SN_API_NOT_YET_FROZEN
#include <libsn/sn.h>
#endif

#define CHECK_DIRS 4
#define CHECK_WINS 6

enum ListType {
	XDE_LIST_PRIVATE,
	XDE_LIST_USER,
	XDE_LIST_SYSTEM,
	XDE_LIST_GLOBAL,
	XDE_LIST_MIXED
};

typedef struct {
	char *name;
	char *version;
	void (*get_rcfile) (void);
	char *(*find_style) (void);
	char *(*get_style) (void);
	void (*set_style) (void);
	void (*reload_style) (void);
	void (*list_dir) (char *, char *, enum ListType);
	void (*list_styles) (void);
	char *(*get_menu) (void);
	void (*gen_item) (char *, enum ListType, char *, char *);
	void (*gen_dir) (char *, char *, enum ListType);
	void (*gen_menu) (void);
	char *(*get_icon) (void);
} WmOperations;

typedef struct _WmScreen WmScreen;
typedef struct _WindowManager WindowManager;

enum {
	XDE_EVENT_PROPAGATE = False,	/* continue processing */
	XDE_EVENT_STOP = True,	/* stop processing */
};

typedef struct {
	Bool (*wm_event) (const XEvent *);	/* event handler */
	Bool (*wm_signal) (int);	/* signal handler */
	void (*wm_changed) (void);	/* window manager changed */
	void (*wm_style_changed) (char *, char *, char *);	/* window manager style
								   changed */
	void (*wm_menu_changed) (char *);
	void (*wm_icon_changed) (char *);
	void (*wm_theme_changed) (char *, char *);	/* window manager theme changed */
	void (*wm_desktop_changed) (int, unsigned long *);
	/* current desktop(s) changed */
	void (*wm_desktops_changed) (unsigned long);
	/* number of desktops changed */
} WmCallbacks;

typedef struct _WmDeferred {
	struct _WmDeferred *next;	/* list linkage */
	void (*action) (XPointer);	/* action to perform when invoked */
	int screen;			/* active screen */
	struct timespec when;		/* expiry time */
	XPointer data;			/* hook for client data */
} WmDeferred;

struct _WindowManager {
	int refs;			/* references to this structure */
	WmOperations *ops;		/* operations */
	union {
		struct {
			Window netwm_check;	/* NetWM/EWMH check window */
			Window winwm_check;	/* GNOME1/WMH check window */
			Window maker_check;	/* WindowMaker Noticeboard check window */
			Window motif_check;	/* Motif/MWMH check window */
			Window icccm_check;	/* ICCCM 2.0 manager window */
			Window redir_check;	/* ICCCM root window */
		};
		Window wins[CHECK_WINS];
	};
	Window proxy;			/* desktop button proxy */
	long pid;			/* window manager pid */
	char *host;			/* window manager host */
	char *name;			/* window manager name */
	char **argv;			/* window manager command line argv[] */
	int argc;			/* window manager command line argc */
	char **cargv;			/* wm WM_COMMAND argv[] */
	int cargc;			/* wm WM_COMMAND argc */
	XClassHint ch;			/* window manager WM_CLASS */
	char *rcfile;			/* window manager rc file */
	union {
		struct {
			char *pdir;	/* WM private directory */
			char *udir;	/* WM user directory */
			char *sdir;	/* WM system directory */
			char *edir;	/* WM config directory */
		};
		char *dirs[CHECK_DIRS];
	};
	char *stylefile;		/* Window manager style file */
	char *style;			/* WM current style */
	char *stylename;		/* WM current style name */
	char *menu;			/* WM current menu */
	Bool noenv;			/* Do we have an environment? */
	char *env;			/* Window manager environment */
	size_t nenv;			/* Number of characters in environment */
	XrmDatabase db;			/* WM resource database */
	char **xdg_dirs;		/* XDG data dirs */
	char *icon;			/* Icon for window manager */
};

typedef struct WmImage WmImage;
struct WmImage {
	WmImage *next;
	int num;
	char *file;
	unsigned int width, height;
	Pixmap pmid;
};

typedef struct {
	WmImage *image;			/* image assigned to this desktop */
} WmArea;

typedef struct {
	int num;
	int row, col;			/* row and column indices */
	struct {
		int current;
		unsigned int numb;
		unsigned int cols;
		unsigned int rows;
		WmArea *areas;		/* areas belonging to this desktop */
	} a;
	WmImage *image;			/* image assigned to this desktop */
} WmDesktop;

typedef struct {
	int num;
	int x, y;
	unsigned int width, height;
	struct {
		int current;
		unsigned int numb;
		unsigned int cols;
		unsigned int rows;
		WmImage *images;	/* points to pixmap sized for monitor */
	} i;
} WmMonitor;

struct _WmScreen {
	int screen;			/* screen number */
	int x, y;			/* geometry in pixels */
	unsigned int width, height;
	Window root;			/* root window for this screen */
	Atom selection;			/* MANAGER selection atom for screen */
	Window selwin;			/* MANAGER selection window for screen */
	Window owner;			/* MANAGER selection owner */
	WindowManager *wm;		/* window manager managing this screen */
	char *theme;			/* XDE theme name */
	char *themefile;		/* XDE theme file */
	struct {
		int current;		/* current monitor/desktop */
		unsigned int numb;	/* number monitors/desktops */
		unsigned int cols;	/* cols of monitors/desktops */
		unsigned int rows;	/* rows of monitors/desktops */
		union {
			WmMonitor *monitors;	/* monitors belonging to this screen */
			WmDesktop *desktops;	/* desktops belonging to this screen */
		};
	} m, d;
	Pixmap pmid;			/* pixmap for entire screen */
	Pixmap save;			/* backing store pixmap for entire screen */
	int numdesk;			/* number of desktops for this screen */
	int curdesk;			/* current desktop for this screen */
};

typedef struct {
	Display *dpy;
	struct {
		int current;		/* current screen/image */
		unsigned int numb;	/* number screens/images */
		unsigned int cols;	/* cols of screens/images */
		unsigned int rows;	/* rows of screens/images */
		union {
			WmScreen *screens;	/* screens belonging to this display */
			WmImage *images;	/* images belonging to this display */
		};
	} s, i;
} WmDisplay;

enum OutputFormat {
	XDE_OUTPUT_HUMAN,
	XDE_OUTPUT_SHELL,
	XDE_OUTPUT_PERL,
	XDE_OUTPUT_PROPS
};

typedef struct {
	int debug;
	int output;
	Bool current;
	Bool menu;
	Bool list;
	Bool set;
	Bool system;
	Bool user;
	Bool link;
	Bool theme;
	Bool dryrun;
	Bool reload;
	int screen;
	char *style;
	char *wmname;
	char *rcfile;
	enum OutputFormat format;
	Bool grab;
	Bool setroot;
	Bool nomonitor;
	unsigned long delay;
	Bool areas;
	char **files;
	Bool remove;
	Bool replace;
	Bool assist;
} Options;

extern Display *dpy;
extern int screen;
extern Window root;
extern WindowManager *wm;
extern WmScreen *screens;
extern WmScreen *scr;
extern WmScreen *event_scr;
extern unsigned int nscr;
extern WmImage *images;
extern WmDesktop *dsk;

#define XPRINTF(args...) do { } while (0)
#define OPRINTF(args...) do { if (options.output > 1) { \
	fprintf(stderr, "I: "); \
	fprintf(stderr, args); \
	fflush(stderr); } } while (0)
#define DPRINTF(args...) do { if (options.debug) { \
	fprintf(stderr, "D: %s +%d %s(): ", __FILE__, __LINE__, __func__); \
	fprintf(stderr, args); \
	fflush(stderr); } } while (0)
#define EPRINTF(args...) do { \
	fprintf(stderr, "E: %s +%d %s(): ", __FILE__, __LINE__, __func__); \
	fprintf(stderr, args); \
	fflush(stderr);   } while (0)
#define DPRINT() do { if (options.debug) { \
	fprintf(stderr, "D: %s +%d %s()\n", __FILE__, __LINE__, __func__); \
	fflush(stderr); } } while (0)

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
extern Atom _XA_MANAGER;
extern Atom _XA_MOTIF_WM_INFO;

extern Atom _XA_NET_ACTIVE_WINDOW;
extern Atom _XA_NET_CLIENT_LIST;
extern Atom _XA_NET_CLIENT_LIST_STACKING;
extern Atom _XA_NET_CURRENT_DESKTOP;
extern Atom _XA_NET_DESKTOP;
extern Atom _XA_NET_DESKTOP_GEOMETRY;
extern Atom _XA_NET_DESKTOP_LAYOUT;
extern Atom _XA_NET_DESKTOP_MASK;
extern Atom _XA_NET_DESKTOP_NAMES;
extern Atom _XA_NET_DESKTOP_PIXMAPS;
extern Atom _XA_NET_DESKTOP_VIEWPORT;
extern Atom _XA_NET_FULL_PLACEMENT;
extern Atom _XA_NET_FULLSCREEN_MONITORS;
extern Atom _XA_NET_HANDLED_ICONS;
extern Atom _XA_NET_ICON_GEOMETRY;
extern Atom _XA_NET_NUMBER_OF_DESKTOPS;
extern Atom _XA_NET_PROPERTIES;
extern Atom _XA_NET_SHOWING_DESKTOP;
extern Atom _XA_NET_SUPPORTED;
extern Atom _XA_NET_SUPPORTING_WM_CHECK;
extern Atom _XA_NET_VIRTUAL_POS;
extern Atom _XA_NET_VIRTUAL_ROOTS;
extern Atom _XA_NET_VISIBLE_DESKTOPS;
extern Atom _XA_NET_WM_NAME;
extern Atom _XA_NET_WM_PID;
extern Atom _XA_NET_WORKAREA;

extern Atom _XA_OB_THEME;
extern Atom _XA_OPENBOX_PID;

extern Atom _XA_WIN_AREA;
extern Atom _XA_WIN_AREA_COUNT;
extern Atom _XA_WIN_CLIENT_LIST;
extern Atom _XA_WIN_DESKTOP_BUTTON_PROXY;
extern Atom _XA_WIN_FOCUS;
extern Atom _XA_WIN_PROTOCOLS;
extern Atom _XA_WIN_SUPPORTING_WM_CHECK;
extern Atom _XA_WIN_WORKSPACE;
extern Atom _XA_WIN_WORKSPACE_COUNT;
extern Atom _XA_WIN_WORKSPACE_NAMES;
extern Atom _XA_WIN_WORKSPACES;

extern Atom _XA_WINDOWMAKER_NOTICEBOARD;

extern Atom _XA_WM_DESKTOP;
extern Atom _XA_XDE_THEME_NAME;
extern Atom _XA_XDE_WM_INFO;
extern Atom _XA_XDE_WM_NAME;
extern Atom _XA_XDE_WM_NETWM_SUPPORT;
extern Atom _XA_XDE_WM_WINWM_SUPPORT;
extern Atom _XA_XDE_WM_MAKER_SUPPORT;
extern Atom _XA_XDE_WM_MOTIF_SUPPORT;
extern Atom _XA_XDE_WM_ICCCM_SUPPORT;
extern Atom _XA_XDE_WM_REDIR_SUPPORT;
extern Atom _XA_XDE_WM_PID;
extern Atom _XA_XDE_WM_HOST;
extern Atom _XA_XDE_WM_CLASS;
extern Atom _XA_XDE_WM_CMDLINE;
extern Atom _XA_XDE_WM_COMMAND;
extern Atom _XA_XDE_WM_RCFILE;
extern Atom _XA_XDE_WM_PRVDIR;
extern Atom _XA_XDE_WM_USRDIR;
extern Atom _XA_XDE_WM_SYSDIR;
extern Atom _XA_XDE_WM_ETCDIR;
extern Atom _XA_XDE_WM_STYLEFILE;
extern Atom _XA_XDE_WM_STYLE;
extern Atom _XA_XDE_WM_STYLENAME;
extern Atom _XA_XDE_WM_MENU;
extern Atom _XA_XDE_WM_ICON;
extern Atom _XA_XDE_WM_THEME;
extern Atom _XA_XDE_WM_THEMEFILE;
extern Atom _XA_XROOTPMAP_ID;
extern Atom _XA_XSETROOT_ID;

extern XContext ScreenContext;

extern Options options;

/* some utility functions */
extern void xde_delete_property(Window win, Atom prop);
extern void xde_set_text_list(Window win, Atom prop, XICCEncodingStyle style, char **list,
			      long n);
extern char *xde_get_text(Window win, Atom prop);
extern void xde_set_text(Window win, Atom prop, XICCEncodingStyle style, char *text);
extern long *xde_get_cardinals(Window win, Atom prop, Atom type, long *n);
extern void xde_set_cardinals(Window win, Atom prop, Atom type, long *cards, long n);
extern Bool xde_get_cardinal(Window win, Atom prop, Atom type, long *card_ret);
extern void xde_set_cardinal(Window win, Atom prop, Atom type, long card);
extern Window *xde_get_windows(Window win, Atom prop, Atom type, long *n);
extern void xde_set_windows(Window win, Atom prop, Atom type, Window *winds, long n);
extern Bool xde_get_window(Window win, Atom prop, Atom type, Window *win_ret);
extern void xde_set_window(Window win, Atom prop, Atom type, Window wind);
extern Time *xde_get_times(Window win, Atom prop, Atom type, long *n);
extern void xde_set_times(Window win, Atom prop, Atom type, Time *times, long n);
extern Bool xde_get_time(Window win, Atom prop, Atom type, Time *time_ret);
extern void xde_set_time(Window win, Atom prop, Atom type, Time time);
extern Atom *xde_get_atoms(Window win, Atom prop, Atom type, long *n);
extern void xde_set_atoms(Window win, Atom prop, Atom type, Atom *atoms, long n);
extern Bool xde_get_atom(Window win, Atom prop, Atom type, Atom *atom_ret);
extern void xde_set_atom(Window win, Atom prop, Atom type, Atom atom);
extern Pixmap *xde_get_pixmaps(Window win, Atom prop, Atom type, long *n);
extern void xde_set_pixmaps(Window win, Atom prop, Atom type, Pixmap *pmaps, long n);
extern Bool xde_get_pixmap(Window win, Atom prop, Atom type, Pixmap *pixmap_ret);
extern void xde_set_pixmap(Window win, Atom prop, Atom type, Pixmap pixmap);

/* action functions */
extern void xde_recheck_wm(void);
extern void xde_check_style(void);
extern void xde_check_menu(void);
extern void xde_check_icon(void);
extern void xde_check_theme(void);
extern void xde_action_check_wm(XPointer);
extern void xde_action_check_theme(XPointer);
extern void xde_defer_wm_check(Time delay);
extern void xde_defer_theme_check(Time delay);
extern void xde_wm_unref(WindowManager *);

/* event functions */
extern Bool xde_handle_event(const XEvent *ev);
extern void xde_defer_action(void (*)(XPointer), Time, XPointer);
extern Bool xde_defer_once(void (*)(XPointer), Time, XPointer);
extern void xde_sig_handler(int sig);
extern void xde_main_quit(XPointer);
extern void xde_process_timeouts(void);
extern void xde_process_xevents(void);
extern void xde_process_deferred(void);
extern void xde_handle_signal(int sig);
extern XPointer xde_main_loop(void);
extern int xde_defer_timer(void);
extern void xde_init(WmCallbacks *);

extern void xde_set_screen(int screennum);
extern void xde_init_display(void);
extern Bool xde_detect_wm(void);
extern void xde_show_wms(void);
extern void xde_identify_wm(void);
extern void xde_set_properties_on(Window win);
extern void xde_set_properties(void);
extern void xde_del_properties_from(Window win);
extern void xde_del_properties(void);
extern Bool xde_find_theme(char *name, char **filename);
extern char *xde_get_style(void);
extern char *xde_get_menu(void);
extern char *xde_get_icon(void);
extern char *xde_get_theme(void);
extern void xde_set_theme(char *name);
extern char *xde_get_proc_environ(char *name);
extern char *xde_get_rcfile_optarg(char *optname);
extern void xde_get_simple_dirs(char *wmname);
extern char *xde_find_style_simple(char *dname, char *fname, char *suffix);
extern Bool xde_test_file(char *path);
extern Bool xde_check_file(char *path);
extern void xde_list_dir_simple(char *xdir, char *dname, char *fname, char *suffix,
				char *style, enum ListType type);
extern void xde_list_styles_simple(void);
extern void xde_get_rcfile_simple(char *wmname, char *rcname, char *option);
extern char *xde_get_menu_simple(char *fname, char *(*from_file) (char *));
extern char *xde_get_style_simple(char *fname, char *(*from_file) (char *));
extern char *xde_get_style_database(char *name, char *clas);
extern char *xde_get_menu_database(char *name, char *clas);
extern void xde_set_style_simple(char *rcname, void (*to_file) (char *, char *));
extern void xde_set_style_database(char *name);
extern void xde_get_rcfile_XTWM(char *xtwm);
extern void xde_get_xdg_dirs(void);
extern void xde_gen_menu_simple(void);
extern void xde_gen_dir_simple(char *xdir, char *dname, char *fname, char *suffix,
			       char *style, enum ListType type);
extern Bool xde_check_wm(void);
extern char *xde_get_icon_simple(const char *fallback);

#endif				/* __XDE_H__ */
