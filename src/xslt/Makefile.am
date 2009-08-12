# $Header: /home/dan/cvs/swfmill/src/Makefile.am,v 1.15 2005/05/22 16:18:42 dan Exp $

NULL = 
MAINTAINERCLEANFILES = Makefile.in simple.xsl

ALL_XSLT = \
	simple-tools.xslt \
	simple-elements.xslt \
	simple-import.xslt \
	simple-deprecated.xslt \
	simple-svg.xslt \
	$(NULL)
	
EXTRA_DIST = \
	assemble.xsl \
	simple.xml \
	$(ALL_XSLT) \
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
	
#CLEAN_FILES = $(BUILT_SOURCES)

NAMESPACES = xmlns:swft=\"http:\/\/subsignal.org\/swfml\/swft\" \
			 xmlns:str=\"http:\/\/exslt.org\/strings\" \
			 xmlns:math=\"http:\/\/exslt.org\/math\"

SUFFIXES = .cpp .xml

.xml.cpp: assemble.xsl $(ALL_XSLT)
	xsltproc $(srcdir)/assemble.xsl $< > $(@:.cpp=.xsl) 
	echo "#include \"xslt.h\"" > $@
	echo "const char *xslt_$* = " >> $@
	sed -e "s/namespaces=\\\"hack\\\"/$(NAMESPACES)/" -e "s/\"/\\\\\"/g" -e "s/\(.*\)/\"\1\\\\n\"/g" $(@:.cpp=.xsl) >> $@
	echo ";" >> $@

libswfmillxslt_la_SOURCES = \
	$(BUILT_SOURCES)
	$(NULL)

