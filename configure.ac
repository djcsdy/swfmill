AC_PREREQ(2.54)

AC_INIT(swfmill, m4_esyscmd([ git describe | tr -d '\n' ]))
AM_INIT_AUTOMAKE([subdir-objects color-tests])


# Checks for programs.
AC_PROG_GREP
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

# cross-compile specifics
echo $host_os | $GREP mingw > /dev/null && IS_WINDOWS=yes
echo $host_os | $GREP darwin > /dev/null && IS_OSX=yes
AM_CONDITIONAL(IS_WINDOWS, [ test $IS_WINDOWS ])
AM_CONDITIONAL(IS_OSX, [ test $IS_OSX ])

AM_PROG_CC_C_O

AM_ICONV
AC_SUBST(ICONV_CONST)

if test $IS_OSX; then
	test $XML_CFLAGS || XML_CFLAGS=-I/usr/include/libxml2
	test $XML_LIBS || XML_LIBS=-lxml2
else
	PKG_CHECK_MODULES(XML, libxml-2.0)
fi
AC_SUBST(XML_CFLAGS)
AC_SUBST(XML_LIBS)

if test $IS_OSX; then
	test $XSLT_CFLAGS || XSLT_CFLAGS=-I/usr/include/libxml2
	test $XSLT_LIBS || XSLT_LIBS="-lxslt -lxml2"
else
	PKG_CHECK_MODULES(XSLT, libxslt)
fi
AC_SUBST(XSLT_CFLAGS)
AC_SUBST(XSLT_LIBS)

if test $IS_OSX; then
	test $EXSLT_CFLAGS || EXSLT_CFLAGS=-I/usr/include/libxml2
	test $EXSLT_LIBS || EXSLT_LIBS="-lexslt -lxslt -lxml2"
else
	PKG_CHECK_MODULES(EXSLT, libexslt)
fi
AC_SUBST(EXSLT_CFLAGS)
AC_SUBST(EXSLT_LIBS)

PKG_CHECK_MODULES(FREETYPE, freetype2)
AC_SUBST(FREETYPE_CFLAGS)
AC_SUBST(FREETYPE_LIBS)

PKG_CHECK_MODULES(PNG, libpng)
AC_SUBST(PNG_CFLAGS)
AC_SUBST(PNG_LIBS)

AC_OUTPUT([
	Makefile
	src/Makefile
	test/Makefile
	test/simple-import/Makefile
	test/xml/Makefile
	])
