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

/* blackbox(1) plugin */

#include "xde.h"

/** @name BLACKBOX
  */
/** @{ */

/** @brief Find the blackbox rc file and default directory.
  *
  * Blackbox takes a command such as:
  *
  *   blackbox [-rc RCFILE]
  *
  * When the RCFILE is not specified, ~/.blackboxrc is used.  The locations of
  * other fluxbox configuration files are specified by the initial configuration
  * file, but are typically placed under ~/.blackbox.  System files are placed
  * under /usr/share/blackbox.
  */
static void
get_rcfile_BLACKBOX()
{
	return xde_get_rcfile_simple("blackbox", ".blackboxrc", "-rc");
}

/** @brief Find a blackbox style file from a style name.
  *
  * Blackbox style files are named files in /usr/share/blackbox/styles or
  * ~/.blackbox/styles.
  */
static char *
find_style_BLACKBOX()
{
	return xde_find_style_simple("styles", "/stylerc");
}

static char *
get_style_BLACKBOX()
{
	return xde_get_style_database("session.styleFile", "Session.StyleFile");
}

/** @brief Reload a blackbox style.
  *
  * Sending SIGUSR1 to the blackbox PID provided in _NET_WM_PID property on the
  * _NET_SUPPORTING_WM_CHECK window> will effect the reconfiguration that
  * results in rereading of the style file.
  */
static void
reload_style_BLACKBOX()
{
	if (wm->pid)
		kill(wm->pid, SIGUSR1);
	else
		EPRINTF("%s", "cannot reload blackbox without a pid\n");
}

/** @brief Set the blackbox style.
  *
  * When blackbox changes the style, it writes the path to the new style in the
  * session.styleFile resource in the ~/.blackboxrc file and then reloads the
  * configuration.
  *
  * The session.styleFile entry looks like:
  *
  *   session.styleFile:	/usr/share/blackbox/styles/Airforce
  *
  * Unlike other window managers, it reloads the configuration rather than restarting.
  */
static void
set_style_BLACKBOX()
{
	return xde_set_style_database("session.styleFile");
}

static void
list_dir_BLACKBOX(char *xdir, char *style)
{
	return xde_list_dir_simple(xdir, "styles", "/stylerc", style);
}

static void
list_styles_BLACKBOX()
{
	return xde_list_styles_simple();
}

WmOperations xde_wm_ops = {
	"blackbox",
	&get_rcfile_BLACKBOX,
	&find_style_BLACKBOX,
	&get_style_BLACKBOX,
	&set_style_BLACKBOX,
	&reload_style_BLACKBOX,
	&list_dir_BLACKBOX,
	&list_styles_BLACKBOX
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
