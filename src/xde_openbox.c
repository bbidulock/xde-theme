/*****************************************************************************

 Copyright (c) 2010-2017  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2002-2009  OpenSS7 Corporation <http://www.openss7.com/>
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
	} else {
		static char *suffix = "/openbox/rc.xml";
		static char *subdir = "/.config";

		if (cnfg) {
			len = strlen(cnfg) + strlen(suffix) + 1;
			wm->rcfile = calloc(len, sizeof(*wm->rcfile));
			strcpy(wm->rcfile, cnfg);
			strcat(wm->rcfile, suffix);
		} else {
			len = strlen(home) + strlen(subdir) + strlen(suffix) + 1;
			wm->rcfile = calloc(len, sizeof(*wm->rcfile));
			strcpy(wm->rcfile, home);
			strcat(wm->rcfile, subdir);
			strcat(wm->rcfile, suffix);
		}
	}
	DPRINTF("pdir = '%s'\n", wm->pdir);
	free(wm->pdir);
	wm->pdir = strdup(wm->rcfile);
	DPRINTF("pdir = '%s'\n", wm->pdir);
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

/** @brief Locate and openbox theme file.
  *
  * Openbox style fiels are XDG organized: that is, the searched directories are
  * $XDG_DATA_HOME:$XDG_DATA_DIRS with appropriate defaults.  Subdirectories
  * under the themes directory (e.g. /usr/share/themes/STYLENAME) that contain
  * an openbox-3 subdirectory containing a themerc file.
  *
  * Because openbox uses the XDG scheme, it does not distinguish between system
  * and user styles.
  */
static char *
locate_theme_OPENBOX(char *theme)
{
	char *dirs, *path, *file;
	char *pos, *end;

	char *xdgh = xde_get_proc_environ("XDG_DATA_HOME");
	char *xdgd = xde_get_proc_environ("XDG_DATA_DIRS");
	char *home = xde_get_proc_environ("HOME") ? : ".";

	dirs = calloc(PATH_MAX, sizeof(*dirs));
	path = calloc(PATH_MAX, sizeof(*path));
	file = calloc(PATH_MAX, sizeof(*file));

	strcpy(file, "/themes/");
	strcat(file, theme);
	strcat(file, "/openbox-3/themerc");

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
		strcat(path, file);
		if (stat(path, &st))
			continue;
		if (!S_ISREG(st.st_mode))
			continue;
		goto got_it;
	}
	free(path);
	free(dirs);
	free(file);
	EPRINTF("could not find path for style '%s'\n", theme);
	return NULL;
      got_it:
	free(dirs);
	free(file);
	return path;
}

/** @brief Find an openbox style file.
  */
static char *
find_style_OPENBOX()
{
	char *path;

	if (strchr(options.style, '/')) {
		EPRINTF("path in openbox style name '%s'\n", options.style);
		return NULL;
	}
	if ((path = locate_theme_OPENBOX(options.style))) {
		char *tmp = path;

		path = strdup(path);
		free(tmp);
	}
	return path;
}

/** @brief Get the path to the openbox menu.
  *
  * Openbox specifies its menu files by placing a <file>path_to_menu</file> line
  * in a <menu></menu> section of the the rc.xml (primary configuraiton) file.
  * We don't really want to parse the entire XML of the file, so we scan for a
  * line containing <menu> and parse any <file>path_to_menu</file> lines after
  * it and before a line continaing </menu>.  Care should be take because there
  * are other <menu></menu> sections in the file (but none of the other contain
  * a <file></file> subsection).
  *
  * The default when no <file></file> is present is the menu.xml file in the
  * configuration directory.  It is questionable whether it will be loaded
  * regardless of the existence of a <file></file> section.  The file that will
  * be used for the root menu is the last one with a <menu id="root-menu" line
  * in it.
  *
  * Well, it looks like <file>menu.xml</file> is implicit and always loaded
  * last.
  */
static char *
get_menu_OPENBOX()
{
	char *menu = NULL;

	get_rcfile_OPENBOX();
	if (wm->pdir && *wm->pdir) {
		static char *suffix = "/menu.xml";
		int len = strlen(wm->pdir) + strlen(suffix) + 1;

		menu = calloc(len, sizeof(*menu));
		strcpy(menu, wm->pdir);
		strcat(menu, suffix);
	}
	if (menu) {
		free(wm->menu);
		wm->menu = menu;
	}
	return menu;
}

static char *
get_style_OPENBOX()
{
	char *theme = NULL;
	XTextProperty tp = { NULL, };

	get_rcfile_OPENBOX();
	XGetTextProperty(dpy, root, &tp, _XA_OB_THEME);
	if (tp.value) {
		theme = strndup((char *) tp.value, tp.nitems);
		XFree(tp.value);
	}
	if (theme) {
		char *path;

		free(wm->stylename);
		wm->stylename = theme;
		free(wm->style);
		wm->style = strdup(theme);
		if ((path = locate_theme_OPENBOX(theme))) {
			free(wm->style);
			wm->style = strdup(path);
			free(wm->stylefile);
			wm->stylefile = strdup(path);
			free(path);
		}
	}
	return theme;
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
  * writes the file and performs a reconfigure.  The entry in the rc.xml file
  * looks like:
  *
  *   <theme>
  *     <name>Penguins</name>
  *     ...
  *   </theme>
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
	FILE *f;
	struct stat st;
	char *stylefile, *buf, *pos, *end, *line, *p, *q;
	int len;
	size_t read, total;

	if (!(stylefile = find_style_OPENBOX())) {
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
	total = 0;
	while (total < st.st_size) {
		read = fread(buf + total, 1, st.st_size - total, f);
		total += read;
		if (total >= st.st_size)
			break;
		if (ferror(f)) {
			EPRINTF("%s: %s\n", wm->rcfile, strerror(errno));
			goto no_buf;
		}
		if (feof(f))
			break;
	}
	len = strlen(options.style) + strlen("<name>%s</name>");
	line = calloc(len, sizeof(*line));
	snprintf(line, len, "<name>%s</name>", options.style);
	if (strstr(buf, line)) {
		DPRINTF("%s: no change\n", stylefile);
		goto no_change;
	}
	if (options.dryrun) {
	} else {
		int intheme = 0;

		if (!(f = freopen(wm->rcfile, "w", f))) {
			EPRINTF("%s: %s\n", wm->rcfile, strerror(errno));
			goto no_change;
		}
		for (pos = buf, end = buf + st.st_size; pos < end; pos = pos + strlen(pos) + 1) {
			*strchrnul(pos, '\n') = '\0';
			if (intheme) {
				if ((p = strstr(pos, "<name>"))
				    && (!(q = strstr(pos, "<!--")) || p < q)) {
					DPRINTF("%s: printing line '    %s'\n", wm->rcfile, line);
					fprintf(f, "    %s\n", line);
					continue;
				} else if ((p = strstr(pos, "</theme>"))
					   && (!(q = strstr(pos, "<!--")) || p < q))
					intheme = 0;
			} else if ((p = strstr(pos, "<theme>"))
				   && (!(q = strstr(pos, "<!--")) || p < q))
				intheme = 1;
			DPRINTF("%s: printing line '%s'\n", wm->rcfile, pos);
			fprintf(f, "%s\n", pos);
		}
		if (options.reload)
			reload_style_OPENBOX();
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
list_dir_OPENBOX(char *xdir, char *style, enum ListType type)
{
	return xde_list_dir_simple(xdir, "themes", "/openbox-3/themerc", "", style, type);
}

static void
list_styles_OPENBOX()
{
	char **dir, *style = get_style_OPENBOX();

	xde_get_xdg_dirs();

	switch (options.format) {
	case XDE_OUTPUT_HUMAN:
	case XDE_OUTPUT_PROPS:
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
		list_dir_OPENBOX(*dir, style, XDE_LIST_MIXED);
	switch (options.format) {
	case XDE_OUTPUT_HUMAN:
	case XDE_OUTPUT_PROPS:
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

static void
gen_item_OPENBOX(char *style, enum ListType type, char *stylename, char *file)
{
	switch(type) {
	case XDE_LIST_PRIVATE:
	case XDE_LIST_USER:
		fprintf(stdout, "  <item label=\"%s\">\n", stylename);
		fprintf(stdout, "    <action name=\"Execute\">\n");
		fprintf(stdout, "      <command>xde-style -s -t -r -u '%s'</command>\n", stylename);
		fprintf(stdout, "    </action>\n");
		fprintf(stdout, "  </item>\n");
		break;
	case XDE_LIST_SYSTEM:
	case XDE_LIST_GLOBAL:
		fprintf(stdout, "  <item label=\"%s\">\n", stylename);
		fprintf(stdout, "    <action name=\"Execute\">\n");
		fprintf(stdout, "      <command>xde-style -s -t -r -y '%s'</command>\n", stylename);
		fprintf(stdout, "    </action>\n");
		fprintf(stdout, "  </item>\n");
		break;
	case XDE_LIST_MIXED:
		fprintf(stdout, "  <item label=\"%s\">\n", stylename);
		fprintf(stdout, "    <action name=\"Execute\">\n");
		fprintf(stdout, "      <command>xde-style -s -t -r '%s'</command>\n", stylename);
		fprintf(stdout, "    </action>\n");
		fprintf(stdout, "  </item>\n");
		break;
	}
}

static void
gen_dir_OPENBOX(char *xdir, char *style, enum ListType type)
{
	return xde_gen_dir_simple(xdir, "themes", "/openbox-3/themerc", "", style, type);
}

static void
gen_menu_OPENBOX()
{
	char **dir, *style = get_style_OPENBOX();

	xde_get_xdg_dirs();

	fprintf(stdout, "%s\n", "<openbox_pipe_menu>");
	for (dir = wm->xdg_dirs; *dir; dir++)
		gen_dir_OPENBOX(*dir, style, XDE_LIST_MIXED);
	fprintf(stdout, "%s\n", "</openbox_pipe_menu>");
}

static char *
get_icon_OPENBOX()
{
	return xde_get_icon_simple("openbox");
}

WmOperations xde_wm_ops = {
	.name = "openbox",
	.version = VERSION,
	.get_rcfile = &get_rcfile_OPENBOX,
	.find_style = &find_style_OPENBOX,
	.get_style = &get_style_OPENBOX,
	.set_style = &set_style_OPENBOX,
	.reload_style = &reload_style_OPENBOX,
	.list_dir = &list_dir_OPENBOX,
	.list_styles = &list_styles_OPENBOX,
	.get_menu = &get_menu_OPENBOX,
	.gen_item = &gen_item_OPENBOX,
	.gen_dir = &gen_dir_OPENBOX,
	.gen_menu = &gen_menu_OPENBOX,
	.get_icon = &get_icon_OPENBOX
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
