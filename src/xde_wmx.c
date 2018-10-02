/*****************************************************************************

 Copyright (c) 2010-2018  Monavacon Limited <http://www.monavacon.com/>
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

/** @name WMX
  */
/** @{ */

/** @brief Get the wmx(1) rc file.
  *
  * wmx(1) does not have an rc file; however, it does have a user directory:
  * ~/.wmx where a directory based menu is provided.
  */
static void
get_rcfile_WMX()
{
	char *home = xde_get_proc_environ("HOME") ? : ".";
	static char *suffix = "/.wmx";
	char *dir;

	dir = calloc(strlen(home) + strlen(suffix) + 1, sizeof(*dir));
	strcpy(dir, home);
	strcat(dir, suffix);
	free(wm->pdir);
	wm->pdir = dir;
	free(wm->udir);
	wm->udir = strdup(dir);
}

static char *
find_style_WMX()
{
	get_rcfile_WMX();
	return NULL;
}

/** @brief Get the wmx(1) menu.
  *
  * wmx(1) uses a directory tree structure in ~/.wmx to provide a menu (as does
  * flwm(1) and wm2(1)).  * The names of executable files in the directory are used
  * as the name of entries in the menu.  The names of subdirectories are used as
  * the names of submenus.  Therefore, this function always returns ~/.wmx
  * (where ~ is the $HOME environment variable from the window manager).
  */
static char *
get_menu_WMX()
{
	char *home = xde_get_proc_environ("HOME") ? : ".";
	static char *suffix = "/.wmx/";
	char *menu;

	get_rcfile_WMX();
	menu = calloc(strlen(home) + strlen(suffix) + 1, sizeof(*menu));
	strcpy(menu, home);
	strcat(menu, suffix);
	free(wm->menu);
	wm->menu = menu;
	return menu;
}

static char *
get_style_WMX()
{
	get_rcfile_WMX();
	return NULL;
}

static void
set_style_WMX()
{
	char *stylefile;

	if (!(stylefile = find_style_WMX())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		return;
	}
}

static void
reload_style_WMX()
{
}

static void
list_dir_WMX(char *xdir, char *style, enum ListType type)
{
}

static void
list_styles_WMX()
{
}

static void
gen_item_WMX(char *style, enum ListType type, char *stylename, char *file)
{
}

static void
gen_dir_WMX(char *xdir, char *style, enum ListType type)
{
}

static void
gen_menu_WMX()
{
}

static char *
get_icon_WMX()
{
	return xde_get_icon_simple("wmx");
}

WmOperations xde_wm_ops = {
	.name = "wmx",
	.version = VERSION,
	.get_rcfile = &get_rcfile_WMX,
	.find_style = &find_style_WMX,
	.get_style = &get_style_WMX,
	.set_style = &set_style_WMX,
	.reload_style = &reload_style_WMX,
	.list_dir = &list_dir_WMX,
	.list_styles = &list_styles_WMX,
	.get_menu = &get_menu_WMX,
	.gen_item = &gen_item_WMX,
	.gen_dir = &gen_dir_WMX,
	.gen_menu = &gen_menu_WMX,
	.get_icon = &get_icon_WMX
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
