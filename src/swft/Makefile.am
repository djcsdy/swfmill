# $Header: /home/dan/cvs/swfmill/src/Makefile.am,v 1.15 2005/05/22 16:18:42 dan Exp $

NULL = 
MAINTAINERCLEANFILES = Makefile.in

if IS_WINDOWS
ADDCFLAGS = -DLIBXML_STATIC -DLIBXSLT_STATIC -DLIBEXSLT_STATIC
ADDLDFLAGS = -all-static
endif
if IS_OSX
#ADDCFLAGS = -DLIBXML_STATIC -DLIBXSLT_STATIC -DLIBEXSLT_STATIC
#ADDLDFLAGS = -all-static
endif

lib_LTLIBRARIES = libswft.la

libswft_la_CFLAGS = $(XML_CFLAGS) $(XSLT_CFLAGS) $(FREETYPE_CFLAGS) $(PNG_CFLAGS) $(ADDCFLAGS) -I../
libswft_la_CXXFLAGS = $(libswft_la_CFLAGS)
libswft_la_LIBADD = $(XML_LIBS) $(XSLT_LIBS) $(FREETYPE_LIBS) $(PNG_LIBS)
libswft_la_LDFLAGS = $(ADDLDFLAGS)

libswft_la_SOURCES = \
	swft.cpp \
	swft_document.cpp \
	swft_path.cpp \
	swft_css.cpp \
	swft_import.cpp \
	swft_import_jpeg.cpp \
	swft_import_png.cpp \
	swft_import_ttf.cpp \
	readpng.c \
	$(NULL)

noinst_HEADERS = \
	swft.h \
	readpng.h \
	$(NULL)
