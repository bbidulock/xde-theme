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

/** @name 2BWM
  */
/** @{ */

static void
get_rcfile_2BWM()
{
}

static char *
find_style_2BWM()
{
	get_rcfile_2BWM();
	return NULL;
}

static char *
get_menu_2BWM()
{
	/* NOTE: 2bwm(1) does not have a menu */
	get_rcfile_2BWM();
	return NULL;
}

static char *
get_style_2BWM()
{
	get_rcfile_2BWM();
	return NULL;
}

static void
set_style_2BWM()
{
	char *stylefile;

	if (!(stylefile = find_style_2BWM())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		return;
	}
}

static void
reload_style_2BWM()
{
}

static void
list_dir_2BWM(char *xdir, char *style, enum ListType type)
{
	(void) xdir;
	(void) style;
	(void) type;
}

static void
list_styles_2BWM()
{
}

static void
gen_item_2BWM(char *style, enum ListType type, char *stylename, char *file)
{
	(void) style;
	(void) type;
	(void) stylename;
	(void) file;
}

static void
gen_dir_2BWM(char *xdir, char *style, enum ListType type)
{
	(void) xdir;
	(void) style;
	(void) type;
}

static void
gen_menu_2BWM()
{
}

static char *
get_icon_2BWM()
{
	return xde_get_icon_simple("2bWM");
}


WmOperations xde_wm_ops = {
	.name = "2bwm",
	.version = VERSION,
	.get_rcfile = &get_rcfile_2BWM,
	.find_style = &find_style_2BWM,
	.get_style = &get_style_2BWM,
	.set_style = &set_style_2BWM,
	.reload_style = &reload_style_2BWM,
	.list_dir = &list_dir_2BWM,
	.list_styles = &list_styles_2BWM,
	.get_menu = &get_menu_2BWM,
	.gen_item = &gen_item_2BWM,
	.gen_dir = &gen_dir_2BWM,
	.gen_menu = &gen_menu_2BWM,
	.get_icon = &get_icon_2BWM
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
