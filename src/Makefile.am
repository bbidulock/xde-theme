
bin_PROGRAMS = xde-theme xde-style

lib_LTLIBRARIES = libxde.la
xdemod_LTLIBRARIES = xde-fluxbox.la

libxde_la_CPPFLAGS = -DNAME=\"libxde\"
libxde_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
libxde_la_SOURCES = xde.h xde.c
libxde_la_LIBADD = $(GDK_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(X11_LIBS)
libxde_la_LDFLAGS = -version-info 0:0:0 -export-dynamic -R $(xdemoddir) \
		    -Wl,--version-script -Wl,xde.VERSION -ldl

xde_fluxbox_la_CPPFLAGS = -DNAME=\"xde-fluxbox\"
xde_fluxbox_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_fluxbox_la_SOURCES = xde.h xde_fluxbox.c
xde_fluxbox_la_LIBADD = libxde.la
xde_fluxbox_la_LDFLAGS = -module -export-dynamic -release $(VERSION) -shared

xde_theme_CPPFLAGS = -DNAME=\"xde-theme\"
xde_theme_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_theme_SOURCES = xde-theme.c
xde_theme_LDADD = $(GDK_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(X11_LIBS)
xde_theme_LDFLAGS = -R $(xdemoddir)

xde_style_CPPFLAGS = -DNAME=\"xde-style\"
xde_style_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_style_SOURCES = xde-style.c
xde_style_LDADD =  libxde.la $(X11_LIBS)
xde_style_LDFLAGS = -R $(xdemoddir)

