#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([xde-theme],[1.5],[https://github.com/bbidulock/xde-theme],[xde-theme], [https://github.com/bbidulock/xde-theme/issues])
AC_REVISION([1.5])

# set better defaults
AC_PREFIX_DEFAULT([/usr])
if test "$prefix" = /usr -o "$prefix" = NONE; then
	test "$sysconfdir" = '${prefix}/etc' && sysconfdir=/etc
	test "$sharedstatedir" = '${prefix}/com' && sharedstatedir=/var
	test "$localstatedir" = '${prefix}/var' && localstatedir=/var
fi

AC_CONFIG_SRCDIR([src/xde.c])
AC_CONFIG_HEADERS([autoconf.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([scripts])
AC_USE_SYSTEM_EXTENSIONS
AC_CANONICAL_TARGET

DATE='2021-05-09'
AC_SUBST([DATE])

MDOCDATE='May 9, 2021'
AC_SUBST([MDOCDATE])

BRANCH='1.5'
AC_SUBST([BRANCH])

# Initialize Automake
AM_INIT_AUTOMAKE([gnits 1.16 dist-lzip no-dist-gzip std-options -Wall color-tests silent-rules tar-pax])
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_SED
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_AR
AC_PROG_MKDIR_P
AC_ARG_VAR([LD], [Linker loader command])

AC_ARG_VAR([MARKDOWN],[markdown command. @<:@default=markdown@:>@])
AC_CHECK_PROGS([MARKDOWN],[markdown],[${am_missing_run}markdown],[$PATH])

AC_ARG_VAR([ASCIIDOC],[asciidoc command. @<:@default=asciidoctor@:>@])
AC_CHECK_PROGS([ASCIIDOC],[asciidoc asciidoctor],[${am_missing_run}asciidoc],[$PATH])

AC_ARG_VAR([ASCIIDOC_BACKEND],[asciidoc backend @<:@default=NONE@:>@])
if test -z "$ASCIIDOC_BACKEND"; then
	case "$ASCIIDOC" in
		asciidoc) ASCIIDOC_BACKEND="--backend=html4" ;;
		asciidoctor) ASCIIDOC_BACKEND="--backend=html5" ;;
		*) AC_MSG_WARN([ASCIIDOC $ASCIIDOC is not supported.]) ;;
	esac
fi

AC_ARG_VAR([ASCIIMAN],[asciidoc to manpage converter. @<:@default=a2x@:>@])
AC_CHECK_PROGS([ASCIIMAN],[a2x asciidoctor],[${am_missing_run}a2x],[$PATH])

AC_ARG_VAR([ASCIIMAN_FORMAT],[asciidoc manpage option @<:@default=NONE@:>@])
if test -z "$ASCIIMAN_FORMAT"; then
	case "$ASCIIMAN" in
		a2x) ASCIIMAN_FORMAT="--format=manpage" ;;
		asciidoctor) ASCIIMAN_FORMAT="--backend=manpage" ;;
		*) AC_MSG_WARN([ASCIIMAN $ASCIIMAN is not supported.]) ;;
	esac
fi

AC_ARG_VAR([LINKS],[links command. @<:@default=lynx@:>@])
AC_CHECK_PROGS([LINKS],[lynx links],[${am_missing_run}lynx],[$PATH])
if test "$LINKS" = "links"; then
	LINKS="links -codepage utf-8"
fi

AC_DISABLE_STATIC
LT_PATH_LD
LT_PREREQ([2.4.2])
LT_INIT([dlopen])
LT_LANG([C])
AC_SUBST([LIBTOOL_DEPS])

PKG_PROG_PKG_CONFIG([0.14.0])
AC_PROG_EGREP

# Checks for libraries.

# Checks for header files.
AS_BOX([Header Files])
AC_CHECK_INCLUDES_DEFAULT
AC_PATH_X
AC_PATH_XTRA
AC_CHECK_HEADERS([fcntl.h locale.h stddef.h stdint.h stdlib.h string.h strings.h sys/ioctl.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AS_BOX([Typedefs, Structures, Compiler])
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AS_BOX([Library Functions])
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostname memmove select strcasecmp strchr strcspn strdup strerror strncasecmp strndup strpbrk strrchr strspn strstr strtol strtoul clock_gettime getcwd memset])

AS_BOX([X11 Extension Libraries])

PKG_CHECK_MODULES([X11],[x11])
PKG_CHECK_MODULES([SMLIB],[sm])
PKG_CHECK_MODULES([GDK],[gdk-pixbuf-xlib-2.0])
PKG_CHECK_MODULES([GLIB],[glib-2.0])
PKG_CHECK_MODULES([GOBJ],[gobject-2.0])
PKG_CHECK_MODULES([GIO],[gio-2.0])
PKG_CHECK_MODULES([GIO_UNIX],[gio-unix-2.0])
PKG_CHECK_MODULES([IMLIB],[imlib2])

AC_ARG_ENABLE([xrandr],
	AS_HELP_STRING([--disable-xrandr],[Disable XRANDR support @<:@default=auto@:>@]))
if test "x$enable_xrandr" != xno ; then
	PKG_CHECK_MODULES([XRANDR],[xrandr],
		[AC_DEFINE([XRANDR],[1], [Define to 1 to support XRANDR extension.])],
		[enable_xrandr=no])
fi
AM_CONDITIONAL([XRANDR],[test "x$enable_xrandr" != xno])
AC_MSG_CHECKING([for XRANDR support])
AC_MSG_RESULT([${enable_xrandr:-yes}])
AM_CONDITIONAL([HAVE_XRANDR],[test  "x$enable_xrandr" != "xno"])

AC_ARG_ENABLE([xinerama],
	AS_HELP_STRING([--disable-xinerama],[Disable XINERAMA support @<:@default=auto@:>@]))
if test "x$enable_xinerama" != xno ; then
	PKG_CHECK_MODULES([XINERAMA],[xinerama],
		[AC_DEFINE([XINERAMA],[1], [Define to 1 to support XINERAMA extension.])],
		[enable_xinerama=no])
fi
AM_CONDITIONAL([XINERAMA],[test "x$enable_xinerama" != xno])
AC_MSG_CHECKING([for XINERAMA support])
AC_MSG_RESULT([${enable_xinerama:-yes}])
AM_CONDITIONAL([HAVE_XINERAMA],[test  "x$enable_xinerama" != "xno"])

AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug],[Enable debugging support @<:@default=disabled@:>@]))
if test "x$enable_debug" = xyes ; then
	AC_DEFINE_UNQUOTED([DEBUG],[],[Define to enable debugging])
fi
AC_MSG_CHECKING([for debug support])
AC_MSG_RESULT([${enable_debug:-no}])

test "x$prefix" = xNONE && prefix="$ac_default_prefix"
sharedir="$datadir"
eval "sharedir=\"$sharedir\""
eval "sharedir=\"$sharedir\""
eval "sharedir=\"$sharedir\""
AC_SUBST([sharedir])

xdgconfdir="$sysconfdir"
eval "xdgconfdir=\"$xdgconfdir\""
eval "xdgconfdir=\"$xdgconfdir\""
eval "xdgconfdir=\"$xdgconfdir\""
AC_SUBST([xdgconfdir])

xdgdatadir="$prefix"
eval "xdgdatadir=\"$xdgdatadir\""
eval "xdgdatadir=\"$xdgdatadir\""
eval "xdgdatadir=\"$xdgdatadir\""
AC_SUBST([xdgdatadir])

xdemoddir='${libdir}/xde/modules'
AC_SUBST([xdemoddir])

xdeincdir='${includedir}/xde'
AC_SUBST([xdeincdir])

pcdir='${libdir}/pkgconfig'
AC_SUBST([pcdir])

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 man/Makefile
		 data/Makefile
		 xde.pc
		 ])
AC_OUTPUT

# vim: set ft=config:
