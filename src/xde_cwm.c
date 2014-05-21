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

/** @name CWM
  */
/** @{ */

static void
get_rcfile_CWM()
{
	return xde_get_rcfile_simple("cwm", ".cwmrc", "-c");
}

static char *
find_style_CWM()
{
	return xde_find_style_simple("styles", "/stylerc", "");
}

static char *
get_menu_CWM()
{
	return xde_get_menu_simple("menu", NULL);
}


/** @brief Get the cwm style.
  *
  * cwm(1) does not have a mechanism for including a file from the rc file;
  * therefore, setting the style consists of symbolically linking the stylerc
  * file to the style file, editting the style elements into the rcfile, and
  * restarting the window manager.
  */
static char *
get_style_CWM()
{
	return xde_get_style_simple("stylerc", NULL);
}

/** @brief Reload cwm style.
  *
  * cwm(1) does not respond to signals.  However, there are key bindings that
  * can be set to reload cwm(1).  We can invoke a reload of cwm(1) by sending a
  * synthetic key press and release to the window manager that it will process
  * as though the key combination has been pressed and released.  The default
  * key combination for restart is CMS-r, for quit CMS-q.
  *
  * Note that we can try this trick with TWM too ...
  */
static void
reload_style_CWM()
{
	XEvent ev;

	ev.xkey.type = KeyPress;
	ev.xkey.display = dpy;
	ev.xkey.window = root;
	ev.xkey.root = root;
	ev.xkey.subwindow = None;
	ev.xkey.time = CurrentTime;
	ev.xkey.x = 0;
	ev.xkey.y = 0;
	ev.xkey.x_root = 0;
	ev.xkey.y_root = 0;
	ev.xkey.state = ControlMask | Mod1Mask | ShiftMask;
	ev.xkey.keycode = XKeysymToKeycode(dpy, XStringToKeysym("r"));
	ev.xkey.same_screen = True;
	XSendEvent(dpy, root, False, SubstructureRedirectMask, &ev);
	XSync(dpy, False);

	ev.xkey.type = KeyRelease;
	XSendEvent(dpy, root, False, SubstructureRedirectMask, &ev);
	XSync(dpy, False);
}

/** @brief Set the cwm style.
  *
  * cwm(1) does not have a mechanism for including a file from the rc file;
  * therefore, setting the style consists of symbolically linking the stylerc
  * file to the style file, editting the style elements into the rcfile, and
  * restarting the window manager.
  */
static void
set_style_CWM()
{
	FILE *f;
	char *buf = NULL, *pos, *end, *tok;
	int bytes = 0, copy = 0, skip = 0, block;
	char *stylefile, *style, *stylerc;
	int len;

	get_rcfile_CWM();
	if (!wm->pid) {
		EPRINTF("%s", "cannot set cwm style without a pid\n");
		goto no_pid;
	}
	if (!(stylefile = find_style_CWM())) {
		EPRINTF("cannot find cwm style '%s'\n", options.style);
		goto no_stylefile;
	}
	if ((style = get_style_CWM()) && !strcmp(style, stylefile))
		goto no_change;
	len = strlen(wm->pdir) + strlen("/stylerc") + 1;
	stylerc = calloc(len, sizeof(*stylerc));
	strcpy(stylerc, wm->pdir);
	strcat(stylerc, "/style");
	unlink(stylerc);
	if (symlink(stylefile, stylerc)) {
		EPRINTF("%s -> %s: %s\n", stylerc, stylefile, strerror(errno));
		goto no_link;
	}
	/* edit settings into cwm rcfile */
	if (!(f = fopen(wm->rcfile, "r"))) {
		EPRINTF("%s :%s\n", wm->rcfile, strerror(errno));
		goto no_rcfile;
	}
	/* read in entire file into buf skipping style elements */
	buf = malloc(BUFSIZ);
	for (pos = buf + bytes; fgets(buf + bytes, BUFSIZ, f);
	     buf = realloc(buf, BUFSIZ + bytes), pos = buf + bytes) {
		tok = pos + strspn(pos, " \t");
		block = (strrchr(pos, '\\') && *(strrchr(pos, '\\') + 1) == '\n') ? 1 : 0;
		if (skip || (!copy && ((len = strcspn(tok, " \t")) &&
				       ((len == strlen("borderwidth")
					 && !strncmp(tok, "borderwidth", len))
					|| (len == strlen("color")
					    && !strncmp(tok, "color", len))
					|| (len == strlen("fontname")
					    && !strncmp(tok, "fontname", len))
					|| (len == strlen("gap")
					    && !strncmp(tok, "gap", len))
					|| (len == strlen("snapdist")
					    && !strncmp(tok, "snapdist", len))
					|| (len == strlen("moveamount")
					    && !strncmp(tok, "moveamount", len))
					|| (len == strlen("sticky")
					    && !strncmp(tok, "sticky", len))
				       )))) {
			skip = block;
		} else {
			copy = block;
			bytes += strlen(pos) + 1;
		}
	}
	/* don't end on a block */
	if (copy || skip) {
		*buf++ = '\n';
		*buf = '\0';
		bytes++;
		copy = skip = 0;
	}
	fclose(f);
	if (!(f = fopen(stylerc, "r"))) {
		EPRINTF("%s :%s\n", stylerc, strerror(errno));
		goto no_stylerc;
	}
	/* append style file */
	for (; fgets(buf + bytes, BUFSIZ, f);
	     buf = realloc(buf, BUFSIZ + bytes), pos = buf + bytes) {
		bytes += strlen(pos) + 1;
	}
	fclose(f);
	if (options.dryrun) {
	} else {
		if (!(f = fopen(wm->rcfile, "w"))) {
			EPRINTF("%s :%s\n", wm->rcfile, strerror(errno));
			goto no_stylerc;
		}
		/* write entire buffer back out */
		for (pos = buf, end = buf + bytes; pos < end; pos += strlen(pos) + 1)
			fprintf(f, "%s", pos);
		fclose(f);
		if (options.reload)
			reload_style_CWM();
	}
      no_stylerc:
	free(buf);
      no_rcfile:
      no_link:
	free(stylerc);
      no_change:
	free(stylefile);
      no_stylefile:
      no_pid:
	return;
}

static void
list_dir_CWM(char *xdir, char *style, enum ListType type)
{
	return xde_list_dir_simple(xdir, "styles", "/stylerc", "", style, type);
}

static void
list_styles_CWM()
{
	return xde_list_styles_simple();
}

static void
gen_item_CWM(char *style, enum ListType type, char *stylename, char *file)
{
}

static void
gen_dir_CWM(char *xdir, char *style, enum ListType type)
{
}

static void
gen_menu_CWM()
{
}

WmOperations xde_wm_ops = {
	.name = "cwm",
	.version = VERSION,
	.get_rcfile = &get_rcfile_CWM,
	.find_style = &find_style_CWM,
	.get_style = &get_style_CWM,
	.set_style = &set_style_CWM,
	.reload_style = &reload_style_CWM,
	.list_dir = &list_dir_CWM,
	.list_styles = &list_styles_CWM,
	.get_menu = &get_menu_CWM,
	.gen_item = &gen_item_CWM,
	.gen_dir = &gen_dir_CWM,
	.gen_menu = &gen_menu_CWM
};

/** @} */

// vim: set sw=8 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
