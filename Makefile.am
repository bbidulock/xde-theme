
SUBDIRS =

AM_CPPFLAGS = -include autoconf.h -DNAME=\"xde-theme\"
AM_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)

bin_PROGRAMS = xde-theme xde-style

xde_theme_SOURCES = xde-theme.c
xde_theme_LDADD = $(GDK_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(X11_LIBS)

xde_style_SOURCES = xde-style.c
xde_style_LDADD =  $(X11_LIBS)

man1_MANS = \
	    xde-theme.man

xde-theme.man: xde-theme.pod
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

EXTRA_DIST = LICENSE README TODO ChangeLog xde-theme.pod

nobase_dist_share_DATA =
nobase_nodist_share_DATA =
nobase_dist_share_SCRIPTS =
nobase_nodist_share_SCRIPTS =

nobase_dist_share_DATA += \
	fluxbox/styles/Squared-cadet/theme.cfg

nobase_dist_share_DATA += \
	fluxbox/styles/Squared-cadet/pixmaps/close_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/close_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/close.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/hilite_select.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/hilite_unselect.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/lhalf_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/lhalf_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/lhalf.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/max_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/max_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/max.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/menu_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/menu_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/menu.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/min_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/min_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/min.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/rhalf_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/rhalf_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/rhalf.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/select.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/shade_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/shade_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/shade.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/stick_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/stick_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/stick.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/stuck_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/stuck_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/stuck.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/toolbar_focused.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/toolbar_unfocused.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/unselect.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/unshade_pressed.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/unshade_unfocus.xpm \
	fluxbox/styles/Squared-cadet/pixmaps/unshade.xpm

nobase_dist_share_DATA += \
	fluxbox/styles/Squared-grey/theme.cfg

nobase_dist_share_DATA += \
	fluxbox/styles/Squared-grey/pixmaps/close_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/close_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/close.xpm \
	fluxbox/styles/Squared-grey/pixmaps/hilite_select.xpm \
	fluxbox/styles/Squared-grey/pixmaps/hilite_unselect.xpm \
	fluxbox/styles/Squared-grey/pixmaps/lhalf_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/lhalf_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/lhalf.xpm \
	fluxbox/styles/Squared-grey/pixmaps/max_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/max_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/max.xpm \
	fluxbox/styles/Squared-grey/pixmaps/menu_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/menu_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/menu.xpm \
	fluxbox/styles/Squared-grey/pixmaps/min_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/min_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/min.xpm \
	fluxbox/styles/Squared-grey/pixmaps/rhalf_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/rhalf_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/rhalf.xpm \
	fluxbox/styles/Squared-grey/pixmaps/select.xpm \
	fluxbox/styles/Squared-grey/pixmaps/shade_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/shade_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/shade.xpm \
	fluxbox/styles/Squared-grey/pixmaps/stick_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/stick_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/stick.xpm \
	fluxbox/styles/Squared-grey/pixmaps/stuck_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/stuck_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/stuck.xpm \
	fluxbox/styles/Squared-grey/pixmaps/toolbar_focused.xpm \
	fluxbox/styles/Squared-grey/pixmaps/toolbar_unfocused.xpm \
	fluxbox/styles/Squared-grey/pixmaps/unselect.xpm \
	fluxbox/styles/Squared-grey/pixmaps/unshade_pressed.xpm \
	fluxbox/styles/Squared-grey/pixmaps/unshade_unfocus.xpm \
	fluxbox/styles/Squared-grey/pixmaps/unshade.xpm

nobase_dist_share_DATA += \
	fluxbox/styles/Squared-green/theme.cfg

nobase_dist_share_DATA += \
	fluxbox/styles/Squared-green/pixmaps/close_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/close_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/close.xpm \
	fluxbox/styles/Squared-green/pixmaps/hilite_select.xpm \
	fluxbox/styles/Squared-green/pixmaps/hilite_unselect.xpm \
	fluxbox/styles/Squared-green/pixmaps/lhalf_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/lhalf_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/lhalf.xpm \
	fluxbox/styles/Squared-green/pixmaps/max_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/max_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/max.xpm \
	fluxbox/styles/Squared-green/pixmaps/menu_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/menu_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/menu.xpm \
	fluxbox/styles/Squared-green/pixmaps/min_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/min_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/min.xpm \
	fluxbox/styles/Squared-green/pixmaps/rhalf_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/rhalf_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/rhalf.xpm \
	fluxbox/styles/Squared-green/pixmaps/select.xpm \
	fluxbox/styles/Squared-green/pixmaps/shade_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/shade_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/shade.xpm \
	fluxbox/styles/Squared-green/pixmaps/stick_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/stick_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/stick.xpm \
	fluxbox/styles/Squared-green/pixmaps/stuck_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/stuck_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/stuck.xpm \
	fluxbox/styles/Squared-green/pixmaps/toolbar_focused.xpm \
	fluxbox/styles/Squared-green/pixmaps/toolbar_unfocused.xpm \
	fluxbox/styles/Squared-green/pixmaps/unselect.xpm \
	fluxbox/styles/Squared-green/pixmaps/unshade_pressed.xpm \
	fluxbox/styles/Squared-green/pixmaps/unshade_unfocus.xpm \
	fluxbox/styles/Squared-green/pixmaps/unshade.xpm

nobase_dist_share_DATA += \
	fluxbox/styles/Squared-blue/theme.cfg

nobase_dist_share_DATA += \
	fluxbox/styles/Squared-blue/pixmaps/close_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/close_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/close.xpm \
	fluxbox/styles/Squared-blue/pixmaps/hilite_select.xpm \
	fluxbox/styles/Squared-blue/pixmaps/hilite_unselect.xpm \
	fluxbox/styles/Squared-blue/pixmaps/lhalf_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/lhalf_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/lhalf.xpm \
	fluxbox/styles/Squared-blue/pixmaps/max_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/max_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/max.xpm \
	fluxbox/styles/Squared-blue/pixmaps/menu_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/menu_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/menu.xpm \
	fluxbox/styles/Squared-blue/pixmaps/min_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/min_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/min.xpm \
	fluxbox/styles/Squared-blue/pixmaps/rhalf_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/rhalf_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/rhalf.xpm \
	fluxbox/styles/Squared-blue/pixmaps/select.xpm \
	fluxbox/styles/Squared-blue/pixmaps/shade_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/shade_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/shade.xpm \
	fluxbox/styles/Squared-blue/pixmaps/stick_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/stick_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/stick.xpm \
	fluxbox/styles/Squared-blue/pixmaps/stuck_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/stuck_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/stuck.xpm \
	fluxbox/styles/Squared-blue/pixmaps/toolbar_focused.xpm \
	fluxbox/styles/Squared-blue/pixmaps/toolbar_unfocused.xpm \
	fluxbox/styles/Squared-blue/pixmaps/unselect.xpm \
	fluxbox/styles/Squared-blue/pixmaps/unshade_pressed.xpm \
	fluxbox/styles/Squared-blue/pixmaps/unshade_unfocus.xpm \
	fluxbox/styles/Squared-blue/pixmaps/unshade.xpm

nobase_dist_share_DATA += \
	blackbox/styles/Squared-cadet \
	blackbox/styles/Squared-grey \
	blackbox/styles/Squared-green \
	blackbox/styles/Squared-blue

nobase_dist_share_DATA += \
	icewm/themes/Squared-cadet/closeA.xpm \
	icewm/themes/Squared-cadet/closeI.xpm \
	icewm/themes/Squared-cadet/default.theme \
	icewm/themes/Squared-cadet/depthA.xpm \
	icewm/themes/Squared-cadet/depthI.xpm \
	icewm/themes/Squared-cadet/hideA.xpm \
	icewm/themes/Squared-cadet/hideI.xpm \
	icewm/themes/Squared-cadet/maximizeA.xpm \
	icewm/themes/Squared-cadet/maximizeI.xpm \
	icewm/themes/Squared-cadet/menuButtonA.xpm \
	icewm/themes/Squared-cadet/menuButtonI.xpm \
	icewm/themes/Squared-cadet/minimizeA.xpm \
	icewm/themes/Squared-cadet/minimizeI.xpm \
	icewm/themes/Squared-cadet/restoreA.xpm \
	icewm/themes/Squared-cadet/restoreI.xpm \
	icewm/themes/Squared-cadet/rolldownA.xpm \
	icewm/themes/Squared-cadet/rolldownI.xpm \
	icewm/themes/Squared-cadet/rollupA.xpm \
	icewm/themes/Squared-cadet/rollupI.xpm

nobase_dist_share_DATA += \
	icewm/themes/Squared-cadet/taskbar/desktop.xpm \
	icewm/themes/Squared-cadet/taskbar/start.xpm \
	icewm/themes/Squared-cadet/taskbar/windows.xpm

nobase_dist_share_DATA += \
	icewm/themes/Squared-grey/closeA.xpm \
	icewm/themes/Squared-grey/closeI.xpm \
	icewm/themes/Squared-grey/default.theme \
	icewm/themes/Squared-grey/depthA.xpm \
	icewm/themes/Squared-grey/depthI.xpm \
	icewm/themes/Squared-grey/hideA.xpm \
	icewm/themes/Squared-grey/hideI.xpm \
	icewm/themes/Squared-grey/maximizeA.xpm \
	icewm/themes/Squared-grey/maximizeI.xpm \
	icewm/themes/Squared-grey/menuButtonA.xpm \
	icewm/themes/Squared-grey/menuButtonI.xpm \
	icewm/themes/Squared-grey/minimizeA.xpm \
	icewm/themes/Squared-grey/minimizeI.xpm \
	icewm/themes/Squared-grey/restoreA.xpm \
	icewm/themes/Squared-grey/restoreI.xpm \
	icewm/themes/Squared-grey/rolldownA.xpm \
	icewm/themes/Squared-grey/rolldownI.xpm \
	icewm/themes/Squared-grey/rollupA.xpm \
	icewm/themes/Squared-grey/rollupI.xpm

nobase_dist_share_DATA += \
	icewm/themes/Squared-grey/taskbar/desktop.xpm \
	icewm/themes/Squared-grey/taskbar/start.xpm \
	icewm/themes/Squared-grey/taskbar/windows.xpm

nobase_dist_share_DATA += \
	icewm/themes/Squared-green/closeA.xpm \
	icewm/themes/Squared-green/closeI.xpm \
	icewm/themes/Squared-green/default.theme \
	icewm/themes/Squared-green/depthA.xpm \
	icewm/themes/Squared-green/depthI.xpm \
	icewm/themes/Squared-green/hideA.xpm \
	icewm/themes/Squared-green/hideI.xpm \
	icewm/themes/Squared-green/maximizeA.xpm \
	icewm/themes/Squared-green/maximizeI.xpm \
	icewm/themes/Squared-green/menuButtonA.xpm \
	icewm/themes/Squared-green/menuButtonI.xpm \
	icewm/themes/Squared-green/minimizeA.xpm \
	icewm/themes/Squared-green/minimizeI.xpm \
	icewm/themes/Squared-green/restoreA.xpm \
	icewm/themes/Squared-green/restoreI.xpm \
	icewm/themes/Squared-green/rolldownA.xpm \
	icewm/themes/Squared-green/rolldownI.xpm \
	icewm/themes/Squared-green/rollupA.xpm \
	icewm/themes/Squared-green/rollupI.xpm

nobase_dist_share_DATA += \
	icewm/themes/Squared-green/taskbar/desktop.xpm \
	icewm/themes/Squared-green/taskbar/start.xpm \
	icewm/themes/Squared-green/taskbar/windows.xpm

nobase_dist_share_DATA += \
	icewm/themes/Squared-blue/closeA.xpm \
	icewm/themes/Squared-blue/closeI.xpm \
	icewm/themes/Squared-blue/default.theme \
	icewm/themes/Squared-blue/depthA.xpm \
	icewm/themes/Squared-blue/depthI.xpm \
	icewm/themes/Squared-blue/hideA.xpm \
	icewm/themes/Squared-blue/hideI.xpm \
	icewm/themes/Squared-blue/maximizeA.xpm \
	icewm/themes/Squared-blue/maximizeI.xpm \
	icewm/themes/Squared-blue/menuButtonA.xpm \
	icewm/themes/Squared-blue/menuButtonI.xpm \
	icewm/themes/Squared-blue/minimizeA.xpm \
	icewm/themes/Squared-blue/minimizeI.xpm \
	icewm/themes/Squared-blue/restoreA.xpm \
	icewm/themes/Squared-blue/restoreI.xpm \
	icewm/themes/Squared-blue/rolldownA.xpm \
	icewm/themes/Squared-blue/rolldownI.xpm \
	icewm/themes/Squared-blue/rollupA.xpm \
	icewm/themes/Squared-blue/rollupI.xpm

nobase_dist_share_DATA += \
	icewm/themes/Squared-blue/taskbar/desktop.xpm \
	icewm/themes/Squared-blue/taskbar/start.xpm \
	icewm/themes/Squared-blue/taskbar/windows.xpm


SUBDIRS += openbox

nobase_dist_share_DATA += \
	WindowMaker/Styles/Squared-cadet.style \
	WindowMaker/Styles/Squared-grey.style \
	WindowMaker/Styles/Squared-green.style \
	WindowMaker/Styles/Squared-blue.style

#nobase_dist_share_DATA += \
#	WindowMaker/Themes/Squared-cadet.themed/style \
#	WindowMaker/Themes/Squared-cadet.themed/scback.xpm \
#	WindowMaker/Themes/Squared-cadet.themed/scicon.xpm \
#	WindowMaker/Themes/Squared-cadet.themed/sctbar.xpm \
#	WindowMaker/Themes/Squared-cadet.themed/sctile.xpm
#
#nobase_dist_share_DATA += \
#	WindowMaker/Themes/Squared-grey.themed/style \
#	WindowMaker/Themes/Squared-grey.themed/syback.xpm \
#	WindowMaker/Themes/Squared-grey.themed/syicon.xpm \
#	WindowMaker/Themes/Squared-grey.themed/sytbar.xpm \
#	WindowMaker/Themes/Squared-grey.themed/sytile.xpm
#
#nobase_dist_share_DATA += \
#	WindowMaker/Themes/Squared-green.themed/style \
#	WindowMaker/Themes/Squared-green.themed/sgback.xpm \
#	WindowMaker/Themes/Squared-green.themed/sgicon.xpm \
#	WindowMaker/Themes/Squared-green.themed/sgtbar.xpm \
#	WindowMaker/Themes/Squared-green.themed/sgtile.xpm
#
#nobase_dist_share_DATA += \
#	WindowMaker/Themes/Squared-blue.themed/style \
#	WindowMaker/Themes/Squared-blue.themed/sbback.xpm \
#	WindowMaker/Themes/Squared-blue.themed/sbicon.xpm \
#	WindowMaker/Themes/Squared-blue.themed/sbtbar.xpm \
#	WindowMaker/Themes/Squared-blue.themed/sbtile.xpm

nobase_dist_share_DATA += \
	WindowMaker/Themes/Squared-cadet.themed/scback.xpm \
	WindowMaker/Themes/Squared-cadet.themed/scicon.xpm \
	WindowMaker/Themes/Squared-cadet.themed/sctbar.xpm \
	WindowMaker/Themes/Squared-cadet.themed/sctile.xpm \
	WindowMaker/Themes/Squared-grey.themed/syback.xpm \
	WindowMaker/Themes/Squared-grey.themed/syicon.xpm \
	WindowMaker/Themes/Squared-grey.themed/sytbar.xpm \
	WindowMaker/Themes/Squared-grey.themed/sytile.xpm \
	WindowMaker/Themes/Squared-green.themed/sgback.xpm \
	WindowMaker/Themes/Squared-green.themed/sgicon.xpm \
	WindowMaker/Themes/Squared-green.themed/sgtbar.xpm \
	WindowMaker/Themes/Squared-green.themed/sgtile.xpm \
	WindowMaker/Themes/Squared-blue.themed/sbback.xpm \
	WindowMaker/Themes/Squared-blue.themed/sbicon.xpm \
	WindowMaker/Themes/Squared-blue.themed/sbtbar.xpm \
	WindowMaker/Themes/Squared-blue.themed/sbtile.xpm


SUBDIRS += gtk

nobase_dist_share_SCRIPTS += \
	jwm/setstyle

nobase_nodist_share_SCRIPTS += \
	jwm/config \
	jwm/getstyles \
	jwm/start

nobase_dist_share_DATA += \
	jwm/defaults \
	jwm/desktop \
	jwm/groups \
	jwm/keys \
	jwm/menu \
	jwm/paths \
	jwm/rc \
	jwm/root \
	jwm/style \
	jwm/tray

nobase_dist_share_DATA += \
	jwm/icons/window-above.xpm \
	jwm/icons/window-below.xpm \
	jwm/icons/window-border.xpm \
	jwm/icons/window-close.xpm \
	jwm/icons/window-command.xpm \
	jwm/icons/window-focus.xpm \
	jwm/icons/window-kill.xpm \
	jwm/icons/window-lower.xpm \
	jwm/icons/window-maximize-b.xpm \
	jwm/icons/window-maximize-h.xpm \
	jwm/icons/window-maximize-v.xpm \
	jwm/icons/window-maximize.xpm \
	jwm/icons/window-menu.xpm \
	jwm/icons/window-minimize.xpm \
	jwm/icons/window-move.xpm \
	jwm/icons/window-raise.xpm \
	jwm/icons/window-resize.xpm \
	jwm/icons/window-sendto.xpm \
	jwm/icons/window-shade.xpm \
	jwm/icons/window-snap.xpm \
	jwm/icons/window-stick.xpm \
	jwm/icons/window-titlebar.xpm \
	jwm/icons/window-unshade.xpm

nobase_dist_share_DATA += \
	jwm/styles/Squared-cadet/button-close.xpm \
	jwm/styles/Squared-cadet/button-max-active.xpm \
	jwm/styles/Squared-cadet/button-max.xpm \
	jwm/styles/Squared-cadet/button-min.xpm \
	jwm/styles/Squared-cadet/style

nobase_dist_share_DATA += \
	jwm/styles/Squared-grey/button-close.xpm \
	jwm/styles/Squared-grey/button-max-active.xpm \
	jwm/styles/Squared-grey/button-max.xpm \
	jwm/styles/Squared-grey/button-min.xpm \
	jwm/styles/Squared-grey/style

nobase_dist_share_DATA += \
	jwm/styles/Squared-green/button-close.xpm \
	jwm/styles/Squared-green/button-max-active.xpm \
	jwm/styles/Squared-green/button-max.xpm \
	jwm/styles/Squared-green/button-min.xpm \
	jwm/styles/Squared-green/style

nobase_dist_share_DATA += \
	jwm/styles/Squared-blue/button-close.xpm \
	jwm/styles/Squared-blue/button-max-active.xpm \
	jwm/styles/Squared-blue/button-max.xpm \
	jwm/styles/Squared-blue/button-min.xpm \
	jwm/styles/Squared-blue/style

nobase_dist_share_DATA += \
	pekwm/icons/window-above.xpm \
	pekwm/icons/window-below.xpm \
	pekwm/icons/window-border.xpm \
	pekwm/icons/window-close.xpm \
	pekwm/icons/window-command.xpm \
	pekwm/icons/window-focus.xpm \
	pekwm/icons/window-kill.xpm \
	pekwm/icons/window-lower.xpm \
	pekwm/icons/window-maximize-b.xpm \
	pekwm/icons/window-maximize-h.xpm \
	pekwm/icons/window-maximize-v.xpm \
	pekwm/icons/window-maximize.xpm \
	pekwm/icons/window-menu.xpm \
	pekwm/icons/window-minimize.xpm \
	pekwm/icons/window-move.xpm \
	pekwm/icons/window-raise.xpm \
	pekwm/icons/window-resize.xpm \
	pekwm/icons/window-sendto.xpm \
	pekwm/icons/window-shade.xpm \
	pekwm/icons/window-snap.xpm \
	pekwm/icons/window-stick.xpm \
	pekwm/icons/window-titlebar.xpm \
	pekwm/icons/window-unshade.xpm

nobase_dist_share_DATA += \
	pekwm/themes/Squared-cadet/arrow_focused.xpm \
	pekwm/themes/Squared-cadet/arrow_selected.xpm \
	pekwm/themes/Squared-cadet/arrow_unfocus.xpm \
	pekwm/themes/Squared-cadet/border_bl.xpm \
	pekwm/themes/Squared-cadet/border_br.xpm \
	pekwm/themes/Squared-cadet/border_b.xpm \
	pekwm/themes/Squared-cadet/border_lr.xpm \
	pekwm/themes/Squared-cadet/close_focused.xpm \
	pekwm/themes/Squared-cadet/close_hovered.xpm \
	pekwm/themes/Squared-cadet/close_pressed.xpm \
	pekwm/themes/Squared-cadet/close_unfocus.xpm \
	pekwm/themes/Squared-cadet/maximize_focused.xpm \
	pekwm/themes/Squared-cadet/maximize_hovered.xpm \
	pekwm/themes/Squared-cadet/maximize_pressed.xpm \
	pekwm/themes/Squared-cadet/maximize_unfocus.xpm \
	pekwm/themes/Squared-cadet/menu_focused.xpm \
	pekwm/themes/Squared-cadet/menu_hovered.xpm \
	pekwm/themes/Squared-cadet/menu_pressed.xpm \
	pekwm/themes/Squared-cadet/menu_unfocus.xpm \
	pekwm/themes/Squared-cadet/minimize_focused.xpm \
	pekwm/themes/Squared-cadet/minimize_hovered.xpm \
	pekwm/themes/Squared-cadet/minimize_pressed.xpm \
	pekwm/themes/Squared-cadet/minimize_unfocus.xpm \
	pekwm/themes/Squared-cadet/separator.xpm \
	pekwm/themes/Squared-cadet/shade_focused.xpm \
	pekwm/themes/Squared-cadet/shade_hovered.xpm \
	pekwm/themes/Squared-cadet/shade_pressed.xpm \
	pekwm/themes/Squared-cadet/shade_unfocus.xpm \
	pekwm/themes/Squared-cadet/stick_focused.xpm \
	pekwm/themes/Squared-cadet/stick_hovered.xpm \
	pekwm/themes/Squared-cadet/stick_pressed.xpm \
	pekwm/themes/Squared-cadet/stick_unfocus.xpm \
	pekwm/themes/Squared-cadet/tab_separator.xpm \
	pekwm/themes/Squared-cadet/theme

nobase_dist_share_DATA += \
	pekwm/themes/Squared-grey/arrow_focused.xpm \
	pekwm/themes/Squared-grey/arrow_selected.xpm \
	pekwm/themes/Squared-grey/arrow_unfocus.xpm \
	pekwm/themes/Squared-grey/border_bl.xpm \
	pekwm/themes/Squared-grey/border_br.xpm \
	pekwm/themes/Squared-grey/border_b.xpm \
	pekwm/themes/Squared-grey/border_lr.xpm \
	pekwm/themes/Squared-grey/close_focused.xpm \
	pekwm/themes/Squared-grey/close_hovered.xpm \
	pekwm/themes/Squared-grey/close_pressed.xpm \
	pekwm/themes/Squared-grey/close_unfocus.xpm \
	pekwm/themes/Squared-grey/maximize_focused.xpm \
	pekwm/themes/Squared-grey/maximize_hovered.xpm \
	pekwm/themes/Squared-grey/maximize_pressed.xpm \
	pekwm/themes/Squared-grey/maximize_unfocus.xpm \
	pekwm/themes/Squared-grey/menu_focused.xpm \
	pekwm/themes/Squared-grey/menu_hovered.xpm \
	pekwm/themes/Squared-grey/menu_pressed.xpm \
	pekwm/themes/Squared-grey/menu_unfocus.xpm \
	pekwm/themes/Squared-grey/minimize_focused.xpm \
	pekwm/themes/Squared-grey/minimize_hovered.xpm \
	pekwm/themes/Squared-grey/minimize_pressed.xpm \
	pekwm/themes/Squared-grey/minimize_unfocus.xpm \
	pekwm/themes/Squared-grey/separator.xpm \
	pekwm/themes/Squared-grey/shade_focused.xpm \
	pekwm/themes/Squared-grey/shade_hovered.xpm \
	pekwm/themes/Squared-grey/shade_pressed.xpm \
	pekwm/themes/Squared-grey/shade_unfocus.xpm \
	pekwm/themes/Squared-grey/stick_focused.xpm \
	pekwm/themes/Squared-grey/stick_hovered.xpm \
	pekwm/themes/Squared-grey/stick_pressed.xpm \
	pekwm/themes/Squared-grey/stick_unfocus.xpm \
	pekwm/themes/Squared-grey/tab_separator.xpm \
	pekwm/themes/Squared-grey/theme

nobase_dist_share_DATA += \
	pekwm/themes/Squared-green/arrow_focused.xpm \
	pekwm/themes/Squared-green/arrow_selected.xpm \
	pekwm/themes/Squared-green/arrow_unfocus.xpm \
	pekwm/themes/Squared-green/border_bl.xpm \
	pekwm/themes/Squared-green/border_br.xpm \
	pekwm/themes/Squared-green/border_b.xpm \
	pekwm/themes/Squared-green/border_lr.xpm \
	pekwm/themes/Squared-green/close_focused.xpm \
	pekwm/themes/Squared-green/close_hovered.xpm \
	pekwm/themes/Squared-green/close_pressed.xpm \
	pekwm/themes/Squared-green/close_unfocus.xpm \
	pekwm/themes/Squared-green/maximize_focused.xpm \
	pekwm/themes/Squared-green/maximize_hovered.xpm \
	pekwm/themes/Squared-green/maximize_pressed.xpm \
	pekwm/themes/Squared-green/maximize_unfocus.xpm \
	pekwm/themes/Squared-green/menu_focused.xpm \
	pekwm/themes/Squared-green/menu_hovered.xpm \
	pekwm/themes/Squared-green/menu_pressed.xpm \
	pekwm/themes/Squared-green/menu_unfocus.xpm \
	pekwm/themes/Squared-green/minimize_focused.xpm \
	pekwm/themes/Squared-green/minimize_hovered.xpm \
	pekwm/themes/Squared-green/minimize_pressed.xpm \
	pekwm/themes/Squared-green/minimize_unfocus.xpm \
	pekwm/themes/Squared-green/separator.xpm \
	pekwm/themes/Squared-green/shade_focused.xpm \
	pekwm/themes/Squared-green/shade_hovered.xpm \
	pekwm/themes/Squared-green/shade_pressed.xpm \
	pekwm/themes/Squared-green/shade_unfocus.xpm \
	pekwm/themes/Squared-green/stick_focused.xpm \
	pekwm/themes/Squared-green/stick_hovered.xpm \
	pekwm/themes/Squared-green/stick_pressed.xpm \
	pekwm/themes/Squared-green/stick_unfocus.xpm \
	pekwm/themes/Squared-green/tab_separator.xpm \
	pekwm/themes/Squared-green/theme

nobase_dist_share_DATA += \
	pekwm/themes/Squared-blue/arrow_focused.xpm \
	pekwm/themes/Squared-blue/arrow_selected.xpm \
	pekwm/themes/Squared-blue/arrow_unfocus.xpm \
	pekwm/themes/Squared-blue/border_bl.xpm \
	pekwm/themes/Squared-blue/border_br.xpm \
	pekwm/themes/Squared-blue/border_b.xpm \
	pekwm/themes/Squared-blue/border_lr.xpm \
	pekwm/themes/Squared-blue/close_focused.xpm \
	pekwm/themes/Squared-blue/close_hovered.xpm \
	pekwm/themes/Squared-blue/close_pressed.xpm \
	pekwm/themes/Squared-blue/close_unfocus.xpm \
	pekwm/themes/Squared-blue/maximize_focused.xpm \
	pekwm/themes/Squared-blue/maximize_hovered.xpm \
	pekwm/themes/Squared-blue/maximize_pressed.xpm \
	pekwm/themes/Squared-blue/maximize_unfocus.xpm \
	pekwm/themes/Squared-blue/menu_focused.xpm \
	pekwm/themes/Squared-blue/menu_hovered.xpm \
	pekwm/themes/Squared-blue/menu_pressed.xpm \
	pekwm/themes/Squared-blue/menu_unfocus.xpm \
	pekwm/themes/Squared-blue/minimize_focused.xpm \
	pekwm/themes/Squared-blue/minimize_hovered.xpm \
	pekwm/themes/Squared-blue/minimize_pressed.xpm \
	pekwm/themes/Squared-blue/minimize_unfocus.xpm \
	pekwm/themes/Squared-blue/separator.xpm \
	pekwm/themes/Squared-blue/shade_focused.xpm \
	pekwm/themes/Squared-blue/shade_hovered.xpm \
	pekwm/themes/Squared-blue/shade_pressed.xpm \
	pekwm/themes/Squared-blue/shade_unfocus.xpm \
	pekwm/themes/Squared-blue/stick_focused.xpm \
	pekwm/themes/Squared-blue/stick_hovered.xpm \
	pekwm/themes/Squared-blue/stick_pressed.xpm \
	pekwm/themes/Squared-blue/stick_unfocus.xpm \
	pekwm/themes/Squared-blue/tab_separator.xpm \
	pekwm/themes/Squared-blue/theme

install-data-twm:
	$(MKDIR_P) $(DESTDIR)$(sharedir)/twm/styles
	$(LN_S) $(sharedir)/twm/styles/Default/stylerc $(DESTDIR)$(sharedir)/twm/stylerc
	$(LN_S) Classic $(DESTDIR)$(sharedir)/twm/styles/Default

nobase_dist_share_DATA += \
	twm/apps \
	twm/buttons \
	twm/defaults \
	twm/keys \
	twm/menu \
	twm/stylemenu \
	twm/winmenu

nobase_dist_share_SCRIPTS += \
	twm/setstyle

nobase_nodist_share_SCRIPTS += \
	twm/config \
	twm/getstyles \
	twm/rc.m4 \
	twm/start

nobase_dist_share_DATA += \
	twm/styles/Classic/stylerc \
	twm/styles/Squared/_stylerc

nobase_dist_share_DATA += \
	twm/styles/Squared-blue/stylerc \
	twm/styles/Squared-blue/bitmaps/menu.xbm \
	twm/styles/Squared-blue/bitmaps/shade.xbm \
	twm/styles/Squared-blue/bitmaps/stick.xbm \
	twm/styles/Squared-blue/bitmaps/lhalf.xbm \
	twm/styles/Squared-blue/bitmaps/rhalf.xbm \
	twm/styles/Squared-blue/bitmaps/min.xbm \
	twm/styles/Squared-blue/bitmaps/max.xbm \
	twm/styles/Squared-blue/bitmaps/close.xbm \
	twm/styles/Squared-blue/bitmaps/desk.xbm \
	twm/styles/Squared-blue/bitmaps/iconify.xbm

nobase_dist_share_DATA += \
	twm/styles/Squared-cadet/stylerc \
	twm/styles/Squared-cadet/bitmaps/menu.xbm \
	twm/styles/Squared-cadet/bitmaps/shade.xbm \
	twm/styles/Squared-cadet/bitmaps/stick.xbm \
	twm/styles/Squared-cadet/bitmaps/lhalf.xbm \
	twm/styles/Squared-cadet/bitmaps/rhalf.xbm \
	twm/styles/Squared-cadet/bitmaps/min.xbm \
	twm/styles/Squared-cadet/bitmaps/max.xbm \
	twm/styles/Squared-cadet/bitmaps/close.xbm \
	twm/styles/Squared-cadet/bitmaps/desk.xbm \
	twm/styles/Squared-cadet/bitmaps/iconify.xbm

nobase_dist_share_DATA += \
	twm/styles/Squared-green/stylerc \
	twm/styles/Squared-green/bitmaps/menu.xbm \
	twm/styles/Squared-green/bitmaps/shade.xbm \
	twm/styles/Squared-green/bitmaps/stick.xbm \
	twm/styles/Squared-green/bitmaps/lhalf.xbm \
	twm/styles/Squared-green/bitmaps/rhalf.xbm \
	twm/styles/Squared-green/bitmaps/min.xbm \
	twm/styles/Squared-green/bitmaps/max.xbm \
	twm/styles/Squared-green/bitmaps/close.xbm \
	twm/styles/Squared-green/bitmaps/desk.xbm \
	twm/styles/Squared-green/bitmaps/iconify.xbm

nobase_dist_share_DATA += \
	twm/styles/Squared-grey/stylerc \
	twm/styles/Squared-grey/bitmaps/menu.xbm \
	twm/styles/Squared-grey/bitmaps/shade.xbm \
	twm/styles/Squared-grey/bitmaps/stick.xbm \
	twm/styles/Squared-grey/bitmaps/lhalf.xbm \
	twm/styles/Squared-grey/bitmaps/rhalf.xbm \
	twm/styles/Squared-grey/bitmaps/min.xbm \
	twm/styles/Squared-grey/bitmaps/max.xbm \
	twm/styles/Squared-grey/bitmaps/close.xbm \
	twm/styles/Squared-grey/bitmaps/desk.xbm \
	twm/styles/Squared-grey/bitmaps/iconify.xbm

install-data-ctwm:
	$(MKDIR_P) $(DESTDIR)$(sharedir)/ctwm/styles
	$(LN_S) $(sharedir)/ctwm/styles/Default/stylerc $(DESTDIR)$(sharedir)/ctwm/stylerc
	$(LN_S) Classic $(DESTDIR)$(sharedir)/ctwm/styles/Default

nobase_dist_share_DATA += \
	ctwm/apps \
	ctwm/buttons \
	ctwm/defaults \
	ctwm/keys \
	ctwm/menu \
	ctwm/stylemenu \
	ctwm/winmenu

nobase_dist_share_SCRIPTS += \
	ctwm/setstyle

nobase_nodist_share_SCRIPTS += \
	ctwm/config \
	ctwm/getstyles \
	ctwm/rc.m4 \
	ctwm/start

nobase_dist_share_DATA += \
	ctwm/styles/Classic/stylerc \
	ctwm/styles/Squared/_stylerc

nobase_dist_share_DATA += \
	ctwm/styles/Squared-blue/stylerc \
	ctwm/styles/Squared-blue/pixmaps/menu_pressed.xpm \
	ctwm/styles/Squared-blue/pixmaps/shade_pressed.xpm \
	ctwm/styles/Squared-blue/pixmaps/stick_pressed.xpm \
	ctwm/styles/Squared-blue/pixmaps/lhalf_pressed.xpm \
	ctwm/styles/Squared-blue/pixmaps/rhalf_pressed.xpm \
	ctwm/styles/Squared-blue/pixmaps/min_pressed.xpm \
	ctwm/styles/Squared-blue/pixmaps/max_pressed.xpm \
	ctwm/styles/Squared-blue/pixmaps/close_pressed.xpm \
	ctwm/styles/Squared-blue/pixmaps/stuck_pressed.xpm \
	ctwm/styles/Squared-blue/pixmaps/unshade_pressed.xpm

nobase_dist_share_DATA += \
	ctwm/styles/Squared-cadet/stylerc \
	ctwm/styles/Squared-cadet/pixmaps/menu_pressed.xpm \
	ctwm/styles/Squared-cadet/pixmaps/shade_pressed.xpm \
	ctwm/styles/Squared-cadet/pixmaps/stick_pressed.xpm \
	ctwm/styles/Squared-cadet/pixmaps/lhalf_pressed.xpm \
	ctwm/styles/Squared-cadet/pixmaps/rhalf_pressed.xpm \
	ctwm/styles/Squared-cadet/pixmaps/min_pressed.xpm \
	ctwm/styles/Squared-cadet/pixmaps/max_pressed.xpm \
	ctwm/styles/Squared-cadet/pixmaps/close_pressed.xpm \
	ctwm/styles/Squared-cadet/pixmaps/stuck_pressed.xpm \
	ctwm/styles/Squared-cadet/pixmaps/unshade_pressed.xpm

nobase_dist_share_DATA += \
	ctwm/styles/Squared-green/stylerc \
	ctwm/styles/Squared-green/pixmaps/menu_pressed.xpm \
	ctwm/styles/Squared-green/pixmaps/shade_pressed.xpm \
	ctwm/styles/Squared-green/pixmaps/stick_pressed.xpm \
	ctwm/styles/Squared-green/pixmaps/lhalf_pressed.xpm \
	ctwm/styles/Squared-green/pixmaps/rhalf_pressed.xpm \
	ctwm/styles/Squared-green/pixmaps/min_pressed.xpm \
	ctwm/styles/Squared-green/pixmaps/max_pressed.xpm \
	ctwm/styles/Squared-green/pixmaps/close_pressed.xpm \
	ctwm/styles/Squared-green/pixmaps/stuck_pressed.xpm \
	ctwm/styles/Squared-green/pixmaps/unshade_pressed.xpm

nobase_dist_share_DATA += \
	ctwm/styles/Squared-grey/stylerc \
	ctwm/styles/Squared-grey/pixmaps/menu_pressed.xpm \
	ctwm/styles/Squared-grey/pixmaps/shade_pressed.xpm \
	ctwm/styles/Squared-grey/pixmaps/stick_pressed.xpm \
	ctwm/styles/Squared-grey/pixmaps/lhalf_pressed.xpm \
	ctwm/styles/Squared-grey/pixmaps/rhalf_pressed.xpm \
	ctwm/styles/Squared-grey/pixmaps/min_pressed.xpm \
	ctwm/styles/Squared-grey/pixmaps/max_pressed.xpm \
	ctwm/styles/Squared-grey/pixmaps/close_pressed.xpm \
	ctwm/styles/Squared-grey/pixmaps/stuck_pressed.xpm \
	ctwm/styles/Squared-grey/pixmaps/unshade_pressed.xpm

install-data-vtwm:
	$(MKDIR_P) $(DESTDIR)$(sharedir)/vtwm/styles
	$(LN_S) $(sharedir)/vtwm/styles/Default/stylerc $(DESTDIR)$(sharedir)/vtwm/stylerc
	$(LN_S) Classic $(DESTDIR)$(sharedir)/vtwm/styles/Default

nobase_dist_share_DATA += \
	vtwm/apps \
	vtwm/buttons \
	vtwm/defaults \
	vtwm/keys \
	vtwm/menu \
	vtwm/stylemenu \
	vtwm/winmenu

nobase_dist_share_SCRIPTS += \
	vtwm/setstyle

nobase_nodist_share_SCRIPTS += \
	vtwm/config \
	vtwm/getstyles \
	vtwm/rc.m4 \
	vtwm/start

nobase_dist_share_DATA += \
	vtwm/styles/Classic/stylerc \
	vtwm/styles/Squared/_stylerc

nobase_dist_share_DATA += \
	vtwm/styles/Squared-blue/stylerc \
	vtwm/styles/Squared-blue/pixmaps/menu_pressed.xpm \
	vtwm/styles/Squared-blue/pixmaps/shade_pressed.xpm \
	vtwm/styles/Squared-blue/pixmaps/stick_pressed.xpm \
	vtwm/styles/Squared-blue/pixmaps/lhalf_pressed.xpm \
	vtwm/styles/Squared-blue/pixmaps/rhalf_pressed.xpm \
	vtwm/styles/Squared-blue/pixmaps/min_pressed.xpm \
	vtwm/styles/Squared-blue/pixmaps/max_pressed.xpm \
	vtwm/styles/Squared-blue/pixmaps/close_pressed.xpm \
	vtwm/styles/Squared-blue/pixmaps/stuck_pressed.xpm \
	vtwm/styles/Squared-blue/pixmaps/unshade_pressed.xpm

nobase_dist_share_DATA += \
	vtwm/styles/Squared-cadet/stylerc \
	vtwm/styles/Squared-cadet/pixmaps/menu_pressed.xpm \
	vtwm/styles/Squared-cadet/pixmaps/shade_pressed.xpm \
	vtwm/styles/Squared-cadet/pixmaps/stick_pressed.xpm \
	vtwm/styles/Squared-cadet/pixmaps/lhalf_pressed.xpm \
	vtwm/styles/Squared-cadet/pixmaps/rhalf_pressed.xpm \
	vtwm/styles/Squared-cadet/pixmaps/min_pressed.xpm \
	vtwm/styles/Squared-cadet/pixmaps/max_pressed.xpm \
	vtwm/styles/Squared-cadet/pixmaps/close_pressed.xpm \
	vtwm/styles/Squared-cadet/pixmaps/stuck_pressed.xpm \
	vtwm/styles/Squared-cadet/pixmaps/unshade_pressed.xpm

nobase_dist_share_DATA += \
	vtwm/styles/Squared-green/stylerc \
	vtwm/styles/Squared-green/pixmaps/menu_pressed.xpm \
	vtwm/styles/Squared-green/pixmaps/shade_pressed.xpm \
	vtwm/styles/Squared-green/pixmaps/stick_pressed.xpm \
	vtwm/styles/Squared-green/pixmaps/lhalf_pressed.xpm \
	vtwm/styles/Squared-green/pixmaps/rhalf_pressed.xpm \
	vtwm/styles/Squared-green/pixmaps/min_pressed.xpm \
	vtwm/styles/Squared-green/pixmaps/max_pressed.xpm \
	vtwm/styles/Squared-green/pixmaps/close_pressed.xpm \
	vtwm/styles/Squared-green/pixmaps/stuck_pressed.xpm \
	vtwm/styles/Squared-green/pixmaps/unshade_pressed.xpm

nobase_dist_share_DATA += \
	vtwm/styles/Squared-grey/stylerc \
	vtwm/styles/Squared-grey/pixmaps/menu_pressed.xpm \
	vtwm/styles/Squared-grey/pixmaps/shade_pressed.xpm \
	vtwm/styles/Squared-grey/pixmaps/stick_pressed.xpm \
	vtwm/styles/Squared-grey/pixmaps/lhalf_pressed.xpm \
	vtwm/styles/Squared-grey/pixmaps/rhalf_pressed.xpm \
	vtwm/styles/Squared-grey/pixmaps/min_pressed.xpm \
	vtwm/styles/Squared-grey/pixmaps/max_pressed.xpm \
	vtwm/styles/Squared-grey/pixmaps/close_pressed.xpm \
	vtwm/styles/Squared-grey/pixmaps/stuck_pressed.xpm \
	vtwm/styles/Squared-grey/pixmaps/unshade_pressed.xpm

install-data-local: install-data-twm install-data-ctwm install-data-vtwm

.PHONY: install-data-twm install-data-ctwm install-data-vtwm

