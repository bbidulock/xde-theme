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

/** @name MATWM2
  */
/** @{ */

/** @brief Find the matwm2 rc file and default directory.
  *
  * matwm2 always loads /etc/matwmrc and then $HOME/.matwmrc if they exist.  It
  * uses internal defaults otherwise.  There is no way of specifying the
  * configuration file on the command line.
  */
static void
get_rcfile_MATWM2()
{
	return xde_get_rcfile_simple("matwm2", ".matwmrc", "-rc");
}

static char *
find_style_MATWM2()
{
	return xde_find_style_simple("styles", "/stylerc", "");
}

static char *
get_menu_MATWM2()
{
	/* NOTE: matwm2(1) does not have a root menu of its own */
	get_rcfile_MATWM2();
	return NULL;
}

static char *
get_style_MATWM2()
{
	return xde_get_style_simple("stylerc", NULL);
}

static void
set_style_MATWM2()
{
	return xde_set_style_simple("stylerc", NULL);
}

/** @brief Reload the matwm2 style.
  *
  * If matwm2 receives a SIGUSR1 signal, it will reload all configurations
  * files.
  */
static void
reload_style_MATWM2()
{
	if (wm->pid)
		kill(wm->pid, SIGUSR1);
	else
		EPRINTF("%s", "cannot reload matwm2 without a pid\n");
}

static void
list_dir_MATWM2(char *xdir, char *style, enum ListType type)
{
	return xde_list_dir_simple(xdir, "styles", "/stylerc", "", style, type);
}

static void
list_styles_MATWM2()
{
	return xde_list_styles_simple();
}

WmOperations xde_wm_ops = {
	"matwm2",
	VERSION,
	&get_rcfile_MATWM2,
	&find_style_MATWM2,
	&get_style_MATWM2,
	&set_style_MATWM2,
	&reload_style_MATWM2,
	&list_dir_MATWM2,
	&list_styles_MATWM2,
	&get_menu_MATWM2
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
