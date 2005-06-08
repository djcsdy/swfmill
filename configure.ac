# $Id: configure.ac,v 1.29 2005/06/01 10:16:06 dan Exp $
AC_PREREQ(2.54)

AC_INIT( swfmill, 0.2.5, dan@subsignal.org )
AM_INIT_AUTOMAKE( AC_PACKAGE_NAME, AC_PACKAGE_VERSION )

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

dnl Check for pkgconfig first
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, yes, no)

dnl Give error and exit if we don't have pkgconfig
if test "x$HAVE_PKGCONFIG" = "xno"; then
  AC_MSG_ERROR(you need to have pkgconfig installed !)
fi

# cross-compile specifics
AM_CONDITIONAL(IS_WINDOWS, [ echo $host_os | grep mingw > /dev/null ])
AM_CONDITIONAL(IS_OSX, [ echo $host_os | grep darwin > /dev/null ])

# Checks for libraries.
PKG_CHECK_MODULES(XML, libxml-2.0)
AC_SUBST(XML_CFLAGS)
AC_SUBST(XML_LIBS)

PKG_CHECK_MODULES(XSLT, libexslt)
AC_SUBST(XSLT_CFLAGS)
AC_SUBST(XSLT_LIBS)

PKG_CHECK_MODULES(FREETYPE, freetype2)
AC_SUBST(FREETYPE_CFLAGS)
AC_SUBST(FREETYPE_LIBS)

PKG_CHECK_MODULES(PNG, libpng)
AC_SUBST(PNG_CFLAGS)
AC_SUBST(PNG_LIBS)

AC_OUTPUT([
	Makefile
	src/Makefile
	])
