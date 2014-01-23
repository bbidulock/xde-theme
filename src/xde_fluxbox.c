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

/* fluxbox(1) plugin */

#include "xde.h"

/** @name FLUXBOX
  */
/** @{ */

/** @brief find the fluxbox init file and default directory.
  *
  * Fluxbox takes a command such as:
  *
  *   fluxbox [-rc RCFILE]
  *
  * When the RCFILE is not specified, ~/.fluxbox/init is used.  The locations of
  * other fluxbox configuration files are specified by the intitial
  * configuration file, but are typically placed under ~/.fluxbox.  System files
  * are placed under /usr/share/fluxbox.
  *
  * System styles are either in /usr/share/fluxbox/styles or in
  * ~/.fluxbox/styles.  Styles under these directories can either be a file or a
  * directory.  When a directory, the actual style file is in a file called
  * theme.cfg.
  */
static void
get_rcfile_FLUXBOX()
{
	char *home = xde_get_proc_environ("HOME") ? : ".";
	char *file = xde_get_rcfile_optarg("-rc");
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
		free(file);
	} else {
		len = strlen(home) + strlen("/.fluxbox/init") + 1;
		wm->rcfile = calloc(len, sizeof(*wm->rcfile));
		strcpy(wm->rcfile, home);
		strcat(wm->rcfile, "/.fluxbox/init");
	}
	xde_get_simple_dirs("fluxbox");
}

/** @brief Find a fluxbox style file from a style name.
  *
  * Fluxbox style files are named files or directories in
  * /usr/share/fluxbox/styles or ~/.fluxbox/styles.  When a named directory,
  * the directory must contain a file named theme.cfg.
  */
static char *
find_style_FLUXBOX()
{
	return xde_find_style_simple("styles", "theme.cfg");
}

/** @brief Get the current fluxbox style.
  *
  * The current fluxbox style is set in the session.styleFile resource in the rc
  * file.
  */
static char *
get_style_FLUXBOX()
{
	XrmValue value;
	char *type, *pos;

	if (wm->style)
		return wm->style;

	get_rcfile_FLUXBOX();
	if (!xde_test_file(wm->rcfile)) {
		EPRINTF("rcfile '%s' does not exist\n", wm->rcfile);
		return NULL;
	}
	xde_init_xrm();
	if (!wm->db && !(wm->db = XrmGetFileDatabase(wm->rcfile))) {
		EPRINTF("cannot read database file '%s'\n", wm->rcfile);
		return NULL;
	}
	if (!XrmGetResource(wm->db, "session.styleFile", "Session.StyleFile",
			    &type, &value)) {
		EPRINTF("%s", "no session.styleFile resource in database\n");
		return NULL;
	}
	free(wm->style);
	wm->style = strndup((char *) value.addr, value.size);
	free(wm->stylename);
	wm->stylename = (pos = strrchr(wm->style, '/')) ?
	    strdup(pos + 1) : strdup(wm->style);
	return wm->style;
}

/** @brief Reload a fluxbox style.
  *
  * Sending SIGUSR2 to the fluxbox PID provided in the _BLACKBOX_PID property on
  * the root window will result in a reconfigure of fluxbox (which is what
  * fluxbox itself does when changing styles); send SIGHUP, a restart.
  *
  */
static void
reload_style_FLUXBOX()
{
	if (wm->pid)
		kill(wm->pid, SIGUSR2);
	else
		EPRINTF("%s", "cannot reload fluxbox without a pid\n");

}

/** @brief Set the fluxbox style.
  *
  * When fluxbox changes the style, it writes the path to the new style in the
  * session.StyleFile resource in the rc file (default ~/.fluxbox/init) and then
  * reloads the configuration.  The session.styleFile entry looks like:
  *
  *   session.styleFile: /usr/share/fluxbox/styles/Airforce
  *
  * Unlike other window managers, it reloads the configuration rather than
  * restarting.  However, fluxbox has the problem that simply reloading the
  * configuration does not result in a change to the menu styles (in particular
  * the font color), so a restart is likely required.
  *
  * Note that when fluxbox restarts, it dow not change the
  * _NET_SUPPORTING_WM_CHECK root window property but it does change the
  * _BLACKBOX_PID root window property, even if it is just to replace it with
  * the same value again.
  */
static void
set_style_FLUXBOX()
{
	char *stylefile, *line, *style;
	int len;

	get_rcfile_FLUXBOX();
	if (!wm->pid) {
		EPRINTF("%s", "cannot set fluxbox style without pid\n");
		goto no_pid;
	}
	if (!xde_test_file(wm->rcfile)) {
		EPRINTF("rcfile '%s' does not exist\n", wm->rcfile);
		goto no_rcfile;
	}
	if (!(stylefile = find_style_FLUXBOX())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		goto no_stylefile;
	}
	if ((style = get_style_FLUXBOX()) && !strcmp(style, stylefile))
		goto no_change;
	xde_init_xrm();
	if (!wm->db && !(wm->db = XrmGetFileDatabase(wm->rcfile))) {
		EPRINTF("cannot read database file '%s'\n", wm->rcfile);
		goto no_db;
	}
	len = strlen(stylefile) + strlen("session.styleFile:\t\t") + 1;
	line = calloc(len, sizeof(*line));
	snprintf(line, len, "session.styleFile:\t\t%s", stylefile);
	XrmPutLineResource(&wm->db, line);
	free(line);
	if (options.dryrun) {
		OPRINTF("would write database to %s as follows:\n", wm->rcfile);
		XrmPutFileDatabase(wm->db, "/dev/stderr");
		if (options.reload)
			OPRINTF("%s", "would reload window manager\n");
	} else {
		XrmPutFileDatabase(wm->db, wm->rcfile);
		if (options.reload)
			reload_style_FLUXBOX();
	}
      no_change:
	if (wm->db) {
		XrmDestroyDatabase(wm->db);
		wm->db = NULL;
	}
      no_db:
	free(stylefile);
      no_stylefile:
      no_rcfile:
      no_pid:
	return;
}

static void
list_dir_FLUXBOX(char *xdir, char *style)
{
	return xde_list_dir_simple(xdir, "styles", "theme.cfg", style);
}

/** @brief List fluxbox styles.
  */
static void
list_styles_FLUXBOX()
{
	return xde_list_styles_simple();
}

WmOperations xde_wm_ops = {
	"fluxbox",
	&get_rcfile_FLUXBOX,
	&find_style_FLUXBOX,
	&get_style_FLUXBOX,
	&set_style_FLUXBOX,
	&reload_style_FLUXBOX,
	&list_dir_FLUXBOX,
	&list_styles_FLUXBOX
};

/** @} */


