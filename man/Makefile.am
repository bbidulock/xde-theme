
xde_man_files = \
	    xde-theme.man \
	    xde-style.man \
	    xde-backdrop.man \
	    xde-actions.man \
	    xde-menu.man

nodist_man1_MANS = $(xde_man_files)
CLEANFILES = $(xde_man_files)

EXTRA_DIST = \
	     xde-theme.pod \
	     xde-style.pod \
	     xde-backdrop.pod \
	     xde-actions.pod \
	     xde-menu.pod

xde-theme.man: xde-theme.pod

xde-style.man: xde-style.pod

xde-backdrop.man: xde-backdrop.pod

xde-actions.man: xde-actions.pod

xde-menu.man: xde-menu.pod

SUFFIXES = .man .pod

.pod.man:
	pod2man --center="User Commands" \
		--date="$(DATE)" \
		--errors="none" \
		--fixed="CW" \
		--fixedbold="CB" \
		--fixeditalic="CI" \
		--fixedbolditalic="CX" \
		--name="XDE-THEME" \
		--nourls \
		--release="$(PACKAGE) $(VERSION)" \
		--section="1" \
		$< >$@

