
nodist_man1_MANS = \
	    xde-theme.man \
	    xde-style.man \
	    xde-backdrop.man \
	    xde-actions.man \
	    xde-menu.man

xde-theme.man: xde-theme.pod

xde-style.man: xde-theme.pod

xde-backdrop.man: xde-backdrop.pod

xde-actions.man: xde-backdrop.pod

xde-menu.man: xde-backdrop.pod

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

