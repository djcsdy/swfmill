
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

libswft_la_CFLAGS = $(XML_CFLAGS) $(XSLT_CFLAGS) $(EXSLT_CFLAGS) $(FREETYPE_CFLAGS) $(PNG_CFLAGS) $(ADDCFLAGS) -I$(top_srcdir)/src -I../
libswft_la_CXXFLAGS = $(libswft_la_CFLAGS)
libswft_la_LIBADD = $(XML_LIBS) $(XSLT_LIBS) $(EXSLT_LIBS) $(FREETYPE_LIBS) $(PNG_LIBS)
libswft_la_LDFLAGS = $(ADDLDFLAGS)

libswft_la_SOURCES = \
	swft.cpp \
	swft_document.cpp \
	swft_path.cpp \
	swft_import.cpp \
	swft_import_jpeg.cpp \
	swft_import_png.cpp \
	swft_import_ttf.cpp \
	swft_import_mp3.cpp \
	swft_import_wav.cpp \
	swft_import_binary.cpp \
	Parser.cpp \
	SVGStyle.cpp \
	SVGGradient.cpp \
	SVGColor.cpp \
	SVGPathParser.cpp \
	SVGPointsParser.cpp \
	SVGTransformParser.cpp \
	SVGAttributeParser.cpp \
	readpng.c \
	$(NULL)

noinst_HEADERS = \
	swft.h \
	Parser.h \
	SVGStyle.h \
	SVGGradient.h \
	SVGColor.h \
	SVGColors.h \
	SVGPathParser.h \
	SVGPointsParser.h \
	SVGTransformParser.h \
	SVGAttributeParser.h \
	readpng.h \
	$(NULL)
