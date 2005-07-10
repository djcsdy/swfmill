# $Header: /home/dan/cvs/swfmill/src/Makefile.am,v 1.15 2005/05/22 16:18:42 dan Exp $

NULL = 
MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = \
	xslt_library.xsl
	$(NULL)
	
bin_PROGRAMS = swfmill

if IS_WINDOWS
ADDCFLAGS = -DLIBXML_STATIC -DLIBXSLT_STATIC -DLIBEXSLT_STATIC
ADDLDFLAGS = -all-static
endif
if IS_OSX
ADDCFLAGS = -DLIBXML_STATIC -DLIBXSLT_STATIC -DLIBEXSLT_STATIC
ADDLDFLAGS = -all-static
endif

swfmill_CFLAGS = $(XML_CFLAGS) $(XSLT_CFLAGS) $(FREETYPE_CFLAGS) $(PNG_CFLAGS) $(CROCO_CFLAGS) $(ADDCFLAGS)
swfmill_CXXFLAGS = $(swfmill_CFLAGS)
swfmill_LDADD = $(XML_LIBS) $(XSLT_LIBS) $(FREETYPE_LIBS) $(PNG_LIBS) $(CROCO_LIBS)
swfmill_LDFLAGS = $(ADDLDFLAGS)

BUILT_SOURCES = \
	gSWFParseXML.cpp \
	gSWFWriteXML.cpp \
	gSWFParser.cpp \
	gSWFWriter.cpp \
	gSWFDumper.cpp \
	gSWFBasics.cpp \
	gSWFSize.cpp \
	xslt_simple2lowlevel.cpp \
	xslt_svg2lowlevel.cpp \
	$(NULL)
	
CLEAN_FILES = xslt_simple2lowlevel.cpp xsl_svg2lowlevel.cpp

# rule to build the generated sources from xsl
#gSWFParseXML.cpp gSWFWriteXML.cpp gSWFParser.cpp gSWFWriter.cpp gSWFDumper.cpp gSWFBasics.cpp gSWFSize.cpp : xsl/mk.xsl xsl/source.xml xsl/basic.xsl xsl/basics.xsl xsl/header.xsl xsl/parser.xsl xsl/parsexml.xsl xsl/size.xsl xsl/writer.xsl xsl/writexml.xsl
#	xsltproc xsl/mk.sh xsl/source.xml

# rule to include .xsl stylesheets as c files
%.cpp : %.xsl
	echo "#include \"xslt.h\"" > $@
	echo "const char *$* = " >> $@
	sed -e "s/\"/\\\\\"/g" -e "s/\(.*\)/\"\1\\\\n\"/g" $< >> $@
	echo ";" >> $@

swfmill_SOURCES = \
	base64.c \
	readpng.c \
	SWFReader.cpp \
	SWFWriter.cpp \
	SWFFile.cpp \
	SWFItem.cpp \
	SWFTag.cpp \
	SWFAction.cpp \
	SWFStackItem.cpp \
	SWFStyle.cpp \
	SWFShapeItem.cpp \
	SWFGlyphList.cpp \
	SWFShapeMaker.cpp \
	\
	gSWFParseXML.cpp \
	gSWFWriteXML.cpp \
	gSWFParser.cpp \
	gSWFWriter.cpp \
	gSWFDumper.cpp \
	gSWFBasics.cpp \
	gSWFSize.cpp \
	\
	swfmill.cpp \
	swft.cpp \
	swft_document.cpp \
	swft_path.cpp \
	swft_css.cpp \
	swft_import.cpp \
	swft_import_jpeg.cpp \
	swft_import_png.cpp \
	swft_import_ttf.cpp \
	xslt_simple2lowlevel.cpp \
	xslt_svg2lowlevel.cpp \
	$(NULL)

noinst_HEADERS = \
	base64.h \
	readpng.h \
	SWFBasic.h \
	SWFReader.h \
	SWFWriter.h \
	SWFFile.h \
	SWFItem.h \
	SWFTag.h \
	SWFAction.h \
	SWFStackItem.h \
	SWFStyle.h \
	SWFShapeItem.h \
	SWFGlyphList.h \
	SWFList.h \
	SWFShapeMaker.h \
	SWF.h \
	swft.h \
	xslt.h \
	$(NULL)
