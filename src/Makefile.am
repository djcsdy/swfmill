# $Header: /home/dan/cvs/swfmill/src/Makefile.am,v 1.15 2005/05/22 16:18:42 dan Exp $

NULL = 
MAINTAINERCLEANFILES = Makefile.in
	
bin_PROGRAMS = swfmill

if IS_WINDOWS
ADDCFLAGS = -DLIBXML_STATIC -DLIBXSLT_STATIC -DLIBEXSLT_STATIC
ADDLDFLAGS = -all-static
endif
if IS_OSX
ADDCFLAGS = -DLIBXML_STATIC -DLIBXSLT_STATIC -DLIBEXSLT_STATIC
ADDLDFLAGS = -all-static
endif

swfmill_CFLAGS = $(XML_CFLAGS) $(XSLT_CFLAGS) $(ADDCFLAGS) -I./swft/ -I./xslt/
swfmill_CXXFLAGS = $(swfmill_CFLAGS)
swfmill_LDADD = $(XML_LIBS) $(XSLT_LIBS) swft/libswft.la xslt/libswfmillxslt.la
swfmill_LDFLAGS = $(ADDLDFLAGS)

BUILT_SOURCES = \
	gSWFParseXML.cpp \
	gSWFWriteXML.cpp \
	gSWFParser.cpp \
	gSWFWriter.cpp \
	gSWFDumper.cpp \
	gSWFBasics.cpp \
	gSWFSize.cpp \
	$(NULL)
	
# rule to build the generated sources from xsl
#gSWFParseXML.cpp gSWFWriteXML.cpp gSWFParser.cpp gSWFWriter.cpp gSWFDumper.cpp gSWFBasics.cpp gSWFSize.cpp : xsl/mk.xsl xsl/source.xml xsl/basic.xsl xsl/basics.xsl xsl/header.xsl xsl/parser.xsl xsl/parsexml.xsl xsl/size.xsl xsl/writer.xsl xsl/writexml.xsl
#	xsltproc xsl/mk.sh xsl/source.xml

swfmill_SOURCES = \
	base64.c \
	SWFReader.cpp \
	SWFWriter.cpp \
	SWFFile.cpp \
	SWFItem.cpp \
	SWFTag.cpp \
	SWFAction.cpp \
	SWFFilter.cpp \
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
	$(NULL)

noinst_HEADERS = \
	base64.h \
	SWFBasic.h \
	SWFReader.h \
	SWFWriter.h \
	SWFFile.h \
	SWFItem.h \
	SWFTag.h \
	SWFAction.h \
	SWFFilter.h \
	SWFStackItem.h \
	SWFStyle.h \
	SWFShapeItem.h \
	SWFGlyphList.h \
	SWFList.h \
	SWFShapeMaker.h \
	SWF.h \
	$(NULL)

SUBDIRS = \
	swft \
	xslt \
	$(NULL)
