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

/* @name TWM
 */
/** @{ */

static void
get_rcfile_TWM()
{
	return xde_get_rcfile_XTWM("twm");
}

static char *
find_style_TWM()
{
	return xde_find_style_simple("styles", "/stylerc", "");
}

static char *
get_menu_TWM()
{
	return xde_get_menu_simple("menu", NULL);
}

static char *
get_style_TWM()
{
	return xde_get_style_simple("stylerc", NULL);
}

static void
set_style_TWM()
{
	return xde_set_style_simple("stylerc", NULL);
}

/** @brief Reload twm style.
  *
  * twm has no restart mechanism; however, we can invoke a reload of twm(1) by
  * sending a synthetic key press and release to the window manager that it will
  * process as though the key combination has been pressed and released.  The
  * default key combination for restart is CM-r; for quit, CM-Delete.
  */
static void
reload_style_TWM()
{
	XEvent ev;

	ev.xkey.type = KeyPress;
	ev.xkey.display = dpy;
	ev.xkey.window = root;
	ev.xkey.subwindow = None;
	ev.xkey.time = CurrentTime;
	ev.xkey.x = 0;
	ev.xkey.y = 0;
	ev.xkey.x_root = 0;
	ev.xkey.y_root = 0;
	ev.xkey.state = ControlMask | Mod1Mask;
	ev.xkey.keycode = XKeysymToKeycode(dpy, XStringToKeysym("r"));
	ev.xkey.same_screen = True;
	XSendEvent(dpy, root, False, SubstructureRedirectMask, &ev);
	XSync(dpy, False);

	ev.xkey.type = KeyRelease;
	XSendEvent(dpy, root, False, SubstructureRedirectMask, &ev);
	XSync(dpy, False);
}

static void
list_dir_TWM(char *xdir, char *style, enum ListType type)
{
	return xde_list_dir_simple(xdir, "styles", "/stylerc", "", style, type);
}

static void
list_styles_TWM()
{
	return xde_list_styles_simple();
}

static void
gen_dir_TWM(char *xdir, char *style, enum ListType type)
{
}

static void
gen_menu_TWM()
{
}

WmOperations xde_wm_ops = {
	.name = "twm",
	.version = VERSION,
	.get_rcfile = &get_rcfile_TWM,
	.find_style = &find_style_TWM,
	.get_style = &get_style_TWM,
	.set_style = &set_style_TWM,
	.reload_style = &reload_style_TWM,
	.list_dir = &list_dir_TWM,
	.list_styles = &list_styles_TWM,
	.get_menu = &get_menu_TWM,
	.gen_dir = &gen_dir_TWM,
	.gen_menu = &gen_menu_TWM
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
