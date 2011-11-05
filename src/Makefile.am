
NULL = 
MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = swfmill

if IS_WINDOWS
ADDCFLAGS = -DLIBXML_STATIC -DLIBXSLT_STATIC -DLIBEXSLT_STATIC
ADDLDFLAGS = -all-static
endif

swfmill_CFLAGS = $(XML_CFLAGS) $(XSLT_CFLAGS) $(EXSLT_CFLAGS) \
	$(FREETYPE_CFLAGS) $(PNG_CFLAGS) $(ADDCFLAGS) \
	-I$(srcdir)/swft/ -I$(srcdir)/xslt/
swfmill_CXXFLAGS = $(swfmill_CFLAGS)
swfmill_LDADD = $(XML_LIBS) $(XSLT_LIBS) $(EXSLT_LIBS) $(FREETYPE_LIBS) \
	$(PNG_LIBS) $(LTLIBICONV)
swfmill_LDFLAGS = $(ADDLDFLAGS)

CODEGEN_GENERATEDHEADERS = \
	SWF.h \
	$(NULL)

CODEGEN_GENERATEDSOURCES = \
	gSWFParseXML.cpp \
	gSWFWriteXML.cpp \
	gSWFParser.cpp \
	gSWFWriter.cpp \
	gSWFDumper.cpp \
	gSWFBasics.cpp \
	gSWFSize.cpp \
	$(NULL)

SIMPLE_DIALECT_GENERATEDXSL = \
	xslt/simple.xsl \
	$(NULL)

SIMPLE_DIALECT_GENERATEDSOURCES = \
	xslt/simple.cpp \
	$(NULL)

SIMPLE_DIALECT_XSLTS = \
	xslt/simple-tools.xslt \
	xslt/simple-elements.xslt \
	xslt/simple-import.xslt \
	xslt/simple-deprecated.xslt \
	xslt/simple-svg.xslt \
	$(NULL)

CODEGEN_SRC = \
	codegen/basic.xsl \
	codegen/basics.xsl \
	codegen/dumper.xsl \
	codegen/header.xsl \
	codegen/mk.xsl \
	codegen/parser.xsl \
	codegen/parsexml.xsl \
	codegen/size.xsl \
	codegen/source.xml \
	codegen/writer.xsl \
	codegen/writexml.xsl \
	$(NULL)

EXTRA_DIST = \
	$(CODEGEN_SRC) \
	\
	xslt/assemble.xsl \
	xslt/simple.xml \
	$(SIMPLE_DIALECT_XSLTS) \
	$(NULL)

BUILT_SOURCES = \
	$(CODEGEN_GENERATEDHEADERS) \
	$(NULL)

$(CODEGEN_GENERATEDSOURCES) $(CODEGEN_GENERATEDHEADERS): $(CODEGEN_SRC)
	xsltproc $(srcdir)/codegen/mk.xsl $(srcdir)/codegen/source.xml

NAMESPACES = xmlns:swft=\"http:\/\/subsignal.org\/swfml\/swft\" \
			 xmlns:str=\"http:\/\/exslt.org\/strings\" \
			 xmlns:math=\"http:\/\/exslt.org\/math\"

$(SIMPLE_DIALECT_GENERATEDSOURCES): $(SIMPLE_DIALECT_GENERATEDSOURCES:.cpp=.xml) xslt/assemble.xsl $(SIMPLE_DIALECT_XSLTS)
	xsltproc $(srcdir)/xslt/assemble.xsl $< > $(@:.cpp=.xsl)
	echo "#include \"xslt/xslt.h\"" > $@
	echo "const char *xslt_simple = " >> $@
	sed -e "s/namespaces=\\\"hack\\\"/$(NAMESPACES)/" -e "s/\"/\\\\\"/g" -e "s/\(.*\)/\"\1\\\\n\"/g" $(@:.cpp=.xsl) >> $@
	echo ";" >> $@

swfmill_SOURCES = \
	swft/swft.cpp \
	swft/swft_document.cpp \
	swft/swft_path.cpp \
	swft/swft_import.cpp \
	swft/swft_import_jpeg.cpp \
	swft/swft_import_png.cpp \
	swft/swft_import_ttf.cpp \
	swft/swft_import_mp3.cpp \
	swft/swft_import_wav.cpp \
	swft/swft_import_binary.cpp \
	swft/Parser.cpp \
	swft/SVGStyle.cpp \
	swft/SVGGradient.cpp \
	swft/SVGColor.cpp \
	swft/SVGPathParser.cpp \
	swft/SVGPointsParser.cpp \
	swft/SVGTransformParser.cpp \
	swft/SVGAttributeParser.cpp \
	swft/readpng.c \
	\
	base64.c \
	Geom.cpp \
	SWFReader.cpp \
	SWFWriter.cpp \
	SWFFile.cpp \
	SWFItem.cpp \
	SWFTag.cpp \
	SWFAction.cpp \
	SWFFilter.cpp \
	SWFOpCode.cpp \
	SWFTrait.cpp \
	SWFShapeItem.cpp \
	SWFGlyphList.cpp \
	SWFShapeMaker.cpp \
	\
	swfmill.cpp \
	$(NULL)

nodist_swfmill_SOURCES = \
	$(CODEGEN_GENERATEDSOURCES) \
	$(SIMPLE_DIALECT_GENERATEDSOURCES) \
	$(NULL)

noinst_HEADERS = \
	swft/swft.h \
	swft/Parser.h \
	swft/SVGStyle.h \
	swft/SVGGradient.h \
	swft/SVGColor.h \
	swft/SVGColors.h \
	swft/SVGPathParser.h \
	swft/SVGPointsParser.h \
	swft/SVGTransformParser.h \
	swft/SVGAttributeParser.h \
	swft/readpng.h \
	\
	xslt/xslt.h \
	\
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
	SWFOpCode.h \
	SWFTrait.h \
	SWFShapeItem.h \
	SWFGlyphList.h \
	SWFList.h \
	SWFShapeMaker.h \
	SWF.h \
	$(NULL)

CLEANFILES = \
	$(CODEGEN_GENERATEDHEADERS) \
	$(CODEGEN_GENERATEDSOURCES) \
	$(SIMPLE_DIALECT_GENERATEDXSL) \
	$(SIMPLE_DIALECT_GENERATEDSOURCES) \
	$(NULL)
