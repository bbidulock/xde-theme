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

/** @name WMAKER
  */
/** @{ */

/** @brief Find the windowmaker rc file and default directory.
  *
  * Windowmaker observes the GNUSTEP_USER_ROOT environment variable.  When not
  * specified, it defaults to ~/GNUstep/Defaults/WindowMaker.  The locations of
  * all wmaker configuration files are under the same directory.
  */
static void
get_rcfile_WMAKER()
{
	char *home = xde_get_proc_environ("HOME") ? : ".";
	char *cnfg = xde_get_proc_environ("GNUSTEP_USER_ROOT");

	free(wm->udir);
	wm->udir = calloc(strlen(home) + strlen("/GNUstep") + 1, sizeof(*wm->udir));
	strcpy(wm->udir, home);
	strcat(wm->udir, "/GNUstep");
	free(wm->pdir);
	if (cnfg) {
		if (cnfg[0] == '/')
			wm->pdir = strdup(cnfg);
		else {
			wm->pdir =
			    calloc(strlen(home) + strlen(cnfg) + 2, sizeof(*wm->pdir));
			strcpy(wm->pdir, home);
			strcat(wm->pdir, "/");
			strcat(wm->pdir, cnfg);
		}
	} else
		wm->pdir = strdup(wm->udir);
	free(wm->sdir);
	wm->sdir = strdup("/usr/share/WindowMaker");
	free(wm->edir);
	wm->edir = strdup("/etc/WindowMaker");

	free(wm->rcfile);
	wm->rcfile =
	    calloc(strlen(wm->pdir) + strlen("/Defaults/WindowMaker") + 1,
		   sizeof(*wm->rcfile));
	strcpy(wm->rcfile, wm->pdir);
	strcat(wm->rcfile, "/Defaults/WindowMaker");
}

static char *
find_style_WMAKER()
{
	char *pos, *path = calloc(PATH_MAX, sizeof(*path)), *res = NULL;
	int i, len, beg, end;

	get_rcfile_WMAKER();
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
		strcpy(path, wm->dirs[i]);
		if (i < 2)
			strcat(path, "/Library/WindowMaker");
		len = strlen(path);
		pos = path + len;
		snprintf(pos, PATH_MAX - len, "/Themes/%s.themed/style", options.style);
		if (xde_test_file(path)) {
			res = strdup(path);
			break;
		}
		snprintf(pos, PATH_MAX - len, "/Themes/%s.style", options.style);
		if (xde_test_file(path)) {
			res = strdup(path);
			break;
		}
		snprintf(pos, PATH_MAX - len, "/Styles/%s.style", options.style);
		if (xde_test_file(path)) {
			res = strdup(path);
			break;
		}
	}
	free(path);
	return res;
}

/** @brief get the WindowMaker menu
  *
  * There are two menus that WindowMaker can use: a new style (properties list)
  * menu which is placed in $GNUSTEP_USER_ROOT/Defaults/WMRootMenu, or an old
  * style menu which is linked to by placing "path" in
  * $GNUSTEP_USER_ROOT/Defaults/WMRootMenu and then placing the the old style
  * menu in "path".  When "path" is relative, it is relative to
  * $GNUSTEP_USER_ROOT/Library/WindowMaker or /usr/share/WindowMaker, whichever
  * actually contains the file.  When $GNUSTEP_USER_ROOT/Defaults/WMRootMenu
  * contains an invalid menu or does not exist, the properties list menu at
  * /usr/share/WindowMaker/plmenu[.lang] is used instead.  Because this function
  * wishes to return a menu file that should be written to (instead of read), we
  * will examine $GNUSTEP_USER_ROOT/Defaults/WMRootMenu to determine which menu
  * type it should contain and then either return the referenced old-style menu
  * or the new style menu.
  */
static char *
get_menu_WMAKER()
{
	FILE *f;
	char *buf, *file, *dir;
	struct stat st;
	static char *suf1 = "/Defaults/WMRootMenu";
	static char *suf2 = "/Library/WindowMaker/";

	get_rcfile_WMAKER();
	if (!(dir = wm->pdir) || *dir == '\0')
		return NULL;
	file = calloc(strlen(dir) + strlen(suf1) + 1, sizeof(*file));
	strcpy(file, dir);
	strcat(file, suf1);
	if (stat(file, &st)) {
		DPRINTF("%s: %s\n", file, strerror(errno));
		goto use_file;
	}
	if (!S_ISREG(st.st_mode)) {
		DPRINTF("%s: not a file\n", file);
		goto use_file;
	}
	if (!(f = fopen(file, "r"))) {
		DPRINTF("%s: %s\n", file, strerror(errno));
		goto use_file;
	}
	buf = calloc(PATH_MAX + 1, sizeof(*buf));
	if (fgets(buf, PATH_MAX, f)) {
		char *b, *e;

		e = buf + strlen(buf);
		for (b = buf; b < e && isspace(*b); b++) ;
		for (e--; e > b && (*e == '\n' || isspace(*e)); e--) ;
		if (e > b + 2 && *b == '"' && *e == '"') {
			*e = '\0';
			b++;
			free(file);
			if (*b == '/') {
				file = strdup(b);
			} else {
				int len = strlen(dir) + strlen(suf2) + strlen(b) + 1;

				file = calloc(len, sizeof(*file));
				strcpy(file, dir);
				strcat(file, suf2);
				strcat(file, b);
			}
		}
	}
	free(buf);
	fclose(f);
      use_file:
	free(wm->menu);
	wm->menu = file;
	return file;
}

/** @brief get the WindowMaker style
  *
  * WindowMaker does not actually have a link or a reference to a style file.
  * Styles are changed by adding properties to G/D/W.
  */
static char *
get_style_WMAKER()
{
	get_rcfile_WMAKER();
	return NULL;
}

static void
set_style_WMAKER()
{
	char *stylefile;

	if (!(stylefile = find_style_WMAKER())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		return;
	}
}

static void
reload_style_WMAKER()
{
}

static void
list_dir_WMAKER(char *xdir, char *style, enum ListType type)
{
}

static void
list_styles_WMAKER()
{
}

static void
gen_menu_WMAKER()
{
}

WmOperations xde_wm_ops = {
	"wmaker",
	VERSION,
	&get_rcfile_WMAKER,
	&find_style_WMAKER,
	&get_style_WMAKER,
	&set_style_WMAKER,
	&reload_style_WMAKER,
	&list_dir_WMAKER,
	&list_styles_WMAKER,
	&get_menu_WMAKER,
	&gen_menu_WMAKER
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
