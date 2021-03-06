
AM_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(SMLIB_CFLAGS) $(GDK_CFLAGS)

lib_LTLIBRARIES = libxde.la

if MAINTAINER_MODE
runtime_search =
runtime_path =
else
runtime_search = -R $(xdemoddir)
runtime_path = -rpath $(xdemoddir)
endif

xdeinc_HEADERS = xde.h

libxde_la_CPPFLAGS = -DNAME=\"libxde\"
libxde_la_SOURCES = xde.h xde.c
libxde_la_LIBADD = $(GDK_LIBS) $(SMLIB_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(X11_LIBS)
libxde_la_LDFLAGS = -version-info 0:0:0 -export-dynamic -shared $(runtime_search) \
		    -Wl,--version-script -Wl,$(srcdir)/xde.VERSION -ldl

EXTRA_DIST = xde.VERSION

bin_PROGRAMS = \
	xde-theme \
	xde-style \
	xde-identify \
	xde-setwm \
	xde-watch

noinst_PROGRAMS = \
	xde-setbg

EXTRA_PROGRAMS = \
	xde-backdrop \
	xde-setbg

xde_theme_CPPFLAGS = -DNAME=\"xde-theme\"
xde_theme_SOURCES = xde-theme.c xde.h
xde_theme_LDADD = libxde.la $(GDK_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(X11_LIBS)
xde_theme_LDFLAGS = $(runtime_search) -ldl

xde_identify_CPPFLAGS = -DNAME=\"xde-identify\"
xde_identify_SOURCES = xde-identify.c xde.h
xde_identify_LDADD = libxde.la $(X11_LIBS)
xde_identify_LDFLAGS = $(runtime_search) -ldl

xde_style_CPPFLAGS = -DNAME=\"xde-style\"
xde_style_SOURCES = xde-style.c xde.h
xde_style_LDADD = libxde.la $(X11_LIBS)
xde_style_LDFLAGS = $(runtime_search) -ldl

xde_backdrop_CPPFLAGS = -DNAME=\"xde-backdrop\"
xde_backdrop_SOURCES = xde-backdrop.c xde.h
xde_backdrop_LDADD = libxde.la $(X11_LIBS)
xde_backdrop_LDFLAGS = $(runtime_search) -ldl

xde_setbg_CPPFLAGS = -DNAME=\"xde-setbg\"
xde_setbg_SOURCES = xde-setbg.c xde.h
xde_setbg_LDADD = libxde.la $(X11_LIBS)
xde_setbg_LDFLAGS = $(runtime_search) -ldl

xde_setwm_CPPFLAGS = -DNAME=\"xde-setwm\"
xde_setwm_SOURCES = xde-setwm.c xde.h
xde_setwm_LDADD = $(X11_LIBS)

xde_watch_CPPFLAGS = -DNAME=\"xde-watch\"
xde_watch_SOURCES = xde-watch.c xde.h
xde_watch_LDADD = libxde.la $(X11_LIBS)
xde_watch_LDFLAGS = $(runtime_search) -ldl

xdemod_LTLIBRARIES = \
	xde-2bwm.la \
	xde-adwm.la \
	xde-afterstep.la \
	xde-awesome.la \
	xde-blackbox.la \
	xde-ctwm.la \
	xde-cwm.la \
	xde-dtwm.la \
	xde-dwm.la \
	xde-echinus.la \
	xde-etwm.la \
	xde-fluxbox.la \
	xde-flwm.la \
	xde-fvwm.la \
	xde-icewm.la \
	xde-jwm.la \
	xde-matwm2.la \
	xde-metacity.la \
	xde-mwm.la \
	xde-openbox.la \
	xde-pekwm.la \
	xde-spectrwm.la \
	xde-twm.la \
	xde-uwm.la \
	xde-vtwm.la \
	xde-waimea.la \
	xde-wind.la \
	xde-wmaker.la \
	xde-wmx.la \
	xde-yeahwm.la

module_ldflags = -module -export-dynamic -release $(VERSION) -shared $(runtime_path) -avoid-version
#module_ldflags = -module -export-dynamic -release $(VERSION) -shared -rpath ${xdemoddir} -avoid-version

xde_2bwm_la_CPPFLAGS = -DNAME=\"xde-2bwm\"
xde_2bwm_la_CFLAGS = $(AM_CFLAGS)
xde_2bwm_la_SOURCES = xde.h xde_2bwm.c
xde_2bwm_la_LIBADD = libxde.la
xde_2bwm_la_LDFLAGS = $(module_ldflags)

xde_adwm_la_CPPFLAGS = -DNAME=\"xde-adwm\"
xde_adwm_la_CFLAGS = $(AM_CFLAGS)
xde_adwm_la_SOURCES = xde.h xde_adwm.c
xde_adwm_la_LIBADD = libxde.la
xde_adwm_la_LDFLAGS = $(module_ldflags)

xde_afterstep_la_CPPFLAGS = -DNAME=\"xde-afterstep\"
xde_afterstep_la_CFLAGS = $(AM_CFLAGS)
xde_afterstep_la_SOURCES = xde.h xde_afterstep.c
xde_afterstep_la_LIBADD = libxde.la
xde_afterstep_la_LDFLAGS = $(module_ldflags)

xde_awesome_la_CPPFLAGS = -DNAME=\"xde-awesome\"
xde_awesome_la_CFLAGS = $(AM_CFLAGS)
xde_awesome_la_SOURCES = xde.h xde_awesome.c
xde_awesome_la_LIBADD = libxde.la
xde_awesome_la_LDFLAGS = $(module_ldflags)

xde_blackbox_la_CPPFLAGS = -DNAME=\"xde-blackbox\"
xde_blackbox_la_CFLAGS = $(AM_CFLAGS)
xde_blackbox_la_SOURCES = xde.h xde_blackbox.c
xde_blackbox_la_LIBADD = libxde.la
xde_blackbox_la_LDFLAGS = $(module_ldflags)

xde_ctwm_la_CPPFLAGS = -DNAME=\"xde-ctwm\"
xde_ctwm_la_CFLAGS = $(AM_CFLAGS)
xde_ctwm_la_SOURCES = xde.h xde_ctwm.c
xde_ctwm_la_LIBADD = libxde.la
xde_ctwm_la_LDFLAGS = $(module_ldflags)

xde_cwm_la_CPPFLAGS = -DNAME=\"xde-cwm\"
xde_cwm_la_CFLAGS = $(AM_CFLAGS)
xde_cwm_la_SOURCES = xde.h xde_cwm.c
xde_cwm_la_LIBADD = libxde.la
xde_cwm_la_LDFLAGS = $(module_ldflags)

xde_dtwm_la_CPPFLAGS = -DNAME=\"xde-dtwm\"
xde_dtwm_la_CFLAGS = $(AM_CFLAGS)
xde_dtwm_la_SOURCES = xde.h xde_dtwm.c
xde_dtwm_la_LIBADD = libxde.la
xde_dtwm_la_LDFLAGS = $(module_ldflags)

xde_dwm_la_CPPFLAGS = -DNAME=\"xde-dwm\"
xde_dwm_la_CFLAGS = $(AM_CFLAGS)
xde_dwm_la_SOURCES = xde.h xde_dwm.c
xde_dwm_la_LIBADD = libxde.la
xde_dwm_la_LDFLAGS = $(module_ldflags)

xde_echinus_la_CPPFLAGS = -DNAME=\"xde-echinus\"
xde_echinus_la_CFLAGS = $(AM_CFLAGS)
xde_echinus_la_SOURCES = xde.h xde_echinus.c
xde_echinus_la_LIBADD = libxde.la
xde_echinus_la_LDFLAGS = $(module_ldflags)

xde_etwm_la_CPPFLAGS = -DNAME=\"xde-etwm\"
xde_etwm_la_CFLAGS = $(AM_CFLAGS)
xde_etwm_la_SOURCES = xde.h xde_etwm.c
xde_etwm_la_LIBADD = libxde.la
xde_etwm_la_LDFLAGS = $(module_ldflags)

xde_fluxbox_la_CPPFLAGS = -DNAME=\"xde-fluxbox\"
xde_fluxbox_la_CFLAGS = $(AM_CFLAGS)
xde_fluxbox_la_SOURCES = xde.h xde_fluxbox.c
xde_fluxbox_la_LIBADD = libxde.la
xde_fluxbox_la_LDFLAGS = $(module_ldflags)

xde_flwm_la_CPPFLAGS = -DNAME=\"xde-flwm\"
xde_flwm_la_CFLAGS = $(AM_CFLAGS)
xde_flwm_la_SOURCES = xde.h xde_flwm.c
xde_flwm_la_LIBADD = libxde.la
xde_flwm_la_LDFLAGS = $(module_ldflags)

xde_fvwm_la_CPPFLAGS = -DNAME=\"xde-fvwm\"
xde_fvwm_la_CFLAGS = $(AM_CFLAGS)
xde_fvwm_la_SOURCES = xde.h xde_fvwm.c
xde_fvwm_la_LIBADD = libxde.la
xde_fvwm_la_LDFLAGS = $(module_ldflags)

xde_icewm_la_CPPFLAGS = -DNAME=\"xde-icewm\"
xde_icewm_la_CFLAGS = $(AM_CFLAGS)
xde_icewm_la_SOURCES = xde.h xde_icewm.c
xde_icewm_la_LIBADD = libxde.la
xde_icewm_la_LDFLAGS = $(module_ldflags)

xde_jwm_la_CPPFLAGS = -DNAME=\"xde-jwm\"
xde_jwm_la_CFLAGS = $(AM_CFLAGS)
xde_jwm_la_SOURCES = xde.h xde_jwm.c
xde_jwm_la_LIBADD = libxde.la
xde_jwm_la_LDFLAGS = $(module_ldflags)

xde_matwm2_la_CPPFLAGS = -DNAME=\"xde-matwm2\"
xde_matwm2_la_CFLAGS = $(AM_CFLAGS)
xde_matwm2_la_SOURCES = xde.h xde_matwm2.c
xde_matwm2_la_LIBADD = libxde.la
xde_matwm2_la_LDFLAGS = $(module_ldflags)

xde_metacity_la_CPPFLAGS = -DNAME=\"xde-metacity\"
xde_metacity_la_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(GOBJ_CFLAGS) $(GIO_CFLAGS) $(GIO_UNIX_CFLAGS)
xde_metacity_la_SOURCES = xde.h xde_metacity.c
xde_metacity_la_LIBADD = libxde.la $(GIO_UNIX_LIBS) $(GIO_LIBS)  $(GOBJ_LIBS) $(GLIB_LIBS)
xde_metacity_la_LDFLAGS = $(module_ldflags)

xde_mwm_la_CPPFLAGS = -DNAME=\"xde-mwm\"
xde_mwm_la_CFLAGS = $(AM_CFLAGS)
xde_mwm_la_SOURCES = xde.h xde_mwm.c
xde_mwm_la_LIBADD = libxde.la
xde_mwm_la_LDFLAGS = $(module_ldflags)

xde_openbox_la_CPPFLAGS = -DNAME=\"xde-openbox\"
xde_openbox_la_CFLAGS = $(AM_CFLAGS)
xde_openbox_la_SOURCES = xde.h xde_openbox.c
xde_openbox_la_LIBADD = libxde.la
xde_openbox_la_LDFLAGS = $(module_ldflags)

xde_pekwm_la_CPPFLAGS = -DNAME=\"xde-pekwm\"
xde_pekwm_la_CFLAGS = $(AM_CFLAGS)
xde_pekwm_la_SOURCES = xde.h xde_pekwm.c
xde_pekwm_la_LIBADD = libxde.la
xde_pekwm_la_LDFLAGS = $(module_ldflags)

xde_spectrwm_la_CPPFLAGS = -DNAME=\"xde-spectrwm\"
xde_spectrwm_la_CFLAGS = $(AM_CFLAGS)
xde_spectrwm_la_SOURCES = xde.h xde_spectrwm.c
xde_spectrwm_la_LIBADD = libxde.la
xde_spectrwm_la_LDFLAGS = $(module_ldflags)

xde_twm_la_CPPFLAGS = -DNAME=\"xde-twm\"
xde_twm_la_CFLAGS = $(AM_CFLAGS)
xde_twm_la_SOURCES = xde.h xde_twm.c
xde_twm_la_LIBADD = libxde.la
xde_twm_la_LDFLAGS = $(module_ldflags)

xde_uwm_la_CPPFLAGS = -DNAME=\"xde-uwm\"
xde_uwm_la_CFLAGS = $(AM_CFLAGS)
xde_uwm_la_SOURCES = xde.h xde_uwm.c
xde_uwm_la_LIBADD = libxde.la
xde_uwm_la_LDFLAGS = $(module_ldflags)

xde_vtwm_la_CPPFLAGS = -DNAME=\"xde-vtwm\"
xde_vtwm_la_CFLAGS = $(AM_CFLAGS)
xde_vtwm_la_SOURCES = xde.h xde_vtwm.c
xde_vtwm_la_LIBADD = libxde.la
xde_vtwm_la_LDFLAGS = $(module_ldflags)

xde_waimea_la_CPPFLAGS = -DNAME=\"xde-waimea\"
xde_waimea_la_CFLAGS = $(AM_CFLAGS)
xde_waimea_la_SOURCES = xde.h xde_waimea.c
xde_waimea_la_LIBADD = libxde.la
xde_waimea_la_LDFLAGS = $(module_ldflags)

xde_wind_la_CPPFLAGS = -DNAME=\"xde-wind\"
xde_wind_la_CFLAGS = $(AM_CFLAGS)
xde_wind_la_SOURCES = xde.h xde_wind.c
xde_wind_la_LIBADD = libxde.la
xde_wind_la_LDFLAGS = $(module_ldflags)

xde_wmaker_la_CPPFLAGS = -DNAME=\"xde-wmaker\"
xde_wmaker_la_CFLAGS = $(AM_CFLAGS)
xde_wmaker_la_SOURCES = xde.h xde_wmaker.c
xde_wmaker_la_LIBADD = libxde.la
xde_wmaker_la_LDFLAGS = $(module_ldflags)

xde_wmx_la_CPPFLAGS = -DNAME=\"xde-wmx\"
xde_wmx_la_CFLAGS = $(AM_CFLAGS)
xde_wmx_la_SOURCES = xde.h xde_wmx.c
xde_wmx_la_LIBADD = libxde.la
xde_wmx_la_LDFLAGS = $(module_ldflags)

xde_yeahwm_la_CPPFLAGS = -DNAME=\"xde-yeahwm\"
xde_yeahwm_la_CFLAGS = $(AM_CFLAGS)
xde_yeahwm_la_SOURCES = xde.h xde_yeahwm.c
xde_yeahwm_la_LIBADD = libxde.la
xde_yeahwm_la_LDFLAGS = $(module_ldflags)

