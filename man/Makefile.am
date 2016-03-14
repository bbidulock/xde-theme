
xde_man_files = \
	    xde-theme.man \
	    xde-style.man \
	    xde-backdrop.man \
	    xde-actions.man \
	    xde-identify.man \
	    xde-setwm.man \
	    xde-stylemon.man \
	    xde-watch.man

nodist_man1_MANS = $(xde_man_files)
CLEANFILES = $(xde_man_files)

EXTRA_DIST = \
	     xde-theme.pod \
	     xde-style.pod \
	     xde-backdrop.pod \
	     xde-actions.pod \
	     xde-identify.pod \
	     xde-setwm.pod \
	     xde-stylemon.pod \
	     xde-watch.pod

xde-theme.man: MANNAME = XDE-THEME
xde-theme.man: xde-theme.pod

xde-style.man: MANNAME = XDE-STYLE
xde-style.man: xde-style.pod

xde-backdrop.man: MANNAME = XDE-BACKDROP
xde-backdrop.man: xde-backdrop.pod

xde-actions.man: MANNAME = XDE-ACTIONS
xde-actions.man: xde-actions.pod

xde-identify.man: MANNAME = XDE-IDENTIFY
xde-identify.man: xde-identify.pod

xde-setwm.man: MANNAME = XDE-SETWM
xde-setwm.man: xde-setwm.pod

xde-setbg.man: MANNAME = XDE-SETBG
xde-setbg.man: xde-setbg.pod

xde-stylemon.man: MANNAME = XDE-STYLEMON
xde-stylemon.man: xde-stylemon.pod

xde-watch.man: MANNAME = XDE-WATCH
xde-watch.man: xde-watch.pod

SUFFIXES = .man .pod

.pod.man:
	pod2man --center="User Commands" \
		--date="$(date)" \
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

