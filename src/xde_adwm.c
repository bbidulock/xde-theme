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

#include "xde.h"

/** @name ADWM
  */
/** @{ */

static void
get_rcfile_ADWM()
{
	xde_get_rcfile_simple("adwm", ".adwm/adwmrc", "-f");
}

/** @brief Find an adwm style from a style name.
  *
  * Adwm style files are named files or directories in /usr/share/adwm/styles or
  * ~/.adwm/styles.  When a named directory, the directory must contain a file
  * named stylerc.
  */
static char *
find_style_ADWM()
{
	return xde_find_style_simple("styles", "/stylerc", "");
}

static char *
from_file_ADWM(char *stylerc)
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
		b += 8;
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
		stylefile = buf;
		break;
	}
	fclose(f);
	if (!stylefile)
		free(buf);
	return stylefile;
}

static char *
get_menu_ADWM()
{
	/* NOTE: adwm does not have a root menu (yet)... */
	get_rcfile_ADWM();
	return NULL;
}

/** @brief Get the style for adwm.
  *
  * There are two ways to implement the style system for adwm: symbolic links or
  * #include statements.  Both accept absolute and relative paths.  The stylerc
  * file in turn links to or includes a stylerc file from the appropriate styles
  * subdirectory.
  *
  * The symbolic link approach is likely the best.  Either acheives the same
  * result.
  */
static char *
get_style_ADWM()
{
	return xde_get_style_simple("stylerc", &from_file_ADWM);
}

/** @brief Reload an adwm style.
  */
static void
reload_style_ADWM()
{
	if (wm->pid)
		kill(wm->pid, SIGHUP);
	else {
		XEvent ev;

		ev.xclient.type = ClientMessage;
		ev.xclient.display = dpy;
		ev.xclient.window = root;
		ev.xclient.message_type = XInternAtom(dpy, "_NET_RELOAD", False);
		ev.xclient.data.l[0] = 0;
		ev.xclient.data.l[1] = 0;
		ev.xclient.data.l[2] = 0;
		ev.xclient.data.l[3] = 0;
		ev.xclient.data.l[4] = 0;
		XSendEvent(dpy, root, False,
			   SubstructureNotifyMask | SubstructureRedirectMask, &ev);
		XFlush(dpy);
	}
}

static void
to_file_ADWM(char *stylerc, char *stylefile)
{
	FILE *f;

	if (!(f = fopen(stylerc, "w"))) {
		EPRINTF("%s: %s\n", stylerc, strerror(errno));
		return;
	}
	fprintf(f, "#include \"%s\"\n", stylefile);
	fclose(f);
}

static void
set_style_ADWM()
{
	return xde_set_style_simple("stylerc", &to_file_ADWM);
}

static void
list_dir_ADWM(char *xdir, char *style, enum ListType type)
{
	return xde_list_dir_simple(xdir, "styles", "/stylerc", "", style, type);
}

static void
list_styles_ADWM()
{
	return xde_list_styles_simple();
}

static void
gen_item_ADWM(char *style, enum ListType type, char *stylename, char *file)
{
}

static void
gen_dir_ADWM(char *xdir, char *style, enum ListType type)
{
}

static void
gen_menu_ADWM()
{
}

static char *
get_icon_ADWM()
{
	return xde_get_icon_simple("adwm");
}

WmOperations xde_wm_ops = {
	.name = "adwm",
	.version = VERSION,
	.get_rcfile = &get_rcfile_ADWM,
	.find_style = &find_style_ADWM,
	.get_style = &get_style_ADWM,
	.set_style = &set_style_ADWM,
	.reload_style = &reload_style_ADWM,
	.list_dir = &list_dir_ADWM,
	.list_styles = &list_styles_ADWM,
	.get_menu = &get_menu_ADWM,
	.gen_item = &gen_item_ADWM,
	.gen_dir = &gen_dir_ADWM,
	.gen_menu = &gen_menu_ADWM,
	.get_icon = &get_icon_ADWM
};


/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
