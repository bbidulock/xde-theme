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
#include <sys/ioctl.h>
#include <sys/wait.h>
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

#define CHECK_DIRS 3
#define CHECK_WINS 6

typedef struct {
	char *name;
	void (*get_rcfile)(void);
	char *(*find_style)(void);
	char *(*get_style) (void);
	void (*set_style) (void);
	void (*reload_style)(void);
	void (*list_styles) (void);
} WmOperations;

typedef struct {
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
		};
		char *dirs[CHECK_DIRS];
	};
	char *stylefile;		/* Window manager style file */
	char *style;			/* WM current style */
	char *stylename;		/* WM current style name */
	Bool noenv;			/* Do we have an environment? */
	char *env;			/* Window manager environment */
	size_t nenv;			/* Number of characters in environment */
	XrmDatabase db;			/* WM resource database */
	char **xdg_dirs;		/* XDG data dirs */
} WindowManager;

typedef struct {
	int screen;			/* screen number */
	int x, y;			/* screen position */
	unsigned int width, height;	/* screen dimensions */
	Window root;			/* root window for this screen */
	WindowManager *wm;		/* window manager managing this screen */
} WmScreen;

typedef struct {
	int debug;
	int output;
	Bool current;
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
} Options;

extern Display *dpy;
extern int screen;
extern Window root;
extern WindowManager *wm;
extern WmScreen *screens;
extern WmScreen *scr;
extern unsigned int nscr;

#define OPRINTF(format, ...) do { if (options.output > 1) fprintf(stderr, "I: " format, __VA_ARGS__); } while (0)
#define DPRINTF(format, ...) do { if (options.debug) fprintf(stderr, "D: %s %s():%d " format, __FILE__, __func__, __LINE__, __VA_ARGS__); } while (0)
#define EPRINTF(format, ...) do { fprintf(stderr, "E: %s %s():%d " format, __FILE__, __func__, __LINE__, __VA_ARGS__); } while (0)

extern Atom _XA_BB_THEME;
extern Atom _XA_BLACKBOX_PID;
extern Atom _XA_MOTIF_WM_INFO;
extern Atom _XA_NET_SUPPORTING_WM_CHECK;
extern Atom _XA_NET_WM_NAME;
extern Atom _XA_NET_WM_PID;
extern Atom _XA_OB_THEME;
extern Atom _XA_OPENBOX_PID;
extern Atom _XA_WINDOWMAKER_NOTICEBOARD;
extern Atom _XA_WIN_SUPPORTING_WM_CHECK;

extern XContext ScreenContext;

extern Options options;

void xde_init_display(void);
Bool xde_detect_wm(void);
void xde_show_wms(void);
Bool xde_find_theme(char *name);

#endif				/* __XDE_H__ */
