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

/* icewm(1) plugin */

#include "xde.h"

/** @name ICEWM
  */
/** @{ */

/** @brief Find the icewm rc file and default directory.
  *
  * Icewm takes a command such as: 
  *
  *   icewm [-c, --config=RCFILE] [-t, --theme=FILE]
  *
  * The default if RCFILE is not specified is ~/.icewm/preferences, unless the
  * ICEWM_PRIVCFG environment variable is specified, 
  */
static void
get_rcfile_ICEWM()
{
	char *home = xde_get_proc_environ("HOME") ? : ".";
	char *file = xde_get_rcfile_optarg("--config");
	char *cnfg = xde_get_proc_environ("ICEWM_PRIVCFG");
	int len;

	free(wm->udir);
	wm->udir = calloc(strlen(home) + strlen("/.icewm") + 1, sizeof(*wm->udir));
	strcpy(wm->udir, home);
	strcat(wm->udir, "/.icewm");
	free(wm->pdir);
	if (cnfg) {
		if (cnfg[0] == '/')
			wm->pdir = strdup(cnfg);
		else {
			len = strlen(home) + strlen(cnfg) + 2;
			wm->pdir = calloc(len, sizeof(*wm->pdir));
			strcpy(wm->pdir, home);
			strcat(wm->pdir, "/");
			strcat(wm->pdir, cnfg);
		}
	} else
		wm->pdir = strdup(wm->udir);
	free(wm->sdir);
	wm->sdir = strdup("/usr/share/icewm");
	free(wm->edir);
	wm->edir = strdup("/etc/icewm");

	free(wm->rcfile);
	if (file) {
		if (file[0] == '/')
			wm->rcfile = strdup(file);
		else {
			len = strlen(wm->pdir) + strlen(file) + 2;
			wm->rcfile = calloc(len, sizeof(*wm->rcfile));
			strcpy(wm->rcfile, wm->pdir);
			strcat(wm->rcfile, "/");
			strcat(wm->rcfile, file);
		}
		free(file);
	} else {
		len = strlen(wm->pdir) + strlen("/preferences") + 1;
		wm->rcfile = calloc(len, sizeof(*wm->rcfile));
		strcpy(wm->rcfile, wm->pdir);
		strcat(wm->rcfile, "/preferences");
	}
}

/** @brief Find an icewm style file.
  *
  * IceWM cannot distinguish between system and user styles.  The theme name
  * specifies a directory in the /usr/share/icewm/themes, ~/.icewm/themes or
  * $ICEWM_PRIVCFG/themes subdirectories.  Theme files can either be a
  * default.theme file in the subdirectory of the theme name, or theme
  * variations can be provided in $VARIATION.theme files.  Theme names can be
  * specified as, e.g. "Airforce" or "Airforce/default" or
  * "ElbergBlue/Wallpaper" for the variation.  The actual file for "Airforce" is
  * "Airforce/default.theme" in one of the theme directories.  The actual type
  * for "ElbergBlue/Wallpaper" is ElbergBlue/Wallpaper.theme.
  */
static char *
find_style_ICEWM()
{
	char *p, *file, *path = calloc(PATH_MAX, sizeof(*path));
	int i, len, beg, end;

	if (options.style[0] == '.' || options.style[0] == '/') {
		EPRINTF("path in icewm style name '%s'\n", options.style);
		return NULL;
	}
	get_rcfile_ICEWM();
	if (!(strchr(options.style, '/'))) {
		len = strlen(options.style) + strlen("/default.theme") + 1;
		file = calloc(len, sizeof(*file));
		snprintf(file, len, "%s/default.theme", options.style);
	} else if (!(p = strstr(options.style, ".theme"))
		   || p[strlen(".theme") + 1] != '\0') {
		len = strlen(options.style) + strlen(".theme") + 1;
		file = calloc(len, sizeof(*file));
		snprintf(file, len, "%s.theme", options.style);
	} else {
		len = strlen(options.style) + 1;
		file = calloc(len, sizeof(*file));
		snprintf(file, len, "%s", options.style);
	}
	if (options.user && !options.system) {
		beg = 0;
		end = 2;
	}
	else if (options.system && !options.user) {
		beg = 2;
		end = CHECK_DIRS;
	}
	else {
		beg = 0;
		end = CHECK_DIRS;
	}
	for (i = beg; i < end; i++) {
		if (!wm->dirs[i] || !wm->dirs[i][0])
			continue;
		snprintf(path, PATH_MAX, "%s/themes/%s", wm->dirs[i], file);
		if (xde_check_file(path))
			break;
	}
	if (i < end) {
		free(options.style);
		options.style = file;
		return path;
	}
	free(file);
	free(path);
	return NULL;
}

static char *
get_menu_ICEWM()
{
	return xde_get_menu_simple("menu", NULL);
}

/** @brief Get the icewm style.
  *
  * When icewm changes the style, it writes the new style to the ~/.icewm/theme
  * or $ICEWM_PRIVCFG/theme file and then restarts.  The ~/.icewm/theme file
  * looks like:
  *
  *   Theme="Penguins/default.theme"
  *   #Theme="Airforce/default.theme"
  *   ##Theme="Penguins/default.theme"
  *   ###Theme="Pedestals/default.theme"
  *   ####Theme="Penguins/default.theme"
  *   #####Theme="Airforce/default.theme"
  *   ######Theme="Archlinux/default.theme"
  *   #######Theme="Airforce/default.theme"
  *   ########Theme="Airforce/default.theme"
  *   #########Theme="Airforce/default.theme"
  *   ##########Theme="Penguins/default.theme"
  *
  */
static char *
get_style_ICEWM()
{
	FILE *f;
	struct stat st;
	char *stylefile, *themerc, *buf, *pos, *trm;
	int i, len, beg, end;
	size_t read, total;

	get_rcfile_ICEWM();
	len = strlen(wm->pdir) + strlen("/theme") + 1;
	themerc = calloc(len, sizeof(*themerc));
	snprintf(themerc, len, "%s/theme", wm->pdir);

	if (!(f = fopen(themerc, "r"))) {
		EPRINTF("%s: %s\n", themerc, strerror(errno));
		goto no_themerc;
	}
	if (fstat(fileno(f), &st)) {
		EPRINTF("%s: %s\n", themerc, strerror(errno));
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
			EPRINTF("%s: %s\n", themerc, strerror(errno));
			goto no_buf;
		}
		if (feof(f))
			break;
	}
	pos = trm = buf;
	if (strncmp(pos, "Theme=\"", 7) != 0) {
		EPRINTF("no theme at start of rc file\n");
		goto no_theme;
	}
	pos += 7;
	if (!(trm = strchr(pos, '"'))) {
		EPRINTF("no theme at start of rc file\n");
		goto no_theme;
	}
	*trm = '\0';
	free(wm->stylename);
	wm->stylename = strdup(pos);
	stylefile = calloc(PATH_MAX, sizeof(*stylefile));
	if (options.user && !options.system) {
		beg = 0;
		end = 2;
	}
	else if (options.system && !options.user) {
		beg = 2;
		end = CHECK_DIRS;
	}
	else {
		beg = 0;
		end = CHECK_DIRS;
	}
	for (i = beg; i < end; i++) {
		if (!wm->dirs[i] || !wm->dirs[i][0])
			continue;
		snprintf(stylefile, PATH_MAX, "%s/themes/%s", wm->dirs[i], wm->stylename);
		if (xde_check_file(stylefile))
			break;
	}
	if (i < end) {
		free(wm->style);
		wm->style = strdup(stylefile);
	}
	free(stylefile);
	return wm->style;
      no_theme:
      no_buf:
	free(buf);
      no_stat:
	fclose(f);
      no_themerc:
	free(themerc);
	return NULL;
}

#define ICEWM_ACTION_NOP		0
#define ICEWM_ACTION_PING		1
#define ICEWM_ACTION_LOGOUT		2
#define ICEWM_ACTION_CANCEL_LOGOUT	3
#define ICEWM_ACTION_REBOOT		4
#define ICEWM_ACTION_SHUTDOWN		5
#define ICEWM_ACTION_ABOUT		6
#define ICEWM_ACTION_WINDOWLIST		7
#define ICEWM_ACTION_RESTARTWM		8

/** @brief Reload an icewm style.
  *
  * There are two ways to get icewm to reload the theme, one is to send a SIGHUP
  * to the window manager process.  The other is to send an _ICEWM_ACTION client
  * message to the root window.
  */
static void
reload_style_ICEWM()
{
	XEvent ev;

#if 0	/* client message is enough */
	if (wm->pid)
		kill(wm->pid, SIGHUP);
#endif

	ev.xclient.type = ClientMessage;
	ev.xclient.display = dpy;
	ev.xclient.window = root;
	ev.xclient.message_type = XInternAtom(dpy, "_ICEWM_ACTION", False);
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = 0;
	ev.xclient.data.l[1] = ICEWM_ACTION_RESTARTWM;	/* XXX: strange... */
	ev.xclient.data.l[2] = 0;
	ev.xclient.data.l[3] = 0;
	ev.xclient.data.l[4] = 0;
	XSendEvent(dpy, root, False, SubstructureNotifyMask | SubstructureRedirectMask,
		   &ev);
	XFlush(dpy);
}

/** @brief Set the icewm style.
  *
  * When icewm changes the style, it writes the new style to the ~/.icewm/theme
  * or $ICEWM_PRIVCFG/theme file and then restarts.  The ~/.icewm/theme file
  * looks like:
  *
  *   Theme="Penguins/default.theme"
  *   #Theme="Airforce/default.theme"
  *   ##Theme="Penguins/default.theme"
  *   ###Theme="Pedestals/default.theme"
  *   ####Theme="Penguins/default.theme"
  *   #####Theme="Airforce/default.theme"
  *   ######Theme="Archlinux/default.theme"
  *   #######Theme="Airforce/default.theme"
  *   ########Theme="Airforce/default.theme"
  *   #########Theme="Airforce/default.theme"
  *   ##########Theme="Penguins/default.theme"
  *
  * icewm cannot distinguish between system an user styles.  The theme name
  * specifies a directory in the /usr/share/icewm/themes, ~/.icewm/themes or
  * $ICEWM_PRIVCFG/themes subdirectories.
  *
  * When xde-session runs, it sets the ICEWM_PRIVCFG environment variable.
  * xde-session and associated tools will always set this environment variable
  * before launching icewm.  icewm respects this environment variable and no
  * special options are necessary when launching icewm.
  *
  * The default configuration directory when ICEWM_PRIVCFG is not specified is
  * ~/.icewm.  The location of all other icewm configuration files are in this
  * directory.  xde-session typically sets ICEWM_PRIVCFG to
  * $XDG_CONFIG_HOME/icewm.
  */
static void
set_style_ICEWM()
{
	FILE *f;
	struct stat st;
	char *stylefile, *themerc, *buf, *pos, *end, *line;
	int n, len;
	size_t read, total;

	if (!(stylefile = find_style_ICEWM())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		goto no_stylefile;
	}
	free(stylefile);

	len = strlen(wm->pdir) + strlen("/theme") + 1;
	themerc = calloc(len, sizeof(*themerc));
	snprintf(themerc, len, "%s/theme", wm->pdir);

	if (!(f = fopen(themerc, "r"))) {
		EPRINTF("%s: %s\n", themerc, strerror(errno));
		goto no_themerc;
	}
	if (fstat(fileno(f), &st)) {
		EPRINTF("%s: %s\n", themerc, strerror(errno));
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
			EPRINTF("%s: %s\n", themerc, strerror(errno));
			goto no_buf;
		}
		if (feof(f))
			break;
	}

	len = strlen(options.style) + strlen("Theme=\"\"") + 1;
	line = calloc(len, sizeof(*line));
	snprintf(line, len, "Theme=\"%s\"", options.style);
	if (strncmp(buf, line, strlen(line)) == 0) {
		OPRINTF("style %s did not change\n", line);
		goto no_change;
	}

	if (options.dryrun) {
	} else {
		OPRINTF("writing new style %s\n", options.style);
		if (!(f = freopen(themerc, "w", f))) {
			EPRINTF("%s: %s\n", themerc, strerror(errno));
			goto no_change;
		}
		fprintf(f, "Theme=\"%s\"\n", options.style);
		for (n = 0, pos = buf, end = buf + st.st_size; pos < end && n < 10;
		     n++, pos = pos + strlen(pos) + 1) {
			*strchrnul(pos, '\n') = '\0';
			fprintf(f, "#%s\n", pos);
		}
		if (options.reload)
			reload_style_ICEWM();
	}
      no_change:
	free(line);
      no_buf:
	free(buf);
      no_stat:
	fclose(f);
      no_themerc:
	free(themerc);
      no_stylefile:
	return;
}

static void
list_dir_ICEWM(char *xdir, char *style, enum ListType type)
{
	DIR *dir, *sub;
	char *dirname, *subdir, *file, *pos, *name;
	struct dirent *d, *e;
	struct stat st;
	int len;

	if (!xdir || !*xdir)
		return;
	len = strlen(xdir) + strlen("/themes") + 1;
	dirname = calloc(len, sizeof(*dirname));
	strcpy(dirname, xdir);
	strcat(dirname, "/themes");
	if (!(dir = opendir(dirname))) {
		DPRINTF("%s: %s\n", dirname, strerror(errno));
		free(dirname);
		return;
	}
	while ((d = readdir(dir))) {
		if (d->d_name[0] == '.')
			continue;
		len = strlen(dirname) + strlen(d->d_name) + 2;
		subdir = calloc(len, sizeof(*subdir));
		strcpy(subdir, dirname);
		strcat(subdir, "/");
		strcat(subdir, d->d_name);
		if (stat(subdir, &st)) {
			EPRINTF("%s: %s\n", subdir, strerror(errno));
			free(subdir);
			continue;
		}
		if (!S_ISDIR(st.st_mode)) {
			DPRINTF("%s: not a directory\n", subdir);
			free(subdir);
			continue;
		}
		if (!(sub = opendir(subdir))) {
			EPRINTF("%s: %s\n", subdir, strerror(errno));
			free(subdir);
			continue;
		}
		while ((e = readdir(sub))) {
			if (e->d_name[0] == '.')
				continue;
			if (!(pos = strstr(e->d_name, ".theme")) ||
			    pos != e->d_name + strlen(e->d_name) - 6)
				continue;
			len = strlen(subdir) + strlen(e->d_name) + 2;
			file = calloc(len, sizeof(*file));
			strcpy(file, subdir);
			strcat(file, "/");
			strcat(file, e->d_name);
			if (stat(file, &st)) {
				EPRINTF("%s: %s\n", file, strerror(errno));
				free(file);
				continue;
			}
			if (!S_ISREG(st.st_mode)) {
				DPRINTF("%s: not a regular file\n", file);
				free(file);
				continue;
			}
			len = strlen(d->d_name) + strlen(e->d_name) + 2;
			name = calloc(len, sizeof(*name));
			strcpy(name, d->d_name);
			if (strcmp(e->d_name, "default.theme")) {
				strcat(name, "/");
				strcat(name, e->d_name);
				*strstr(name, ".theme") = '\0';
			}
			if (!options.theme || xde_find_theme(name, NULL)) {
				switch (options.format) {
				case XDE_OUTPUT_HUMAN:
				case XDE_OUTPUT_PROPS:
					fprintf(stdout, "%s %s%s\n", name, file,
						(style && !strcmp(style, file)) ? " *" : "");
					break;
				case XDE_OUTPUT_SHELL:
					fprintf(stdout, "\t'%s\t%s\t%s'\n", name, file,
						(style && !strcmp(style, file)) ? "*" : "");
					break;
				case XDE_OUTPUT_PERL:
					fprintf(stdout, "\t\t'%s' => [ '%s', %d ],\n", name, file,
						(style && !strcmp(style, file)) ? 1 : 0);
					break;
				}
			}
			free(name);
			free(file);
		}
		closedir(sub);
		free(subdir);
	}
	closedir(dir);
	free(dirname);
}

static void
list_styles_ICEWM()
{
	return xde_list_styles_simple();
}

static void
gen_item_ICEWM(char *style, enum ListType type, char *stylename, char *file)
{
}

static void
gen_dir_ICEWM(char *xdir, char *style, enum ListType type)
{
}

static void
gen_menu_ICEWM()
{
}

static char *
get_icon_ICEWM()
{
	return xde_get_icon_simple("icewm");
}

WmOperations xde_wm_ops = {
	.name = "icewm",
	.version = VERSION,
	.get_rcfile = &get_rcfile_ICEWM,
	.find_style = &find_style_ICEWM,
	.get_style = &get_style_ICEWM,
	.set_style = &set_style_ICEWM,
	.reload_style = &reload_style_ICEWM,
	.list_dir = &list_dir_ICEWM,
	.list_styles = &list_styles_ICEWM,
	.get_menu = &get_menu_ICEWM,
	.gen_item = &gen_item_ICEWM,
	.gen_dir = &gen_dir_ICEWM,
	.gen_menu = &gen_menu_ICEWM,
	.get_icon = &get_icon_ICEWM
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
