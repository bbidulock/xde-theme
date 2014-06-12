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

/** @brief Get the current style of the window manager.
  */
static void
current_style()
{
	WmOperations *ops;

	OPRINTF("%s\n", "getting current style");

	if (options.screen == -1) {
		for (screen = 0; screen < nscr; screen++) {
			OPRINTF("getting current style for screen %d\n", screen);
			scr = screens + screen;
			root = scr->root;
			if (!(wm = scr->wm) || !(ops = wm->ops) || !ops->get_style) {
				EPRINTF("cannot get current style for screen %d\n",
					screen);
				continue;
			}
			ops->get_style();
			if (wm->style
			    && (!options.theme || xde_find_theme(wm->stylename, NULL))) {
				fprintf(stdout, "%s %s\n", wm->stylename, wm->style);
			} else if (options.theme && xde_get_theme()) {
				fprintf(stdout, "%s %s\n", scr->theme, scr->themefile);
			} else if (options.theme) {
				EPRINTF("cannot get current theme for screen %d\n",
					screen);
			} else {
				EPRINTF("cannot get current style for screen %d\n",
					screen);
			}
		}
	} else if (0 <= options.screen && options.screen < nscr) {
		screen = options.screen;
		OPRINTF("getting current style for screen %d\n", screen);
		scr = screens + screen;
		root = scr->root;
		if (!(wm = scr->wm) || !(ops = wm->ops) || !ops->get_style) {
			EPRINTF("cannot get current style for screen %d\n", screen);
			return;
		}
		ops->get_style();
		if (wm->style && (!options.theme || xde_find_theme(wm->stylename, NULL))) {
			fprintf(stdout, "%s %s\n", wm->stylename, wm->style);
		} else if (options.theme && xde_get_theme()) {
			fprintf(stdout, "%s %s\n", scr->theme, scr->themefile);
		} else if (options.theme) {
			EPRINTF("cannot get current theme for screen %d\n", screen);
		} else {
			EPRINTF("cannot get current style for screen %d\n", screen);
		}
	} else {
		EPRINTF("invalid screen number %d\n", options.screen);
		exit(2);
	}
}

/** @brief Generate a style submenu for the window manager
  */
static void
gen_menu()
{
	WmOperations *ops;

	OPRINTF("%s\n", "generating style menu");

	if (options.screen == -1) {
		for (screen = 0; screen < nscr; screen++) {
			scr = screens + screen;
			root = scr->root;
			if (!(wm = scr->wm) || !(ops = wm->ops) || !ops->gen_menu)
				continue;
			ops->gen_menu();
		}
	} else if (0 <= options.screen && options.screen < nscr) {
		screen = options.screen;
		scr = screens + screen;
		root = scr->root;
		if (!(wm = scr->wm) || !(ops = wm->ops) || !ops->gen_menu) {
			EPRINTF("cannot generate menu for screen %d\n", screen);
			return;
		}
		ops->gen_menu();
	} else {
		EPRINTF("invalid screen number %d\n", options.screen);
		exit(2);
	}
}

/** @brief List the style of the window manager.
  */
static void
list_styles()
{
	WmOperations *ops;

	OPRINTF("%s\n", "listing styles");

	if (options.screen == -1) {
		for (screen = 0; screen < nscr; screen++) {
			scr = screens + screen;
			root = scr->root;
			if (!(wm = scr->wm) || !(ops = wm->ops) || !ops->list_styles)
				continue;
			ops->list_styles();
		}
	} else if (0 <= options.screen && options.screen < nscr) {
		screen = options.screen;
		scr = screens + screen;
		root = scr->root;
		if (!(wm = scr->wm) || !(ops = wm->ops) || !ops->list_styles) {
			EPRINTF("cannot list styles for screen %d\n", screen);
			return;
		}
		ops->list_styles();
	} else {
		EPRINTF("invalid screen number %d\n", options.screen);
		exit(2);
	}
}

/** @brief Set the style of the window manager.
  */
static void
set_style()
{
	WmOperations *ops;

	OPRINTF("%s\n", "setting style");

	if (options.screen == -1) {
		for (screen = 0; screen < nscr; screen++) {
			scr = screens + screen;
			root = scr->root;
			if (!(wm = scr->wm))
				continue;
			if (options.theme)
				xde_set_theme(options.style);
			if (!(ops = wm->ops) || !ops->set_style)
				continue;
			ops->set_style();
		}
	} else if (0 <= options.screen && options.screen < nscr) {
		screen = options.screen;
		scr = screens + screen;
		root = scr->root;
		if (!(wm = scr->wm))
			return;
		if (options.theme)
			xde_set_theme(options.style);
		if (!(ops = wm->ops) || !ops->set_style) {
			EPRINTF("cannot set style for screen %d\n", screen);
			return;
		}
		ops->set_style();
	} else {
		EPRINTF("invalid screen number %d\n", options.screen);
		exit(2);
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
    %1$s {-s|--set}  [options]  STYLE\n\
    %1$s {-m|--menu} [options]\n\
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
    %1$s [{-c|--current}] [options]\n\
    %1$s {-l|--list} [options] [STYLE]\n\
    %1$s {-s|--set}  [options]  STYLE\n\
    %1$s {-m|--menu} [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    STYLE\n\
        style name or full path to style file\n\
        name of the style to list or set\n\
Command options:\n\
    -c, --current\n\
        list current style [default when no other command specified]\n\
    -l, --list [STYLE]\n\
        list styles\n\
    -s, --set STYLE\n\
        set the style\n\
    -m, --menu\n\
        generate a style or theme menu\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
Options:\n\
    -e, --shell\n\
        output suitable for shell evaluation or source [default: human]\n\
    -p, --perl\n\
        output suitable for perl evaluation [default: human]\n\
    -y, --system\n\
        set or list system styles\n\
    -u, --user\n\
        set or list user styles\n\
    -S, --screen SCREEN\n\
        only act on screen number SCREEN [default: all(-1)]\n\
    -L, --link\n\
        link style files where possible\n\
    -t, --theme\n\
        only list styles that are also XDE themes, set or get XDE theme\n\
	name instead of style name\n\
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
	options.format = XDE_OUTPUT_HUMAN;

	while (1) {
		int c, val;

#ifdef _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"current",	no_argument,		NULL, 'c'},
			{"list",	no_argument,		NULL, 'l'},
			{"set",		no_argument,		NULL, 's'},
			{"menu",	no_argument,		NULL, 'm'},
			{"system",	no_argument,		NULL, 'y'},
			{"user",	no_argument,		NULL, 'u'},
			{"screen",	required_argument,	NULL, 'S'},
			{"link",	no_argument,		NULL, 'L'},
			{"theme",	no_argument,		NULL, 't'},
			{"wmname",	required_argument,	NULL, 'w'},
			{"rcfile",	required_argument,	NULL, 'f'},
			{"reload",	no_argument,		NULL, 'r'},
			{"shell",	no_argument,		NULL, 'e'},
			{"perl",	no_argument,		NULL, 'p'},

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

		c = getopt_long_only(argc, argv, "clsmyuS:Ltw:f:repnD::v::hVCH?",
				     long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "clsmyuS:Ltw:f:repnDvhVC?");
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
			options.menu = False;
			options.current = True;
			break;
		case 'm':	/* -m, --menu */
			options.set = False;
			options.list = False;
			options.current = False;
			options.menu = True;
			break;
		case 'l':	/* -l, --list */
			options.set = False;
			options.current = False;
			options.menu = False;
			options.list = True;
			break;
		case 's':	/* -s, --set */
			options.list = False;
			options.menu = False;
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
		case 't':	/* -t, --theme */
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
		case 'e':	/* -e, --shell */
			options.format = XDE_OUTPUT_SHELL;
			break;
		case 'p':	/* -p, --perl */
			options.format = XDE_OUTPUT_PERL;
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
	else if (options.menu)
		gen_menu();
	else {
		usage(argc, argv);
		exit(2);
	}
	if (options.output > 1)
		xde_show_wms();
	exit(0);
}

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
