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

/* fluxbox(1) plugin */

#include "xde.h"

/** @name FLUXBOX
  */
/** @{ */

/** @brief find the fluxbox init file and default directory.
  *
  * Fluxbox takes a command such as:
  *
  *   fluxbox [-rc RCFILE]
  *
  * When the RCFILE is not specified, ~/.fluxbox/init is used.  The locations of
  * other fluxbox configuration files are specified by the intitial
  * configuration file, but are typically placed under ~/.fluxbox.  System files
  * are placed under /usr/share/fluxbox.
  *
  * System styles are either in /usr/share/fluxbox/styles or in
  * ~/.fluxbox/styles.  Styles under these directories can either be a file or a
  * directory.  When a directory, the actual style file is in a file called
  * theme.cfg.
  */
static void
get_rcfile_FLUXBOX()
{
	return xde_get_rcfile_simple("fluxbox", ".fluxbox/init", "-rc");
}

/** @brief Find a fluxbox style file from a style name.
  *
  * Fluxbox style files are named files or directories in
  * /usr/share/fluxbox/styles or ~/.fluxbox/styles.  When a named directory,
  * the directory must contain a file named theme.cfg.
  */
static char *
find_style_FLUXBOX()
{
	char *style, *p;

	style = xde_find_style_simple("styles", "/theme.cfg", "");
	if ((p = strstr(style, "/theme.cfg")))
		*p = '\0';
	return (style);
}

/** @brief Get the current menu file.
  *
  * The current menu file is set in the session.menuFile resources in the rc
  * file.
  */
static char *
get_menu_FLUXBOX()
{
	return xde_get_menu_database("session.menuFile", "Session.MenuFile");
}

/** @brief Get the current fluxbox style.
  *
  * The current fluxbox style is set in the session.styleFile resource in the rc
  * file.
  */
static char *
get_style_FLUXBOX()
{
	return xde_get_style_database("session.styleFile", "Session.StyleFile");
}

/** @brief Reload a fluxbox style.
  *
  * Sending SIGUSR2 to the fluxbox PID provided in the _BLACKBOX_PID property on
  * the root window will result in a reconfigure of fluxbox (which is what
  * fluxbox itself does when changing styles); send SIGHUP, a restart.
  *
  * NOTE WELL: Fluxbox changed its behaviour after release 1.3.5.
  *
  * Before:
  *   SIGHUP  - restart
  *   SIGUSR1 - reload configuration
  *   SIGUSR2 - reload menu file
  *
  * After:
  *   SIGHUP  - exit
  *   SIGUSR1 - restart
  *   SIGUSR2 - reload configuration
  *
  * Note also the following:
  *   SIGINT  - shutdown
  *   SIGTERM - shutdown
  *
  * The only single signal that is consistent across version is SIGUSR1 which
  * will reload in the before case and restart in the after case.
  *
  */
static void
reload_style_FLUXBOX()
{
	if (wm->pid)
		kill(wm->pid, SIGUSR1);
	else
		EPRINTF("%s", "cannot reload fluxbox without a pid\n");

}

/** @brief Set the fluxbox style.
  *
  * When fluxbox changes the style, it writes the path to the new style in the
  * session.StyleFile resource in the rc file (default ~/.fluxbox/init) and then
  * reloads the configuration.  The session.styleFile entry looks like:
  *
  *   session.styleFile: /usr/share/fluxbox/styles/Airforce
  *
  * Unlike other window managers, it reloads the configuration rather than
  * restarting.  However, fluxbox has the problem that simply reloading the
  * configuration does not result in a change to the menu styles (in particular
  * the font color), so a restart is likely required.
  *
  * Note that when fluxbox restarts, it does not change the
  * _NET_SUPPORTING_WM_CHECK root window property but it does change the
  * _BLACKBOX_PID root window property, even if it is just to replace it with
  * the same value again.
  */
static void
set_style_FLUXBOX()
{
	return xde_set_style_database("session.styleFile");
}

static void
list_dir_FLUXBOX(char *xdir, char *style, enum ListType type)
{
	return xde_list_dir_simple(xdir, "styles", "/theme.cfg", "", style, type);
}

static void
list_styles_FLUXBOX()
{
	return xde_list_styles_simple();
}

static void
gen_item_FLUXBOX(char *style, enum ListType type, char *stylename, char *file)
{
	switch (type) {
	case XDE_LIST_PRIVATE:
	case XDE_LIST_USER:
		fprintf(stdout, "  [exec] (%s) {xde-style -s -t -r -u '%s'}\n",
				stylename, stylename);
		break;
	case XDE_LIST_SYSTEM:
	case XDE_LIST_GLOBAL:
		fprintf(stdout, "  [exec] (%s) {xde-style -s -t -r -y '%s'}\n",
				stylename, stylename);
		break;
	case XDE_LIST_MIXED:
		fprintf(stdout, "  [exec] (%s) {xde-style -s -t -r '%s'}\n",
				stylename, stylename);
		break;
	}
}

static void
gen_dir_FLUXBOX(char *xdir, char *style, enum ListType type)
{
	xde_gen_dir_simple(xdir, "styles", "/theme.cfg", "", style, type);
}

static void
gen_menu_FLUXBOX()
{
	const char *which;

	if (options.system && !options.user) {
		which = "System ";
	} else if (!options.system && options.user) {
		which = "User ";
	} else {
		which = "";
	}
	if (options.theme) {
		fprintf(stdout, "[submenu] (%sThemes) {Choose a theme...}\n", which);
		xde_gen_menu_simple();
		fprintf(stdout, "[end]\n");
	} else {
		fprintf(stdout, "[submenu] (%sStyles) {Choose a style...}\n", which);
		xde_gen_menu_simple();
		fprintf(stdout, "[end]\n");
	}
}

static char *
get_icon_FLUXBOX()
{
	return xde_get_icon_simple("fluxbox");
}

WmOperations xde_wm_ops = {
	.name = "fluxbox",
	.version = VERSION,
	.get_rcfile = &get_rcfile_FLUXBOX,
	.find_style = &find_style_FLUXBOX,
	.get_style = &get_style_FLUXBOX,
	.set_style = &set_style_FLUXBOX,
	.reload_style = &reload_style_FLUXBOX,
	.list_dir = &list_dir_FLUXBOX,
	.list_styles = &list_styles_FLUXBOX,
	.get_menu = &get_menu_FLUXBOX,
	.gen_item = &gen_item_FLUXBOX,
	.gen_dir = &gen_dir_FLUXBOX,
	.gen_menu = &gen_menu_FLUXBOX,
	.get_icon = &get_icon_FLUXBOX
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
