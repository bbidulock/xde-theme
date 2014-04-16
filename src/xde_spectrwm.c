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

/** @name SPECTRWM
  */
/** @{ */

static void
get_rcfile_SPECTRWM()
{
}

static char *
find_style_SPECTRWM()
{
	get_rcfile_SPECTRWM();
	return NULL;
}

static char *
get_menu_SPECTRWM()
{
	/* NOTE: spectrwm(1) does not have a root menu */
	get_rcfile_SPECTRWM();
	return NULL;
}

static char *
get_style_SPECTRWM()
{
	get_rcfile_SPECTRWM();
	return NULL;
}

static void
set_style_SPECTRWM()
{
	char *stylefile;

	if (!(stylefile = find_style_SPECTRWM())) {
		EPRINTF("cannot find style '%s'\n", options.style);
		return;
	}
}

static void
reload_style_SPECTRWM()
{
}

static void
list_dir_SPECTRWM(char *xdir, char *style)
{
}

static void
list_styles_SPECTRWM()
{
}

WmOperations xde_wm_ops = {
	"spectrwm",
	&get_rcfile_SPECTRWM,
	&find_style_SPECTRWM,
	&get_style_SPECTRWM,
	&set_style_SPECTRWM,
	&reload_style_SPECTRWM,
	&list_dir_SPECTRWM,
	&list_styles_SPECTRWM,
	&get_menu_SPECTRWM
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
