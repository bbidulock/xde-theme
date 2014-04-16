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

/* jwm(1) plugin */

#include "xde.h"

/** @name JWM
  */
/** @{ */

static void
get_rcfile_JWM()
{
	return xde_get_rcfile_simple("jwm", ".jwmrc", "-rc");
}

/** @brief Find a jwm style from a style name.
  *
  * JWM style files are named files or directories in /usr/share/jwm/styles or
  * ~/.jwm/styles.  When a named directory, the directory must contain a file
  * named style.
  */
static char *
find_style_JWM()
{
	char *style;

	if (!(style = xde_find_style_simple("styles", "/style", "")))
		style = xde_find_style_simple("themes", "/style", "");
	return style;
}

static char *
from_file_JWM(char *stylerc)
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
		if (!(b = strstr(buf, "<Include>")))
			continue;
		if (!(e = strstr(buf, "</Include>")))
			continue;
		b += strlen("<Include>");
		if (b >= e)
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
get_menu_JWM()
{
	return xde_get_menu_simple("root", &from_file_JWM);
}

/** @brief Get the style for jwm.
  *
  * There are two ways to implement the style system for jwm(1): symbolic links
  * or <Include></Include> statements.  Both accept absolute or relative paths.
  * The style file in turn links to or include a style file from the appropriate
  * styles subdirectory.
  *
  * The symbolic link approach is likely best.  Either acheives the same result.
  *
  * NOTE: this only works with the xde-styles style system for jwm(1) and
  * requires that the primary configuration file be that consistent with the
  * xde-styles package.
  */
static char *
get_style_JWM()
{
	return xde_get_style_simple("style", &from_file_JWM);
}

/** @brief Reload a jwm style.
  *
  * JWM can be reloaded or restarted by sending a _JWM_RELOAD or _JWM_RESTART
  * ClientMessage to the root window, or by executing jwm -reload or jwm
  * -restart.
  */
static void
reload_style_JWM()
{
	XEvent ev;

	OPRINTF("%s", "reloading jwm\n");
	ev.xclient.type = ClientMessage;
	ev.xclient.display = dpy;
	ev.xclient.window = root;
	ev.xclient.message_type = XInternAtom(dpy, "_JWM_RESTART", False);
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = 0;
	ev.xclient.data.l[1] = 0;
	ev.xclient.data.l[2] = 0;
	ev.xclient.data.l[3] = 0;
	ev.xclient.data.l[4] = 0;
	XSendEvent(dpy, root, False, SubstructureRedirectMask, &ev);
	XSync(dpy, False);
}

static void
to_file_JWM(char *stylerc, char *stylefile)
{
	FILE *f;

	if (!(f = fopen(stylerc, "w"))) {
		EPRINTF("%s: %s\n", stylerc, strerror(errno));
		return;
	}
	fprintf(f, "<?xml version=\"1.0\"?>\n");
	fprintf(f, "<JWM>\n");
	fprintf(f, "   <Include>%s</Include>\n", stylefile);
	fprintf(f, "</JWM>\n");
	fclose(f);
}

/** @brief Set the jwm style.
  *
  * When jwm changes its style (the way we have it set up), it writes ~/.jwm/style to
  * include a new file and restarts.  The jwm style file, ~/.jwm/style looks like:
  *
  * <?xml version="1.0"?>
  * <JWM>
  *    <Include>/usr/share/jwm/styles/Squared-blue</Include>
  * </JWM>
  *
  * The last component of the path is the theme name.  System styles are located in
  * /usr/share/jwm/styles; user styles are located in ~/.jwm/styles.
  *
  * xde-session sets the environment variable JWM_CONFIG_FILE to point to the primary
  * configuration file; JWM_CONFIG_DIR to point to the system configuration directory
  * (default /usr/share/jwm); JWM_CONFIG_HOME to point to the user configuration
  * directory (default ~/.jwm but set under an xde-session to ~/.config/jwm).
  *
  * Note that older versions of jwm(1) do not provide tilde expansion in
  * configuration files.
  */
static void
set_style_JWM()
{
	return xde_set_style_simple("style", &to_file_JWM);
}

static void
list_dir_JWM(char *xdir, char *style)
{
	xde_list_dir_simple(xdir, "styles", "/style", "", style);
	xde_list_dir_simple(xdir, "themes", "/style", "", style);
}

static void
list_styles_JWM()
{
	return xde_list_styles_simple();
}

WmOperations xde_wm_ops = {
	"jwm",
	&get_rcfile_JWM,
	&find_style_JWM,
	&get_style_JWM,
	&set_style_JWM,
	&reload_style_JWM,
	&list_dir_JWM,
	&list_styles_JWM,
	&get_menu_JWM
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
