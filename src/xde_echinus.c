/*****************************************************************************

 Copyright (c) 2010-2020  Monavacon Limited <http://www.monavacon.com/>
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

#include "xde.h"

/** @name ECHINUS
  */
/** @{ */

static void
get_rcfile_ECHINUS()
{
	xde_get_rcfile_simple("echinus", ".echinus/echinusrc", "-f");
}

/** @brief Find an echinus style from a style name.
  *
  * Echinus style files are named files or directories in
  * /usr/share/echinus/styles or ~/.echinus/styles.  When a named directory, the
  * directory must contain a file named stylerc.
  */
static char *
find_style_ECHINUS()
{
	return xde_find_style_simple("styles", "/stylerc", "");
}

static char *
from_file_ECHINUS(char *stylerc)
{
	FILE *f;
	char *buf, *b, *e;
	char *stylefile = NULL;

	if (!(f = fopen(stylerc, "r"))) {
		DPRINTF("%s: %s\n", stylerc, strerror(errno));
		return NULL;
	}
	buf = calloc(PATH_MAX + 1, sizeof(*buf));
	while (fgets(buf, PATH_MAX, f)) {
		b = buf;
		b += strspn(b, " \t");
		if (*b == '!' || *b == '\n')
			continue;
		if (strncmp(b, "#include", 8))
			continue;
		b += strspn(b + 8, " \t") + 8;
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
		stylefile = buf;
		break;
	}
	fclose(f);
	if (!stylefile)
		free(buf);
	return stylefile;
}

static char *
get_menu_ECHINUS()
{
	/* NOTE: echinus does not have a root menu */
	get_rcfile_ECHINUS();
	return NULL;
}


/** @brief Get the style for echinus.
  *
  * There are two ways to implement the style system for echinus: symbolic links
  * or #include statements.  Both accept absolute and relative paths.  The
  * stylerc file in turn links to or includes a stylerc file from the
  * appropriate styles subdirectory.
  *
  * The symbolic link approach is likely best.  Either acheives the same result.
  */
static char *
get_style_ECHINUS()
{
	return xde_get_style_simple("stylerc", &from_file_ECHINUS);
}

/** @brief Reload an echinus style.
  */
static void
reload_style_ECHINUS()
{
	if (wm->pid)
		kill(wm->pid, SIGHUP);
	else
		EPRINTF("%s", "cannot restart echinus without a pid\n");
}

static void
to_file_ECHINUS(char *stylerc, char *stylefile)
{
	FILE *f;

	if (!(f = fopen(stylerc, "w"))) {
		EPRINTF("%s: %s\n", stylerc, strerror(errno));
		return;
	}
	fprintf(f, "#include \"%s\"\n", stylefile);
	fclose(f);
}

/** @brief Set the style for echinus.
  *
  * Our style system for echinus places an '#include' statement in the rc file
  * that includes the stylerc file in the same directory as the rc file.  The
  * stylerc file in turn includes a stylerc file from the appropriate styles
  * subdirectory.  Alternately, we can make the stylerc file a symbolic link to
  * the style directory stylerc file.
  *
  * Sending a SIGHUP will get echinus to restart.
  */
static void
set_style_ECHINUS()
{
	return xde_set_style_simple("stylerc", &to_file_ECHINUS);
}

static void
list_dir_ECHINUS(char *xdir, char *style, enum ListType type)
{
	return xde_list_dir_simple(xdir, "styles", "/stylerc", "", style, type);
}

static void
list_styles_ECHINUS()
{
	return xde_list_styles_simple();
}

static void
gen_item_ECHINUS(char *style, enum ListType type, char *stylename, char *file)
{
	(void) style;
	(void) type;
	(void) stylename;
	(void) file;
}

static void
gen_dir_ECHINUS(char *xdir, char *style, enum ListType type)
{
	(void) xdir;
	(void) style;
	(void) type;
}

static void
gen_menu_ECHINUS()
{
}

static char *
get_icon_ECHINUS()
{
	return xde_get_icon_simple("echinus");
}

WmOperations xde_wm_ops = {
	.name = "echinus",
	.version = VERSION,
	.get_rcfile = &get_rcfile_ECHINUS,
	.find_style = &find_style_ECHINUS,
	.get_style = &get_style_ECHINUS,
	.set_style = &set_style_ECHINUS,
	.reload_style = &reload_style_ECHINUS,
	.list_dir = &list_dir_ECHINUS,
	.list_styles = &list_styles_ECHINUS,
	.get_menu = &get_menu_ECHINUS,
	.gen_item = &gen_item_ECHINUS,
	.gen_dir = &gen_dir_ECHINUS,
	.gen_menu = &gen_menu_ECHINUS,
	.get_icon = &get_icon_ECHINUS
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
