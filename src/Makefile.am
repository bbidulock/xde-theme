
AM_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)

lib_LTLIBRARIES = libxde.la

libxde_la_CPPFLAGS = -DNAME=\"libxde\"
libxde_la_SOURCES = xde.h xde.c
libxde_la_LIBADD = $(GDK_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(X11_LIBS)
libxde_la_LDFLAGS = -version-info 0:0:0 -export-dynamic -R $(xdemoddir) \
		    -Wl,--version-script -Wl,xde.VERSION -ldl

bin_PROGRAMS = xde-theme xde-style xde-identify
EXTRA_PROGRAMS = xde-backdrop

xde_theme_CPPFLAGS = -DNAME=\"xde-theme\"
xde_theme_SOURCES = xde-theme.c
xde_theme_LDADD = $(GDK_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(X11_LIBS)
xde_theme_LDFLAGS = -R $(xdemoddir) -ldl -dlpreopen xde-fluxbox.la

xde_identify_CPPFLAGS = -DNAME=\"xde-identify\"
xde_identify_SOURCES = xde-identify.c
xde_identify_LDADD = libxde.la $(X11_LIBS)
xde_identify_LDFLAGS = -R $(xdemoddir) -ldl -dlpreopen xde-fluxbox.la

xde_style_CPPFLAGS = -DNAME=\"xde-style\"
xde_style_SOURCES = xde-style.c
xde_style_LDADD = libxde.la $(X11_LIBS)
xde_style_LDFLAGS = -R $(xdemoddir) -ldl -dlpreopen xde-fluxbox.la

xde_backdrop_CPPFLAGS = -DNAME=\"xde-backdrop\"
xde_backdrop_SOURCES = xde-backdrop.c
xde_backdrop_LDADD = libxde.la $(X11_LIBS)
xde_backdrop_LDFLAGS = -R $(xdemoddir) -ldl -dlpreopen xde-fluxbox.la

xdemod_LTLIBRARIES = \
	xde-2bwm.la \
	xde-afterstep.la \
	xde-awesome.la \
	xde-blackbox.la \
	xde-ctwm.la \
	xde-cwm.la \
	xde-dtwm.la \
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

xde_2bwm_la_CPPFLAGS = -DNAME=\"xde-2bwm\"
xde_2bwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_2bwm_la_SOURCES = xde.h xde_2bwm.c
xde_2bwm_la_LIBADD = libxde.la
xde_2bwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_afterstep_la_CPPFLAGS = -DNAME=\"xde-afterstep\"
xde_afterstep_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_afterstep_la_SOURCES = xde.h xde_afterstep.c
xde_afterstep_la_LIBADD = libxde.la
xde_afterstep_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_awesome_la_CPPFLAGS = -DNAME=\"xde-awesome\"
xde_awesome_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_awesome_la_SOURCES = xde.h xde_awesome.c
xde_awesome_la_LIBADD = libxde.la
xde_awesome_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_blackbox_la_CPPFLAGS = -DNAME=\"xde-blackbox\"
xde_blackbox_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_blackbox_la_SOURCES = xde.h xde_blackbox.c
xde_blackbox_la_LIBADD = libxde.la
xde_blackbox_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_ctwm_la_CPPFLAGS = -DNAME=\"xde-ctwm\"
xde_ctwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_ctwm_la_SOURCES = xde.h xde_ctwm.c
xde_ctwm_la_LIBADD = libxde.la
xde_ctwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_cwm_la_CPPFLAGS = -DNAME=\"xde-cwm\"
xde_cwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_cwm_la_SOURCES = xde.h xde_cwm.c
xde_cwm_la_LIBADD = libxde.la
xde_cwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_dtwm_la_CPPFLAGS = -DNAME=\"xde-dtwm\"
xde_dtwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_dtwm_la_SOURCES = xde.h xde_dtwm.c
xde_dtwm_la_LIBADD = libxde.la
xde_dtwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_echinus_la_CPPFLAGS = -DNAME=\"xde-echinus\"
xde_echinus_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_echinus_la_SOURCES = xde.h xde_echinus.c
xde_echinus_la_LIBADD = libxde.la
xde_echinus_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_etwm_la_CPPFLAGS = -DNAME=\"xde-etwm\"
xde_etwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_etwm_la_SOURCES = xde.h xde_etwm.c
xde_etwm_la_LIBADD = libxde.la
xde_etwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_fluxbox_la_CPPFLAGS = -DNAME=\"xde-fluxbox\"
xde_fluxbox_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_fluxbox_la_SOURCES = xde.h xde_fluxbox.c
xde_fluxbox_la_LIBADD = libxde.la
xde_fluxbox_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_flwm_la_CPPFLAGS = -DNAME=\"xde-flwm\"
xde_flwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_flwm_la_SOURCES = xde.h xde_flwm.c
xde_flwm_la_LIBADD = libxde.la
xde_flwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_fvwm_la_CPPFLAGS = -DNAME=\"xde-fvwm\"
xde_fvwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_fvwm_la_SOURCES = xde.h xde_fvwm.c
xde_fvwm_la_LIBADD = libxde.la
xde_fvwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_icewm_la_CPPFLAGS = -DNAME=\"xde-icewm\"
xde_icewm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_icewm_la_SOURCES = xde.h xde_icewm.c
xde_icewm_la_LIBADD = libxde.la
xde_icewm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_jwm_la_CPPFLAGS = -DNAME=\"xde-jwm\"
xde_jwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_jwm_la_SOURCES = xde.h xde_jwm.c
xde_jwm_la_LIBADD = libxde.la
xde_jwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_matwm2_la_CPPFLAGS = -DNAME=\"xde-matwm2\"
xde_matwm2_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_matwm2_la_SOURCES = xde.h xde_matwm2.c
xde_matwm2_la_LIBADD = libxde.la
xde_matwm2_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_metacity_la_CPPFLAGS = -DNAME=\"xde-metacity\"
xde_metacity_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_metacity_la_SOURCES = xde.h xde_metacity.c
xde_metacity_la_LIBADD = libxde.la
xde_metacity_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_mwm_la_CPPFLAGS = -DNAME=\"xde-mwm\"
xde_mwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_mwm_la_SOURCES = xde.h xde_mwm.c
xde_mwm_la_LIBADD = libxde.la
xde_mwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_openbox_la_CPPFLAGS = -DNAME=\"xde-openbox\"
xde_openbox_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_openbox_la_SOURCES = xde.h xde_openbox.c
xde_openbox_la_LIBADD = libxde.la
xde_openbox_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_pekwm_la_CPPFLAGS = -DNAME=\"xde-pekwm\"
xde_pekwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_pekwm_la_SOURCES = xde.h xde_pekwm.c
xde_pekwm_la_LIBADD = libxde.la
xde_pekwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_spectrwm_la_CPPFLAGS = -DNAME=\"xde-spectrwm\"
xde_spectrwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_spectrwm_la_SOURCES = xde.h xde_spectrwm.c
xde_spectrwm_la_LIBADD = libxde.la
xde_spectrwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_twm_la_CPPFLAGS = -DNAME=\"xde-twm\"
xde_twm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_twm_la_SOURCES = xde.h xde_twm.c
xde_twm_la_LIBADD = libxde.la
xde_twm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_uwm_la_CPPFLAGS = -DNAME=\"xde-uwm\"
xde_uwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_uwm_la_SOURCES = xde.h xde_uwm.c
xde_uwm_la_LIBADD = libxde.la
xde_uwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_vtwm_la_CPPFLAGS = -DNAME=\"xde-vtwm\"
xde_vtwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_vtwm_la_SOURCES = xde.h xde_vtwm.c
xde_vtwm_la_LIBADD = libxde.la
xde_vtwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_waimea_la_CPPFLAGS = -DNAME=\"xde-waimea\"
xde_waimea_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_waimea_la_SOURCES = xde.h xde_waimea.c
xde_waimea_la_LIBADD = libxde.la
xde_waimea_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_wind_la_CPPFLAGS = -DNAME=\"xde-wind\"
xde_wind_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_wind_la_SOURCES = xde.h xde_wind.c
xde_wind_la_LIBADD = libxde.la
xde_wind_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_wmaker_la_CPPFLAGS = -DNAME=\"xde-wmaker\"
xde_wmaker_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_wmaker_la_SOURCES = xde.h xde_wmaker.c
xde_wmaker_la_LIBADD = libxde.la
xde_wmaker_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_wmx_la_CPPFLAGS = -DNAME=\"xde-wmx\"
xde_wmx_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_wmx_la_SOURCES = xde.h xde_wmx.c
xde_wmx_la_LIBADD = libxde.la
xde_wmx_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_yeahwm_la_CPPFLAGS = -DNAME=\"xde-yeahwm\"
xde_yeahwm_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_yeahwm_la_SOURCES = xde.h xde_yeahwm.c
xde_yeahwm_la_LIBADD = libxde.la
xde_yeahwm_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

