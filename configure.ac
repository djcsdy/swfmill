AC_PREREQ(2.54)

AC_INIT(swfmill, m4_esyscmd([ git describe | tr -d '\n' ]))
AM_INIT_AUTOMAKE([subdir-objects color-tests])

AC_CONFIG_MACRO_DIR([autoconfig/m4])

LT_INIT


# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
PKG_PROG_PKG_CONFIG
AM_PROG_CC_C_O
AC_PATH_PROG([XSLTPROC], [xsltproc])
AC_PATH_PROG([MTASC], [mtasc])

if test "x${XSLTPROC}" == "x"; then
	AC_MSG_ERROR([Please install xsltproc before trying to build swfmill])
fi


# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h unistd.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T


# Set flags if we're on Windows or Mac OS X, since we have some specific
# hard-coded behaviour for those platforms.
echo $host_os | $GREP mingw > /dev/null && IS_WINDOWS=yes
echo $host_os | $GREP darwin > /dev/null && IS_OSX=yes
AM_CONDITIONAL(IS_WINDOWS, [ test $IS_WINDOWS ])
AM_CONDITIONAL(IS_OSX, [ test $IS_OSX ])


# Checks for libraries

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


# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor memset pow sqrt strcasecmp strdup strrchr])


AC_OUTPUT([
	Makefile
	src/Makefile
	test/Makefile
	test/simple-import/Makefile
	test/xml/Makefile
	])
