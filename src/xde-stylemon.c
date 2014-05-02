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

const char *program = NAME;

static Bool
selectionreleased(Display *display, XEvent *event, XPointer arg)
{
	if (event->type == DestroyNotify && event->xdestroywindow.window == (Window) arg)
		return True;
	return False;
}

enum {
	CMD_MODE_RUN = 0,
	CMD_MODE_REPLACE = 1,
	CMD_MODE_QUIT = 2,
	CMD_MODE_SET = 3,
	CMD_MODE_HELP = 4,
	CMD_MODE_VERSION = 5,
	CMD_MODE_COPYING = 6
} command;

SmcConn smcconn;
IceConn iceconn;

char *client_id;

int running;
Atom _XA_MANAGER;

WmScreen *
getscreen(Window win)
{
	Window *wins = NULL, wroot = None, parent = None;
	unsigned int num = 0;
	WmScreen *s = NULL;

	if (!win)
		return (s);
	if (!XFindContext(dpy, win, ScreenContext, (XPointer *) &s))
		return (s);
	if (XQueryTree(dpy, win, &wroot, &parent, &wins, &num))
		XFindContext(dpy, wroot, ScreenContext, (XPointer *) &s);
	if (wins)
		XFree(wins);
	return (s);
}

WmScreen *
geteventscr(XEvent *ev)
{
	return (event_scr = getscreen(ev->xany.window) ? : scr);
}

static Bool
IGNOREEVENT(XEvent *e)
{
	DPRINTF("Got ignore event %d\n", e->type);
	return False;
}

static Bool
selectionclear(XEvent *e)
{
	if (e->xselectionclear.selection == scr->selection && scr->selwin) {
		int n;
		WmScreen *s;

		XDestroyWindow(dpy, scr->selwin);
		XDeleteContext(dpy, scr->selwin, ScreenContext);
		scr->selwin = None;
		if ((scr->owner = XGetSelectionOwner(dpy, scr->selection))) {
			XSelectInput(dpy, scr->owner, StructureNotifyMask);
			XSaveContext(dpy, scr->owner, ScreenContext, (XPointer) scr);
			XSync(dpy, False);
		}
		return True;
	}
	return False;
}

static Bool
clientmessage(XEvent *e)
{
	if (e->xclient.message_type != _XA_MANAGER)
		return False;
	if (e->xclient.data.l[1] != scr->selection)
		return False;
	if (!scr->owner || scr->owner != e->xclient.data.l[2]) {
		if (scr->owner) {
			XDeleteContext(dpy, scr->owner, ScreenContext);
			scr->owner = None;
		}
		if (scr->owner != e->xclient.data.l[2]
		    && (scr->owner = e->xclient.data.l[2])) {
			XSelectInput(dpy, scr->owner, StructureNotifyMask);
			XSaveContext(dpy, scr->owner, ScreenContext, (XPointer) scr);
			XSync(dpy, False);
		}
	}
	if (scr->selwin) {
		XDestroyWindow(dpy, scr->selwin);
		XDeleteContext(dpy, scr->selwin, ScreenContext);
		scr->selwin = None;
	}
	return True;
}

static Bool
destroynotify(XEvent *e)
{
	int n;
	XClientMessageEvent mev;

	if (!e->xany.window || scr->owner != e->xany.window)
		return False;
	XDeleteContext(dpy, scr->owner, ScreenContext);

	XGrabServer(dpy);
	if ((scr->owner = XGetSelectionOwner(dpy, scr->selection))) {
		XSelectInput(dpy, scr->owner, StructureNotifyMask);
		XSaveContext(dpy, scr->owner, ScreenContext, (XPointer) scr);
		XSync(dpy, False);
	}
	XUngrabServer(dpy);

	if (!scr->selwin) {
		scr->selwin = XCreateSimpleWindow(dpy, scr->root,
						  DisplayWidth(dpy, scr->screen),
						  DisplayHeight(dpy, scr->screen),
						  1, 1, 0, 0L, 0L);
		XSaveContext(dpy, scr->selwin, ScreenContext, (XPointer) scr);
	}

	XSetSelectionOwner(dpy, scr->selection, scr->selwin, CurrentTime);
	if (scr->owner) {
		XEvent ev;

		XIfEvent(dpy, &ev, &selectionreleased, (XPointer) scr->owner);
		XDeleteContext(dpy, scr->owner, ScreenContext);
		scr->owner = None;
	}
	mev.display = dpy;
	mev.type = ClientMessage;
	mev.window = scr->root;
	mev.message_type = _XA_MANAGER;
	mev.format = 32;
	mev.data.l[0] = CurrentTime;	/* FIXME: timestamp */
	mev.data.l[1] = scr->selection;
	mev.data.l[2] = scr->selwin;
	mev.data.l[3] = 2;
	mev.data.l[4] = 0;
	XSendEvent(dpy, scr->root, False, StructureNotifyMask, (XEvent *) &mev);
	XSync(dpy, False);
	return True;
}

Bool (*handler[LASTEvent + (EXTRANGE * BaseLast)]) (XEvent *) = {
	/* *INDENT-OFF* */
	[KeyPress]		= IGNOREEVENT,
	[KeyRelease]		= IGNOREEVENT,
	[ButtonPress]		= IGNOREEVENT,
	[ButtonRelease]		= IGNOREEVENT,
	[MotionNotify]		= IGNOREEVENT,
	[EnterNotify]		= IGNOREEVENT,
	[LeaveNotify]		= IGNOREEVENT,
	[FocusIn]		= IGNOREEVENT,
	[FocusOut]		= IGNOREEVENT,
	[KeymapNotify]		= IGNOREEVENT,
	[Expose]		= IGNOREEVENT,
	[GraphicsExpose]	= IGNOREEVENT,
	[NoExpose]		= IGNOREEVENT,
	[VisibilityNotify]	= IGNOREEVENT,
	[CreateNotify]		= IGNOREEVENT,
	[DestroyNotify]		= IGNOREEVENT,
	[UnmapNotify]		= IGNOREEVENT,
	[MapNotify]		= IGNOREEVENT,
	[MapRequest]		= IGNOREEVENT,
	[ReparentNotify]	= IGNOREEVENT,
	[ConfigureNotify]	= IGNOREEVENT,
	[ConfigureRequest]	= IGNOREEVENT,
	[GravityNotify]		= IGNOREEVENT,
	[ResizeRequest]		= IGNOREEVENT,
	[CirculateNotify]	= IGNOREEVENT,
	[CirculateRequest]	= IGNOREEVENT,
	[PropertyNotify]	= IGNOREEVENT,
	[SelectionClear]	= IGNOREEVENT,
	[SelectionRequest]	= IGNOREEVENT,
	[SelectionNotify]	= IGNOREEVENT,
	[ColormapNotify]	= IGNOREEVENT,
	[ClientMessage]		= IGNOREEVENT,
	[MappingNotify]		= IGNOREEVENT,
	/* *INDENT-ON* */
};

Bool
handle_event(XEvent *ev)
{
	if (ev->type <= LASTEvent && handler[ev->type]) {
		geteventscr(ev);
		return (handler[ev->type]) (ev);
	}
	DPRINTF("WARNING: No handler for event type %d\n", ev->type);
	return False;
}

int signum;

void
sighandler(int sig)
{
	if (sig)
		signum = sig;
}

static void
startup(char *previous_id)
{
	char name[32];
	XClientMessageEvent mev;
	unsigned long procs = 0;
	SmcCallbacks cbs;
	char errmsg[256];
	int n;

	_XA_MANAGER = XInternAtom(dpy, "MANAGER", False);

	signal(SIGHUP, sighandler);
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
	signal(SIGQUIT, sighandler);

	for (scr = screens, n = 0; n < nscr; n++, scr++) {
		snprintf(name, 32, "_XDE_STYLE_S%d", scr->screen);
		scr->selection = XInternAtom(dpy, name, False);

		scr->selwin = XCreateSimpleWindow(dpy, scr->root,
						  DisplayWidth(dpy, scr->screen),
						  DisplayHeight(dpy, scr->screen), 1, 1,
						  0, 0L, 0L);
		XSaveContext(dpy, scr->selwin, ScreenContext, (XPointer) scr);

		XGrabServer(dpy);
		if ((scr->owner = XGetSelectionOwner(dpy, scr->selection))) {
			XSelectInput(dpy, scr->owner, StructureNotifyMask);
			XSaveContext(dpy, scr->owner, ScreenContext, (XPointer) scr);
			XSync(dpy, False);
		}
		XUngrabServer(dpy);

		switch (command) {
		case CMD_MODE_RUN:
		default:
			if (scr->owner) {
				fprintf(stderr,
					"another instance of %s already running -- exiting\n",
					NAME);
				exit(0);
			}
			break;
		case CMD_MODE_REPLACE:
			if (scr->owner)
				fprintf(stderr,
					"another instance of %s already running -- replacing\n",
					NAME);
			break;
		case CMD_MODE_QUIT:
			if (scr->owner)
				fprint(stderr,
				       "another instance of %s already running -- quitting\n",
				       NAME);
			break;
		}

		XSetSelectionOwner(dpy, scr->selection, scr->selwin, CurrentTime);
		if (scr->owner != None) {
			XEvent ev;

			XIfEvent(dpy, &ev, &selectionreleased, (XPointer) scr->owner);
			XDeleteContext(dpy, scr->owner, ScreenContext);
			scr->owner = None;
		}
		mev.display = dpy;
		mev.type = ClientMessage;
		mev.window = scr->root;
		mev.message_type = _XA_MANAGER;
		mev.format = 32;
		mev.data.l[0] = CurrentTime;	/* FIXME: timestamp */
		mev.data.l[1] = scr->selection;
		mev.data.l[2] = scr->selwin;
		mev.data.l[3] = 2;
		mev.data.l[4] = 0;
		XSendEvent(dpy, scr->root, False, StructureNotifyMask, (XEvent *) &mev);
		XSync(dpy, False);
	}

	if (command == CMD_MODE_QUIT)
		exit(0);

	/* try to connect to session manager */
	procs =
	    SmcSaveYourselfProcMask | SmcDieProcMask | SmcSaveCompleteProcMask |
	    SmcShutdownCancelledProcMask;
	cbs.save_yourself.callback = xde_save_yourself_cb;
	cbs.save_yourself.client_data = (SmPointer) NULL;
	cbs.die.callback = xde_die_cb;
	cbs.die.client_data = (SmPointer) NULL;
	cbs.save_complete.callback = xde_save_complete;
	cbs.save_complete.client_data = (SmPointer) NULL;
	cbs.shutdown_cancelled.callback = xde_shutdown_cacnelled;
	cbs.shutdown_cancalled.client_data = (SmPointer) NULL;

	smcconn =
	    SmcOpenConnection(NULL, (SmPointer) scr, SmProtoMajor, SmProtoMinor, procs,
			      &cbs, previous_id, &client_id, sizeof(client_id), errmsg);
	if (smcconn == NULL) {
		/* darn, no sesssion manager */
		return;
	}
	iceconn = SmcGetIceConnection(smcconn);

}

static void
event_loop()
{
	int ifd, xfd = 0, num = 1;
	XEvent ev;

	xfd = ConnectionNumber(dpy);
	if (iceconn) {
		ifd = IceConnectionNumber(iceconn);
		num++;
	}
	while (running) {
		struct pollfd pfd[2] = {
			{xfd, POLLIN | POLLERR | POLLHUP, 0}
			{ifd, POLLIN | POLLERR | POLLHUP, 0},
		};

		if (signum) {
			if (signum == SIGHUP) {
				fprintf(stderr, "Exiting on SIGHUP\n");
				running = 0;
				break;
			}
			signum = 0;
		}
		if (poll(pfd, num, -1) == -1) {
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART)
				continue;
			fprintf(stderr, "poll failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else {
			if (pfd[0].revents & (POLLNVAL | POLLHUP | POLLERR)) {
				fprintf(stderr, "poll error on ICE connection\n");
				exit(EXIT_FAILURE);
			}
			if (pfd[1].revents & (POLLNVAL | POLLHUP | POLLERR)) {
				fprintf(stderr, "poll error on X connection\n");
				exit(EXIT_FAILURE);
			}
			if (pfd[0].revents & POLLIN) {
				IceProcessMessages(iceconn, NULL, NULL);
			}
			if (pfd[1].revents & POLLIN) {
				while (XPending(dpy) && running) {
					XNextEvent(dpy, &ev);
					if (!handle_event(&ev))
						fprintf(stderr,
							"WARNING: X Event %d not handled\n",
							ev.type);
				}
			}
		}
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
    %1$s [options]\n\
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
    -y, --system\n\
        set or list system styles\n\
    -u, --user\n\
        set or list user styles\n\
    -S, --screen SCREEN\n\
        only act on screen number SCREEN [default: all(-1)]\n\
    -L, --link\n\
        link style files where possible\n\
    -t, --theme\n\
        only list styles that are also XDE themes\n\
    -w, --wmname WMNAME\n\
        don't detect window manager, use WMNAME\n\
    -f, --rcfile FILE\n\
        assume window manager uses rc file FILE, needs -w\n\
    -r, --reload\n\
        when setting the style, ask wm to reload\n\
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
			{"run",		no_argument,		NULL, 'r'},
			{"replace",	no_argument,		NULL, 'R'},
			{"quit",	no_argument,		NULL, 'q'},
			{"set",		required_argument,	NULL, 's'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},

			{"current",	no_argument,		NULL, 'c'},
			{"list",	no_argument,		NULL, 'l'},
			{"set",		no_argument,		NULL, 's'},
			{"system",	no_argument,		NULL, 'y'},
			{"user",	no_argument,		NULL, 'u'},
			{"screen",	required_argument,	NULL, 'S'},
			{"link",	no_argument,		NULL, 'L'},
			{"theme",	no_argument,		NULL, 't'},
			{"wmname",	required_argument,	NULL, 'w'},
			{"rcfile",	required_argument,	NULL, 'f'},
			{"reload",	no_argument,		NULL, 'r'},

			{"dry-run",	no_argument,		NULL, 'n'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "clsyuS:Ltw:f:rnD::v::hVCH?",
				     long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "clsyuS:Ltw:f:rnDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			if (options.debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;

		case 'c':	/* -c, --current */
			options.set = False;
			options.list = False;
			options.current = True;
			break;
		case 'l':	/* -l, --list */
			options.set = False;
			options.current = False;
			options.list = True;
			break;
		case 's':	/* -s, --set */
			options.list = False;
			options.current = False;
			options.set = True;
			break;
		case 'y':	/* -y, --system */
			options.user = False;
			options.system = True;
			break;
		case 'u':	/* -u, --user */
			options.system = False;
			options.user = True;
			break;
		case 'S':	/* -S, --screen */
			options.screen = atoi(optarg);
			break;
		case 'L':	/* -L, --link */
			options.link = True;
			break;
		case 't':
			options.theme = True;
			break;
		case 'w':	/* -w, --wmname NAME */
			if (!options.wmname)
				options.wmname = strdup(optarg);
			else
				goto bad_option;
			break;
		case 'f':	/* -f, --rcfile FILE */
			if (options.wmname && !options.rcfile)
				options.rcfile = strdup(optarg);
			else
				goto bad_option;
			break;
		case 'r':	/* -r, --reload */
			options.reload = True;
			break;
		case 'n':	/* -n, --dry-run */
			options.dryrun = True;
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
		options.style = strdup(argv[optind++]);
		if (optind < argc)
			goto bad_nonopt;
	}
	xde_init_display();
	if (!xde_detect_wm()) {
		EPRINTF("%s\n", "no detected window manager");
		exit(1);
	}
	if (options.current)
		current_style();
	else if (options.list)
		list_styles();
	else if (options.set)
		set_style();
	else {
		usage(argc, argv);
		exit(2);
	}
	if (options.output > 1)
		xde_show_wms();
	exit(0);
}

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
