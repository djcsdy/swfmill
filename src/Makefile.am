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

swfmill_CFLAGS = $(XML_CFLAGS) $(XSLT_CFLAGS) $(ADDCFLAGS) -I$(srcdir)/swft/ -I$(srcdir)/xslt/
swfmill_CXXFLAGS = $(swfmill_CFLAGS)
swfmill_LDADD = $(XML_LIBS) $(XSLT_LIBS) swft/libswft.la xslt/libswfmillxslt.la
swfmill_LDFLAGS = $(ADDLDFLAGS)

BUILT_SOURCES = \
	SWF.h \
	$(NULL)

GENERATED_FILES = \
	gSWFParseXML.cpp \
	gSWFWriteXML.cpp \
	gSWFParser.cpp \
	gSWFWriter.cpp \
	gSWFDumper.cpp \
	gSWFBasics.cpp \
	gSWFSize.cpp \
	$(NULL)
	
$(GENERATED_FILES) $(BUILT_SOURCES):
	xsltproc $(srcdir)/codegen/mk.xsl $(srcdir)/codegen/source.xml

swfmill_SOURCES = \
	base64.c \
	Geom.cpp \
	SWFReader.cpp \
	SWFWriter.cpp \
	SWFFile.cpp \
	SWFItem.cpp \
	SWFTag.cpp \
	SWFAction.cpp \
	SWFFilter.cpp \
	SWFTrait.cpp \
	SWFShapeItem.cpp \
	SWFGlyphList.cpp \
	SWFShapeMaker.cpp \
	\
	swfmill.cpp \
	$(NULL)

nodist_swfmill_SOURCES = $(GENERATED_FILES)

noinst_HEADERS = \
	base64.h \
	Geom.h \
	SWFBasic.h \
	SWFReader.h \
	SWFWriter.h \
	SWFFile.h \
	SWFItem.h \
	SWFIdItem.h \
	SWFIdItems.h \
	SWFTag.h \
	SWFAction.h \
	SWFFilter.h \
	SWFTrait.h \
	SWFShapeItem.h \
	SWFGlyphList.h \
	SWFList.h \
	SWFShapeMaker.h \
	SWF.h \
	$(NULL)

SUBDIRS = \
	swft \
	xslt \
	codegen \
	$(NULL)


CLEANFILES = $(BUILT_SOURCES) $(GENERATED_FILES)
