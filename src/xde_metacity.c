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
#include <glib-object.h>
#include <gio/gio.h>

/** @name METACITY
  */
/** @{ */

static void
get_rcfile_METACITY()
{
}

/** @brief Locate a metacity theme file.
  *
  * Metacity theme files are XDG organized: that is, the searched directories
  * are $XDG_DATA_HOME:$XDG_DATA_DIRS with appropriate defaults.  Subdirectories
  * under the themes directory (e.g. /usr/share/themes/STYLENAME) that contain
  * a metacity-1 subdirectory containing a metacity-theme-[12].xml file.
  *
  * Because metacity uses the XDG scheme, it does not distinguish between system
  * and user styles.
  */
static char *
locate_theme_METACITY(char *theme)
{
	char *dirs, *path, *file1, *file2;
	char *pos, *end;

	char *xdgh = xde_get_proc_environ("XDG_DATA_HOME");
	char *xdgd = xde_get_proc_environ("XDG_DATA_DIRS");
	char *home = xde_get_proc_environ("HOME") ? : ".";

	dirs = calloc(PATH_MAX, sizeof(*dirs));
	path = calloc(PATH_MAX, sizeof(*path));
	file1 = calloc(PATH_MAX, sizeof(*file1));
	file2 = calloc(PATH_MAX, sizeof(*file2));

	strcpy(file1, "/themes/");
	strcat(file1, theme);
	strcat(file1, "/metacity-1/metacity-theme-1.xml");

	strcpy(file2, "/themes/");
	strcat(file2, theme);
	strcat(file2, "/metacity-1/metacity-theme-2.xml");

	if (xdgh)
		strcpy(dirs, xdgh);
	else {
		strcpy(dirs, home);
		strcat(dirs, "/.local/share");
	}
	strcat(dirs, ":");
	if (xdgd)
		strcat(dirs, xdgd);
	else
		strcat(dirs, "/usr/local/share:/usr/share");
	for (pos = dirs, end = pos + strlen(dirs); pos < end;
	     pos = strchrnul(pos, ':'), pos[0] = '\0', pos++) ;
	for (pos = dirs; pos < end; pos += strlen(pos) + 1) {
		struct stat st;

		strcpy(path, pos);
		strcat(path, file2);
		if (stat(path, &st) || !S_ISREG(st.st_mode)) {
			strcpy(path, pos);
			strcat(path, file1);
			if (stat(path, &st) || !S_ISREG(st.st_mode))
				continue;

		}
		goto got_it;
	}
	free(path);
	free(dirs);
	free(file1);
	free(file2);
	EPRINTF("could not find path for style '%s'\n", theme);
	return NULL;
      got_it:
	free(dirs);
	free(file1);
	free(file2);
	return path;
}

/** @brief Find a metacity style file.
  */
static char *
find_style_METACITY()
{
	char *path;

	if (strchr(options.style, '/')) {
		EPRINTF("path in openbox style name '%s'\n", options.style);
		return NULL;
	}
	if ((path = locate_theme_METACITY(options.style))) {
		char *tmp = path;

		path = strdup(path);
		free(tmp);
	}
	return path;
}

static char *
get_menu_METACITY()
{
	/* NOTE: metacity does not have a root menu of its own */
	get_rcfile_METACITY();
	return NULL;
}

/** @brief Get the current metacity(1) theme.
  */
static char *
get_style_METACITY()
{
	GSettings *settings;
	gchar *val;
	char *theme = NULL;

	if ((settings = g_settings_new("org.gnome.desktop.wm.preferences"))) {
		if ((val = g_settings_get_string(settings, "theme"))) {
			theme = strdup(val);
			g_free(val);
		}
		g_object_unref(G_OBJECT(settings));
	}
	if (theme) {
		char *path;

		free(wm->stylename);
		wm->stylename = theme;
		free(wm->style);
		wm->style = strdup(theme);
		if ((path = locate_theme_METACITY(theme))) {
			free(wm->style);
			wm->style = strdup(path);
			free(wm->stylefile);
			wm->stylefile = strdup(path);
			free(path);
		}
	}
	return theme;
}

/*
 * metacity(1) can be restarted by sending a _METACITY_RESTART_MESSAGE client
 * message to the root window with a mask of SubstructureRedirectMask or
 * SubstructureNotifyMask.  A change to the dconf theme is automatically
 * reloaded when the change is detected; hoever, to forcibly reload a theme (may
 * have changed on disk), send a _METACITY_RELOAD_THEME_MESSAGE to the root
 * window.
 *
 * _METACITY_RESTART_MESSAGE: all data.l elements 0.
 *
 * _METACITY_RELOAD_THEME_MESSAGE: all data.l elements 0.
 *
 * Three other client messages are avaliable.  They are:
 *
 * _METACITY_SET_KEYBINDING_MESSAGE: xev.xclient.data.l[0] = boolean: whether to
 * enable or disable bindings.
 *
 * _METACITY_SET_MOUSEMODS_MESSAGE: xev.xclient.data.l[0] = boolean: whether to
 * enable or disable mouse mods.
 *
 * _METACITY_TOGGLE_VERBOSE: all data.l elements 0.
 */
static void
reload_style_METACITY()
{
	XEvent ev;

	ev.xclient.type = ClientMessage;
	ev.xclient.display = dpy;
	ev.xclient.window = root;
	ev.xclient.message_type = XInternAtom(dpy, "_METACITY_RELOAD_THEME_MESSAGE", False);
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = 0;
	ev.xclient.data.l[1] = 0;
	ev.xclient.data.l[2] = 0;
	ev.xclient.data.l[3] = 0;
	ev.xclient.data.l[4] = 0;
	XSendEvent(dpy, root, False,
			SubstructureNotifyMask | SubstructureRedirectMask, &ev);
	XFlush(dpy);
}

/** @brief Set a metacity(1) style.
  *
  * metacity(1) (and mutter(1)) styles can be set using GSettings in
  * org.gnome.desktop.wm.preferences:theme.
  */
static void
set_style_METACITY()
{
	char *stylefile;

	if (!(stylefile = find_style_METACITY())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		return;
	}
	if (options.dryrun) {
	} else {
		GSettings *settings;

		if ((settings = g_settings_new("org.gnome.desktop.wm.preferences"))) {
			if (!g_settings_set_string(settings, "theme", options.style))
				EPRINTF("could not set style '%s'\n", options.style);
			else {
				g_settings_apply(settings);
				g_settings_sync();
				free(wm->stylename);
				wm->stylename = strdup(options.style);
				free(wm->style);
				wm->style = strdup(stylefile);
				free(wm->stylefile);
				wm->stylefile = strdup(stylefile);
			}
			g_object_unref(G_OBJECT(settings));
		}
		if (options.reload)
			reload_style_METACITY();
	}
	free(stylefile);
	return;
}

static void
list_dir_METACITY(char *xdir, char *style, enum ListType type)
{
	return xde_list_dir_simple(xdir, "themes", "/metacity-1/metacity-theme-2.xml", "", style, type);
}

static void
list_styles_METACITY()
{
	char **dir, *style = get_style_METACITY();

	xde_get_xdg_dirs();

	switch (options.format) {
	case XDE_OUTPUT_HUMAN:
		break;
	case XDE_OUTPUT_SHELL:
		fprintf(stdout, "XDE_WM_STYLES=(\n");
		break;
	case XDE_OUTPUT_PERL:
		fprintf(stdout, "{\n");
		fprintf(stdout, "\tstyles => {\n");
		break;
	}
	for (dir = wm->xdg_dirs; *dir; dir++)
		list_dir_METACITY(*dir, style, XDE_LIST_MIXED);
	switch (options.format) {
	case XDE_OUTPUT_HUMAN:
		break;
	case XDE_OUTPUT_SHELL:
		fprintf(stdout, ")\n");
		break;
	case XDE_OUTPUT_PERL:
		fprintf(stdout, "\t},\n");
		fprintf(stdout, "}\n");
		break;
	}
}

WmOperations xde_wm_ops = {
	"metacity",
	VERSION,
	&get_rcfile_METACITY,
	&find_style_METACITY,
	&get_style_METACITY,
	&set_style_METACITY,
	&reload_style_METACITY,
	&list_dir_METACITY,
	&list_styles_METACITY,
	&get_menu_METACITY
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
