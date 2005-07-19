# $Header: /home/dan/cvs/swfmill/src/Makefile.am,v 1.15 2005/05/22 16:18:42 dan Exp $

NULL = 
MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = \
	xslt_simple2lowlevel.xsl
	xslt_svg2lowlevel.xsl
	$(NULL)
	
lib_LTLIBRARIES = libswfmillxslt.la

libswfmillxslt_la_CFLAGS =
libswfmillxslt_la_CXXFLAGS = $(libswfmillxslt_la_CFLAGS)
libswfmillxslt_la_LIBADD =

BUILT_SOURCES = \
	xslt_simple2lowlevel.cpp \
	xslt_svg2lowlevel.cpp \
	$(NULL)
	
CLEAN_FILES = xslt_simple2lowlevel.cpp xsl_svg2lowlevel.cpp


# rule to include .xsl stylesheets as c files
%.cpp : %.xsl
	echo "#include \"xslt.h\"" > $@
	echo "const char *$* = " >> $@
	sed -e "s/\"/\\\\\"/g" -e "s/\(.*\)/\"\1\\\\n\"/g" $< >> $@
	echo ";" >> $@

libswfmillxslt_la_SOURCES = \
	xslt_simple2lowlevel.cpp \
	xslt_svg2lowlevel.cpp \
	$(NULL)

noinst_HEADERS = \
	xslt.h \
	$(NULL)
