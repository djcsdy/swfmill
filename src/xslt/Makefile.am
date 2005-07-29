# $Header: /home/dan/cvs/swfmill/src/Makefile.am,v 1.15 2005/05/22 16:18:42 dan Exp $

NULL = 
MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = \
	assemble.xsl \
	simple.xml \
	simple-tools.xslt \
	simple-elements.xslt \
	simple-import.xslt \
	simple-deprecated.xslt \
	$(NULL)

noinst_HEADERS = \
	xslt.h \
	$(NULL)

lib_LTLIBRARIES = libswfmillxslt.la

libswfmillxslt_la_CFLAGS =
libswfmillxslt_la_CXXFLAGS = $(libswfmillxslt_la_CFLAGS)
libswfmillxslt_la_LIBADD =

BUILT_SOURCES = \
	simple.cpp \
	$(NULL)
	
CLEAN_FILES = $(BUILT_SOURCES)

NAMESPACES = xmlns:swft=\"http:\/\/subsignal.org\/swfml\/swft\" \
			 xmlns:str=\"http:\/\/exslt.org\/strings\" \
			 xmlns:math=\"http:\/\/exslt.org\/math\"

%.cpp : %.xml assemble.xsl
	xsltproc assemble.xsl $< > $(addsuffix .xsl, $(basename $<)) 
	echo "#include \"xslt.h\"" > $@
	echo "const char *xslt_$* = " >> $@
	sed -e "s/namespaces=\\\"hack\\\"/$(NAMESPACES)/" -e "s/\"/\\\\\"/g" -e "s/\(.*\)/\"\1\\\\n\"/g" $(addsuffix .xsl, $(basename $<)) >> $@
	echo ";" >> $@

libswfmillxslt_la_SOURCES = \
	$(BUILT_SOURCES)
	$(NULL)

