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

#include "xde.h"

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <sys/poll.h>
#include <sys/timerfd.h>

const char *program = NAME;

static enum { StartingUp, WaitForWM, WaitAfterWM, DetectWM, ShutDown } state;

/* Structure representing a single pixmap of an image at a specific size */
typedef struct {
	unsigned int width, height;
	Pixmap pixmap;
} BdPixmap;

/* structure representing a set of pixmaps for an image of various sizes */
typedef struct {
	int count;
	BdPixmap *pixmaps;
} BdImage;

/* structure representing a desktop on a monitor */
typedef struct {
	int index;
	BdImage *image;
} BdDesktop;

/* structure representing a monitor of a screen */
typedef struct {
	int index;
	int x, y;
	unsigned int width, height;
	int row, col;			/* r&c in monitor layout */
	Window backdrop;		/* backdrop window for this monitor */
	BdDesktop *current_desktop;
} BdMonitor;

BdMonitor *monitors;

/* structure representing a screen of a display */
typedef struct {
	int index;
	int x, y;
	unsigned int width, height;
	int row, col;			/* r&c in screen layout */
	int number_of_desktops;
	int current_desktop;
	int workspace_count;
	int workspace;
	BdDesktop *desktops;
} BdScreen;

#define EXTRANGE    16		/* all (used) X11 extension events must fit in this range 
				 */

enum { XrandrBase, XineramaBase, XsyncBase, BaseLast };

typedef struct {
	Bool have;
	int base;
} Extension;

Extention ext[BaseLast];

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

typedef struct {
	char *name;
	Atom *atom;
	void (*pc_handler) (XEvent *);	/* property change handler */
	void (*cm_handler) (XEvent *);	/* client message handler */
	Atom value;
} AtomHandler;

static AtomHandler myatoms[] = {
	/* *INDENT-OFF* **/
	{ "_WIN_SUPPORTING_WM_CHECK",	&_XA_WIN_SUPPORTING_WM_CHECK,	&pc_handle_WIN_SUPPORTING_WM_CHECK,	NULL,					None },
	{ "_NET_WM_NAME",		&_XA_NET_WM_NAME,		&pc_handle_NET_WM_NAME,			NULL,					None },
	{ "_ICEWMBG_QUIT",		&_XA_ICEWMBG_QUIT,		&pc_handle_ICEWMBG_QUIT,		NULL,					None },
	{ "ESETROOT_PMAP_ID",		&_XA_ESETROOT_PMAP_ID,		&pc_handle_ESETROOT_PMAP_ID,		NULL,					None },
	{ "_XSETROOT_ID",		&_XA_XSETROOT_ID,		&pc_handle_XSETROOT_ID,			NULL,					None },
	{ "_XROOTMAP_ID",		&_XA_XROOTMAP_ID,		&pc_handle_XROOTMAP_ID,			NULL,					None },
	{ "_WIN_DESKTOP_BUTTON_PROXY",	&_XA_WIN_DESKTOP_BUTTON_PROXY,	&pc_handle_WIN_DESKTOP_BUTTON_PROXY,	NULL,					None },
	{ "_NET_NUMBER_OF_DESKTOPS",	&_XA_NET_NUMBER_OF_DESKTOPS,	&pc_handle_NET_NUMBER_OF_DESKTOPS,	NULL,					None },
	{ "_NET_CURRENT_DESKTOP",	&_XA_NET_CURRENT_DESKTOP,	&pc_handle_NET_CURRENT_DESKTOP,		NULL,					None },
	{ "_WIN_WORKSPACE_COUNT",	&_XA_WIN_WORKSPACE_COUNT,	&pc_handle_WIN_WORKSPACE_COUNT,		NULL,					None },
	{ "_WIN_WORKSPACE",		&_XA_WIN_WORKSPACE,		&pc_handle_WIN_WORKSPACE,		NULL,					None },
	{ "_NET_DESKTOP_PIXMAPS",	&_XA_NET_DESKTOP_PIXMAP,	&pc_handle_NET_DESKTOP_PIXMAPS,		NULL,					None },
	{ "_XROOTPMAP_ID",		&_XA_XROOTPMAP_ID,		&pc_handle_XROOTPMAP_ID,		NULL,					None },
	{ "_NET_DESKTOP_NAMES",		&_XA_NET_DESKTOP_NAMES,		&pc_handle_NET_DESKTOP_NAMES,		NULL,					None },
	{ "_OB_THEME",			&_XA_OB_THEME,			&pc_handle_OB_THEME,			NULL,					None },
	{ "_BB_THEME",			&_XA_BB_THEME,			&pc_handle_BB_THEME,			NULL,					None },
	{ "_XDE_THEME",			&_XA_XDE_THEME,			&pc_handle_XDE_THEME,			NULL,					None },
	{ NULL,				NULL,				NULL,					NULL,					None }
	/* *INDENT-ON* **/
};


/** @brief handle ButtonPress events
  * @param ev - X Event
  *
  * We receive button press events from the desktop button proxy or from our own backdrop
  * windows.
  * 
  */
static void
handle_ButtonPress(XEvent *ev)
{
}

static void
handle_ButtonRelease(XEvent *ev)
{
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

	for (i = 0; myatoms[i].name; i++)
		if (myatoms[i].value == ev->xproperty.atom) {
			if (myatoms[i].pc_handler)
				return myatoms[i].pc_handler(ev);
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

	if ((type = ev->xclient.message_type) == XA_WM_PROTOCOLS)
		type = ev->xclient.data.l[0];
	for (i = 0; myatoms[i].name; i++)
		if (myatoms[i].value == type) {
			if (myatoms[i].cm_handler)
				return myatoms[i].cm_handler(ev);
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
#ifdef XRANDR
	[RRScreenChangeNotify + LASTEvent + EXTRANGE * XrandrBase] = handle_RRScreenChangeNotify,
#endif
	/* *INDENT-ON* */
};

static void
handle_event(XEvent *ev)
{
	int i, slot;

	if (ev->type <= -LASTEvent)
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

#ifdef XINERAMA
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
		} while (0);
	}
#endif				/* XINERAMA */
}

static void
bd_init_xrandr()
{
	int dummy;

#ifdef XRANDR
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
		} while (0);
	}
#endif				/* XRANDR */
}

static void
mainloop()
{
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
		XSelectInput(dpy, root, PropertyChangeMask | SubstructureNotifyMask);
		XSaveContext(dpy, root, ScreenContext, (XPointer) scr);
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
			if ((pfd[0].revents & pfd[1].
			     revents) & (POLLNVAL | POLLHUP | POLLERR)) {
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
	mainloop();
	exit(0);
}

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
