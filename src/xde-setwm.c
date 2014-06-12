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

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

const char *program = NAME;

int output = 1;
int debug = 0;

static void
copying(int argc, char *argv[])
{
	if (!output && !debug)
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
	if (!output && !debug)
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
	if (!output && !debug)
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
	if (!output && !debug)
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
    -N, --name NAME\n\
        set the window manager name to NAME\n\
    -K, --class CLASS\n\
        set the window manager class to CLASS\n\
    -p, --pid PID\n\
        set the window manager pid to PID\n\
    -c, --command PROGRAM [ARG [ARG ...]]\n\
        set the window manager command to PROGRAM and ARGS\n\
    -S, --screen SCREEN\n\
        only act on screen number SCREEN [default: all(-1)]\n\
    -D, --debug [LEVEL]\n\
        increment or set debug LEVEL [default: 0]\n\
    -v, --verbose [LEVEL]\n\
        increment or set output verbosity LEVEL [default: 1]\n\
        this option may be repeated.\n\
", argv[0]);
}

Display *dpy;

int
handler(Display *display, XErrorEvent *xev)
{
	if (debug) {
		char msg[80], req[80], num[80], def[80];

		snprintf(num, sizeof(num), "%d", xev->request_code);
		snprintf(def, sizeof(def), "[request_code=%d]", xev->request_code);
		XGetErrorDatabaseText(dpy, "xdg-setwm", num, def, req, sizeof(req));
		if (XGetErrorText(dpy, xev->error_code, msg, sizeof(msg)) != Success)
			msg[0] = '\0';
		fprintf(stderr, "X error %s(0x%lx): %s\n", req, xev->resourceid, msg);
	}
	return (0);
}

int
main(int argc, char *argv[])
{
	char *name = NULL;
	char *klass = NULL;
	char *revision = NULL;
	pid_t pid = 0;
	int screen = -1;
	Window root;
	XTextProperty xtp = { NULL, };
	char *p;
	char *hostp;
	char **cmdv = NULL;
	int cmdc = 0;

	while (1) {
		int c, val;

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"name",	required_argument,	NULL, 'N'},
			{"class",	required_argument,	NULL, 'K'},
			{"pid",		required_argument,	NULL, 'p'},
			{"revision",	required_argument,	NULL, 'r'},
			{"command",	required_argument,	NULL, 'c'},
			{"screen",	required_argument,	NULL, 'S'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "N:K:p:r:c:S:nD::v::hVCH?",
				     long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "N:K:p:r:c:S:nDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;
		case 'N':	/* -N, --name NAME */
			free(name);
			name = strdup(optarg);
			break;
		case 'K':	/* -K, --class CLASS */
			free(klass);
			klass = strdup(optarg);
			break;
		case 'p':	/* -p, --pid PID */
			pid = atoi(optarg);
			break;
		case 'r':	/* -r, --revision VERSION */
			revision = strdup(optarg);
			break;
		case 'c':	/* -c, --command PROGRAM [ARG [ARG ...]] */
			cmdv = &argv[optind - 1];
			cmdc = argc - optind + 1;
			break;
		case 'S':	/* -S, --screen SCREEN */
			screen = atoi(optarg);
			break;
		case 'D':	/* -d, --debug [LEVEL] */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n",
					argv[0]);
			if (optarg == NULL) {
				debug++;
			} else {
				if ((val = strtol(optarg, NULL, 0)) < 0)
					goto bad_option;
				debug = val;
			}
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n",
					argv[0]);
			if (optarg == NULL) {
				output++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
		case 'H':	/* -H, --? */
			if (debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (debug)
				fprintf(stderr, "%s: printing version message\n",
					argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			if (debug)
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
			if (output || debug) {
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
		if (cmdv)
			break;
	}

	if (!(dpy = XOpenDisplay(0))) {
		fprintf(stderr, "%s\n", "cannot open display");
		exit(127);
	}
	XSetErrorHandler(handler);
	if (screen == -1)
		screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	if ((hostp = calloc(64, sizeof(*hostp)))) {
		gethostname(hostp, 64);
		if (!*hostp) {
			free(hostp);
			hostp = NULL;
		}
	}
	if (!name && cmdv) {
		name = strdup(cmdv[0]);
		if ((p = strrchr(name, '/')))
			memmove(name, p + 1, strlen(p + 1) + 1);
		for (p = name; *p; p++)
			*p = tolower(*p);
	}
	if (name && !klass) {
		klass = strdup(name);
		klass[0] = toupper(klass[0]);
	}
	if (klass && !name) {
		name = strdup(klass);
		for (p = name; *p; p++)
			*p = tolower(*p);
	}
	if (name) {
		char *namever;
		char *res[3] = { NULL, };

		namever = calloc(256, sizeof(*namever));
		strcpy(namever, name);
		if (revision) {
			strcat(namever, " ");
			strcat(namever, revision);
		}
		XmbTextListToTextProperty(dpy, &namever, 1, XUTF8StringStyle, &xtp);
		XSetTextProperty(dpy, root, &xtp,
				 XInternAtom(dpy, "_NET_WM_NAME", False));
		free(namever);
		namever = NULL;
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));
		XmbTextListToTextProperty(dpy, &name, 1, XStdICCTextStyle, &xtp);
		XSetTextProperty(dpy, root, &xtp, XInternAtom(dpy, "WM_NAME", False));
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));
		XmbTextListToTextProperty(dpy, &name, 1, XUTF8StringStyle, &xtp);
		XSetTextProperty(dpy, root, &xtp,
				 XInternAtom(dpy, "_XDE_WM_NAME", False));
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));
		res[0] = name;
		res[1] = klass;
		res[2] = NULL;
		XmbTextListToTextProperty(dpy, res, 2, XStdICCTextStyle, &xtp);
		if (xtp.value)
			xtp.nitems++;
		XSetTextProperty(dpy, root, &xtp, XInternAtom(dpy, "WM_CLASS", False));
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));
		XmbTextListToTextProperty(dpy, res, 2, XUTF8StringStyle, &xtp);
		if (xtp.value)
			xtp.nitems++;
		XSetTextProperty(dpy, root, &xtp,
				 XInternAtom(dpy, "_XDE_WM_CLASS", False));
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));
	} else {
		XDeleteProperty(dpy, root, XInternAtom(dpy, "_NET_WM_NAME", False));
		XDeleteProperty(dpy, root, XInternAtom(dpy, "WM_NAME", False));
		XDeleteProperty(dpy, root, XInternAtom(dpy, "_XDE_WM_NAME", False));
		XDeleteProperty(dpy, root, XInternAtom(dpy, "WM_CLASS", False));
		XDeleteProperty(dpy, root, XInternAtom(dpy, "_XDE_WM_CLASS", False));
	}
	if (revision) {
		XmbTextListToTextProperty(dpy, &revision, 1, XUTF8StringStyle, &xtp);
		XSetTextProperty(dpy, root, &xtp,
				 XInternAtom(dpy, "_XDE_WM_VERSION", False));
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));
	} else {
		XDeleteProperty(dpy, root, XInternAtom(dpy, "_XDE_WM_VERSION", False));
	}
	if (pid) {
		unsigned long long_pid = pid;

		XChangeProperty(dpy, root, XInternAtom(dpy, "_NET_WM_PID", False),
				XA_CARDINAL, 32, PropModeReplace,
				(unsigned char *) &long_pid, 1);
		XChangeProperty(dpy, root, XInternAtom(dpy, "_XDE_WM_PID", False),
				XA_CARDINAL, 32, PropModeReplace,
				(unsigned char *) &long_pid, 1);
	} else {
		XDeleteProperty(dpy, root, XInternAtom(dpy, "_NET_WM_PID", False));
		XDeleteProperty(dpy, root, XInternAtom(dpy, "_XDE_WM_PID", False));
	}
	if (hostp) {
		XmbTextListToTextProperty(dpy, &hostp, 1, XStdICCTextStyle, &xtp);
		XSetTextProperty(dpy, root, &xtp,
				 XInternAtom(dpy, "WM_CLIENT_MACHINE", False));
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));
		XmbTextListToTextProperty(dpy, &hostp, 1, XUTF8StringStyle, &xtp);
		XSetTextProperty(dpy, root, &xtp,
				 XInternAtom(dpy, "_XDE_WM_HOSTNAME", False));
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));

	} else {
		XDeleteProperty(dpy, root, XInternAtom(dpy, "WM_CLIENT_MACHINE", False));
		XDeleteProperty(dpy, root, XInternAtom(dpy, "_XDE_WM_HOSTNAME", False));
	}
	if (cmdv) {
		XmbTextListToTextProperty(dpy, cmdv, cmdc, XStdICCTextStyle, &xtp);
		if (xtp.value)
			xtp.nitems++;
		XSetTextProperty(dpy, root, &xtp, XInternAtom(dpy, "WM_COMMAND", False));
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));
		XmbTextListToTextProperty(dpy, cmdv, cmdc, XUTF8StringStyle, &xtp);
		if (xtp.value)
			xtp.nitems++;
		XSetTextProperty(dpy, root, &xtp,
				 XInternAtom(dpy, "_XDE_WM_COMMAND", False));
		if (xtp.value) {
			XFree(xtp.value);
			xtp.value = NULL;
		}
		memset(&xtp, 0, sizeof(xtp));
	} else {
		XDeleteProperty(dpy, root, XInternAtom(dpy, "WM_COMMAND", False));
		XDeleteProperty(dpy, root, XInternAtom(dpy, "_XDE_WM_COMMAND", False));
	}
	XSync(dpy, False);
	XCloseDisplay(dpy);
	exit(0);
}
