
bin_PROGRAMS = xde-theme xde-style

lib_LTLIBRARIES = libxde.la

libxde_la_CPPFLAGS = -DNAME=\"libxde\"
libxde_la_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
libxde_la_SOURCES = xde.h xde.c
libxde_la_LIBADD = $(GDK_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(X11_LIBS)

xde_theme_CPPFLAGS = -DNAME=\"xde-theme\"
xde_theme_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_theme_SOURCES = xde-theme.c
xde_theme_LDADD = $(GDK_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(X11_LIBS)

xde_style_CPPFLAGS = -DNAME=\"xde-style\"
xde_style_CFLAGS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(GDK_CFLAGS)
xde_style_SOURCES = xde-style.c
xde_style_LDADD =  libxde.la $(X11_LIBS)

