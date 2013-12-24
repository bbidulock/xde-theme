
AC_PREREQ(2.69)
AC_INIT([xde-styles],[1.1.21],[http://github.com/bbidulock/xde-styles],[xde-styles])
AC_REVISION([1.1.21])

AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_AUX_DIR([scripts])

AM_INIT_AUTOMAKE([gnits 1.14 dist-xz no-dist-gzip std-options -Wall color-tests silent-rules tar-pax])
AM_MAINTAINER_MODE

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P

test "x$prefix" = xNONE && prefix="$ac_default_prefix"
sharedir="$datadir"
eval "sharedir=\"$sharedir\""
eval "sharedir=\"$sharedir\""
eval "sharedir=\"$sharedir\""

AC_SUBST([sharedir])


AC_CONFIG_FILES([Makefile
		 openbox/Makefile
		 gtk/Makefile
		 fvwm/start
		 jwm/config
		 jwm/getstyles
		 jwm/start
		 ctwm/config
		 ctwm/getstyles
		 ctwm/rc.m4
		 ctwm/start
		 vtwm/config
		 vtwm/getstyles
		 vtwm/rc.m4
		 vtwm/start
		 twm/config
		 twm/getstyles
		 twm/rc.m4
		 twm/start
		 ])
AC_OUTPUT
