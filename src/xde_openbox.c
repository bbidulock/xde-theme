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

/* openbox(1) plugin */

#include "xde.h"

/** @name OPENBOX
  */
/** @{ */

/** @brief Find the openbox rc file and default directory.
  *
  * Openbox takes a command such as:
  *
  *   openbox [--config-file RCFILE]
  *
  * When RCFILE is not specified, $XDG_CONFIG_HOME/openbox/rc.xml is used.  The
  * locations of other openbox configuration files are specified by the initial
  * configuration file, but are typically placed under ~/.config/openbox.
  * System files are placed under /usr/share/openbox.
  */
static void
get_rcfile_OPENBOX()
{
	char *home = xde_get_proc_environ("HOME") ? : ".";
	char *file = xde_get_rcfile_optarg("--config-file");
	char *cnfg = xde_get_proc_environ("XDG_CONFIG_HOME");
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
		if (cnfg) {
			len = strlen(cnfg) + strlen("/openbox/rc.xml") + 1;
			wm->rcfile = calloc(len, sizeof(*wm->rcfile));
			strcpy(wm->rcfile, cnfg);
		} else {
			len = strlen(home) + strlen("/.config") +
			    strlen("/openbox/rc.xml") + 1;
			wm->rcfile = calloc(len, sizeof(*wm->rcfile));
			strcpy(wm->rcfile, home);
			strcat(wm->rcfile, "/.config");
		}
		strcat(wm->rcfile, "/openbox/rc.xml");
	}
	free(wm->pdir);
	wm->pdir = strdup(wm->rcfile);
	if (strrchr(wm->pdir, '/'))
		*strrchr(wm->pdir, '/') = '\0';
	free(wm->udir);
	if (cnfg) {
		len = strlen(cnfg) + strlen("/openbox") + 1;
		wm->udir = calloc(len, sizeof(*wm->udir));
		strcpy(wm->udir, cnfg);
		strcat(wm->udir, "/openbox");
	} else {
		len = strlen(home) + strlen("/.config/openbox") + 1;
		wm->udir = calloc(len, sizeof(*wm->udir));
		strcpy(wm->udir, home);
		strcat(wm->udir, "/.config/openbox");
	}
	free(wm->sdir);
	wm->sdir = strdup("/usr/share/openbox");
	free(wm->edir);
	wm->edir = strdup("/etc/xdg/openbox");

	return;
}

/** @brief Find an openbox style file.
  *
  * Openbox sytle file are XDG organized: that is, the searched directories are
  * $XDG_DATA_HOME:$XDG_DATA_DIRS with appropriate defaults.  Subdirectories
  * under the themes directory (e.g. /usr/share/themes/STYLENAME) that contain
  * an openbox-3 subdirectory containing a themerc file.
  *
  * Because openbox uses the XDG scheme, it does not distinguish between system
  * and user styles.
  */
static char *
find_style_OPENBOX()
{
	char *dirs, *path, *file;
	char *pos, *end;

	if (strchr(options.style, '/')) {
		EPRINTF("path in openbox style name '%s'\n", wm->style);
		return NULL;
	}

	get_rcfile_OPENBOX();

	dirs = calloc(PATH_MAX, sizeof(*dirs));
	path = calloc(PATH_MAX, sizeof(*path));
	file = calloc(PATH_MAX, sizeof(*path));

	strcpy(file, "/themes/");
	strcat(file, wm->style);
	strcat(file, "/openbox-3/themerc");

	if (getenv("XDG_DATA_HOME"))
		strcat(dirs, getenv("XDG_DATA_HOME"));
	else {
		strcat(dirs, getenv("HOME") ? : ".");
		strcat(dirs, "/.local/share");
	}
	strcat(dirs, ":");
	if (getenv("XDG_DATA_DIRS"))
		strcat(dirs, getenv("XDG_DATA_DIRS"));
	else
		strcat(dirs, "/usr/local/share:/usr/share");
	for (pos = dirs, end = pos + strlen(dirs); pos < end;
	     pos = strchrnul(pos, ':'), pos[0] = '\0', pos++) ;
	for (pos = dirs; pos < end; pos += strlen(pos) + 1) {
		strcpy(path, pos);
		strcat(path, file);
		if (xde_test_file(path))
			goto got_it;
	}
	free(path);
	free(dirs);
	free(file);
	EPRINTF("could not find path for style '%s'\n", wm->style);
	return NULL;
      got_it:
	free(dirs);
	free(file);
	return path;

}

static char *
get_menu_OPENBOX()
{
	get_rcfile_OPENBOX();
	return NULL;
}

static char *
get_style_OPENBOX()
{
	get_rcfile_OPENBOX();
	return NULL;
}

#define OB_CONTROL_RECONFIGURE	    1	/* reconfigure */
#define OB_CONTROL_RESTART	    2	/* restart */
#define OB_CONTROL_EXIT		    3	/* exit */

/** @brief Reload an openbox style.
  *
  * Openbox can be reconfigured by sending an _OB_CONTROL message to the root
  * window with a control type in data.l[0].  The control type can be one of:
  *
  * OB_CONTROL_RECONFIGURE    1   reconfigure
  * OB_CONTROL_RESTART        2   restart
  * OB_CONTROL_EXIT           3   exit
  *
  */
static void
reload_style_OPENBOX()
{
	XEvent ev;

	ev.xclient.type = ClientMessage;
	ev.xclient.display = dpy;
	ev.xclient.window = root;
	ev.xclient.message_type = XInternAtom(dpy, "_OB_CONTROL", False);
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = OB_CONTROL_RECONFIGURE;
	ev.xclient.data.l[1] = 0;
	ev.xclient.data.l[2] = 0;
	ev.xclient.data.l[3] = 0;
	ev.xclient.data.l[4] = 0;
	XSendEvent(dpy, root, False,
		   SubstructureNotifyMask | SubstructureRedirectMask, &ev);
	XFlush(dpy);
}

/** @brief Set the openbox style.
  *
  * When openbox changes its theme, it changes the _OB_THEME property on the root
  * window.  openbox also changes the theme section in ~/.config/openbox/rc.xml and
  * writes the file and performs a reconfigure.
  *
  * openbox sets the _OB_CONFIG_FILE property on the root window when the
  * configuration file differs from the default (but not otherwise).
  *
  * openbox does not provide internal actions for setting the theme: it uses an
  * external theme setting program that communicates with the window manager.
  *
  * When xde-session runs, it sets the OPENBOX_RCFILE environment variable.
  * xde-session and associated tools will always launch openbox with a command such
  * as:
  *
  *   openbox ${OPENBOX_RCFILE:+--config-file $OPENBOX_RCFILE}
  *
  * The default configuration file when OPENBOX_RCFILE is not specified is
  * $XDG_CONFIG_HOME/openbox/rc.xml.  The location of other openbox configuration
  * files are specified by the initial configuration file.  xde-session typically sets
  * OPENBOX_RCFILE to $XDG_CONFIG_HOME/openbox/xde-rc.xml.
  */
static void
set_style_OPENBOX()
{
	char *stylefile;

	if (!(stylefile = find_style_OPENBOX())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		return;
	}

	if (options.dryrun) {
	} else {
		if (options.reload)
			reload_style_OPENBOX();
	}
	return;
}

static void
list_dir_OPENBOX(char *xdir, char *style)
{
	return xde_list_dir_simple(xdir, "themes", "/openbox-3/themerc", "", style);
}

static void
list_styles_OPENBOX()
{
	char **dir, *style = get_style_OPENBOX();

	xde_get_xdg_dirs();

	for (dir = wm->xdg_dirs; *dir; dir++)
		list_dir_OPENBOX(*dir, style);
}

WmOperations xde_wm_ops = {
	"openbox",
	&get_rcfile_OPENBOX,
	&find_style_OPENBOX,
	&get_style_OPENBOX,
	&set_style_OPENBOX,
	&reload_style_OPENBOX,
	&list_dir_OPENBOX,
	&list_styles_OPENBOX,
	&get_menu_OPENBOX
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
