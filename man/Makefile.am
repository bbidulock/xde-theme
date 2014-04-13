
xde_man_files = \
	    xde-theme.man \
	    xde-style.man \
	    xde-backdrop.man \
	    xde-actions.man \
	    xde-menu.man \
	    xde-identify.man

nodist_man1_MANS = $(xde_man_files)
CLEANFILES = $(xde_man_files)

EXTRA_DIST = \
	     xde-theme.pod \
	     xde-style.pod \
	     xde-backdrop.pod \
	     xde-actions.pod \
	     xde-menu.pod \
	     xde-identify.pod

xde-theme.man: MANNAME = XDE-THEME
xde-theme.man: xde-theme.pod

xde-style.man: MANNAME = XDE-STYLE
xde-style.man: xde-style.pod

xde-backdrop.man: MANNAME = XDE-BACKDROP
xde-backdrop.man: xde-backdrop.pod

xde-actions.man: MANNAME = XDE-ACTIONS
xde-actions.man: xde-actions.pod

xde-menu.man: MANNAME = XDE-MENU
xde-menu.man: xde-menu.pod

xde-identify.man: MANNAME = XDE-IDENTIFY
xde-identify.man: xde-identify.pod

SUFFIXES = .man .pod

.pod.man:
	pod2man --center="User Commands" \
		--date="$(DATE)" \
		--errors="none" \
		--fixed="CW" \
		--fixedbold="CB" \
		--fixeditalic="CI" \
		--fixedbolditalic="CX" \
		--name="$(MANNAME)" \
		--nourls \
		--release="$(PACKAGE) $(VERSION)" \
		--section="1" \
		$< >$@

