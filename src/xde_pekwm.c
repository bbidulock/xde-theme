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

/** @name PEKWM
  */
/** @{ */

static void
get_rcfile_PEKWM()
{
	return xde_get_rcfile_simple("pekwm",  ".pekwm/config", "--config");
}

static char *
find_style_PEKWM()
{
	return xde_find_style_simple("themes", "/theme", "");
}

static char *
from_file_PEKWM(char *fname, char *field)
{
	FILE *f;
	char *buf, *b, *e, *result = NULL;

	if (!(f = fopen(fname, "r"))) {
		DPRINTF("%s: %s\n", fname, strerror(errno));
		return NULL;
	}
	buf = calloc(PATH_MAX + 1, sizeof(*buf));
	while (fgets(buf, PATH_MAX, f)) {
		b = buf;
		b += strspn(b, " \t");
		if (*b == '#' || *b == '\n')
			continue;
		if (!(b = strstr(b, field)))
			continue;
		b += strlen(field);
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
		result = buf;
		break;
	}
	fclose(f);
	if (result) {
		/* watch out for tilde expansion */
		if (*result == '~') {
			char *home = xde_get_proc_environ("HOME") ? : ".";

			memmove(result + strlen(home), result + 1, strlen(result + 1) + 1);
			memcpy(result, home, strlen(home));
		}
	} else
		free(buf);
	return result;
}

/** @brief Get the pekwm menu.
  *
  * pekwm places its menu specification in its primary configuration file (e.g.
  * ~/.pekwm/config) in a section that looks like:
  *
  * Files {
  *	Menu = "~/.pekwm/menu"
  * }
  *
  * Note that the path is to a file.  Also, if this is not present in the
  * primary configuration file (e.g. ~/.pekwm/config), the default from the
  * system configuration file (normally /etc/pekwm/config_system) is used, which
  * defaults to "~/.pekwm/menu".
  *
  * This function pretty much assumes that the standard pekwm(1) configuration
  * is in place.
  */
static char *
get_menu_PEKWM()
{
	char *menufile = NULL;

	get_rcfile_PEKWM();
	if (wm->edir && wm->edir[0]) {
		int len;
		char *file;

		len = strlen(wm->edir) + strlen("/config_system") + 1;
		file = calloc(len, sizeof(*file));
		strcpy(file, wm->edir);
		strcat(file, "/config_system");
		menufile = from_file_PEKWM(file, "Menu");
		free(file);
	}
	if (wm->rcfile) {
		char *tmp;

		if ((tmp = from_file_PEKWM(wm->rcfile, "Menu"))) {
			free(menufile);
			menufile = tmp;
		}
	}
	if (menufile) {
		free(wm->menu);
		/* menufile is too big (PATH_MAX) */
		wm->menu = strdup(menufile);
		free(menufile);
	}
	return wm->menu;
}

/** @brief Get the pekwm style.
  *
  * pekwm places its theme specification in its primary configuration file (e.g.
  * ~/.pekwm/config) in a section that looks like:
  *
  * Files {
  *     Theme = "/usr/share/pekwm/themes/Airforce"
  * }
  *
  * Note that is a path to a directory and not a file.  Also, if this is not
  * present in the primary configuration file (e.g. ~/.pekwm/config), the
  * default from the system configuration file (normally
  * /etc/pekwm/config_system) is used.  That value is normally
  * "$_PEKWM_THEME_PATH/default" where, _PEKWM_THEME_PATH translates to
  * /usr/share/pekwm/themes.
  *
  * This function pretty much assumes that the standard pekwm(1) configuration
  * is in place.
  */
static char *
get_style_PEKWM()
{
	char *stylefile = NULL;

	get_rcfile_PEKWM();
	if (wm->rcfile)
		stylefile = from_file_PEKWM(wm->rcfile, "Theme");
	if (stylefile) {
		char *pos;

		free(wm->style);
		wm->style = strdup(stylefile);
		free(wm->stylename);
		/* trim off path */
		wm->stylename = (pos = strrchr(stylefile, '/')) ?
		    strdup(pos + 1) : strdup(stylefile);
		free(stylefile);
	}
	return wm->style;
}

/** @brief Reload pekwm style.
  *
  * pekwm can be restarted by sending a SIGHUP signal to the pekwm process.
  * pekwm sets its pid in the _NET_WM_PID(CARDINAL) property on the root window
  * (not the check window) as well as the fqdn of the host in the
  * WM_CLIENT_MACHINE(STRING) property, again on the root window.  The XDE::EWMH
  * module figures this out.
  */
static void
reload_style_PEKWM()
{
	if (wm->pid)
		kill(wm->pid, SIGHUP);
	else
		EPRINTF("%s", "cannot reload pewkm without a pid\n");
}

/** @brief Set a pekwm style.
  *
  * When pekwm changes its style, it places the theme directory in the
  * ~/.pekwm/config file.  This normally has the form:
  *
  *   Files {
  *       Theme = "/usr/share/pekwm/themes/Airforce"
  *   }
  *
  * The last component of the path is the theme name.  The full path is to a
  * directory which contains a F<theme> file.  System styles are located in
  * /usr/share/pekwm/themes; user styles are located in ~/.pekwm/themes.
  *
  * When xde-session runs, it sets the PEKWM_RCFILE environment variable.
  * xde-session and associated tools always launch pekwm with a command such as:
  *
  *   pekwm ${PEKWM_RCFILE:+--config $PEKWM_RCFILE}
  *
  * The default configuration file when PEKWM_RCFILE is not specified is
  * ~/.pekwm/config.  The locations of other pekwm(1) configuration files are
  * specified in the initial configuration file.  xde-session typically sets
  * PEKWM_RCFILE to $XDG_CONFIG_HOME/pekwm/config.
  */
static void
set_style_PEKWM()
{
	FILE *f;
	struct stat st;
	char *stylefile, *buf, *pos, *end, *line, *p, *q;
	int len;
	size_t read, total;

	if (!wm->pid) {
		EPRINTF("%s", "cannot set pekwm style without pid\n");
		goto no_stylefile;
	}
	if (!(stylefile = find_style_PEKWM())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		goto no_stylefile;
	}
	if (!(f = fopen(wm->rcfile, "r"))) {
		EPRINTF("%s: %s\n", wm->rcfile, strerror(errno));
		goto no_rcfile;
	}
	if (fstat(fileno(f), &st)) {
		EPRINTF("%s: %s\n", wm->rcfile, strerror(errno));
		goto no_stat;
	}
	buf = calloc(st.st_size + 1, sizeof(*buf));
	/* read entire file into buffer */
	for (total = 0; total < st.st_size; total += read)
		if ((read = fread(buf + total, 1, st.st_size - total, f)))
			if (total < st.st_size)
				goto no_buf;
	len = strlen(stylefile) + strlen("Theme = \"\"") + 1;
	line = calloc(len, sizeof(*line));
	snprintf(line, len, "Theme = \"%s\"", stylefile);
	if (strstr(buf, line))
		goto no_change;
	if (options.dryrun) {
	} else {
		if (!(f = freopen(wm->rcfile, "w", f))) {
			EPRINTF("%s: %s\n", wm->rcfile, strerror(errno));
			goto no_change;
		}
		for (pos = buf, end = buf + st.st_size; pos < end; pos = pos + strlen(pos) + 1) {
			*strchrnul(pos, '\n') = '\0';
			if ((p = strstr(pos, "Theme = ")) && (!(q = strchr(pos, '#')) || p < q))
				fprintf(f, "    %s\n", line);
			else
				fprintf(f, "%s\n", pos);
		}
		if (options.reload)
			reload_style_PEKWM();
	}
      no_change:
	free(line);
      no_buf:
	free(buf);
      no_stat:
	fclose(f);
      no_rcfile:
	free(stylefile);
      no_stylefile:
	return;
}

static void
list_dir_PEKWM(char *xdir, char *style)
{
	return xde_list_dir_simple(xdir, "themes", "/theme", "", style);
}

static void
list_styles_PEKWM()
{
	return xde_list_styles_simple();
}

WmOperations xde_wm_ops = {
	"pekwm",
	&get_rcfile_PEKWM,
	&find_style_PEKWM,
	&get_style_PEKWM,
	&set_style_PEKWM,
	&reload_style_PEKWM,
	&list_dir_PEKWM,
	&list_styles_PEKWM,
	&get_menu_PEKWM
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
