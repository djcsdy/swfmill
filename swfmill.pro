TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    src/SWFTrait.cpp \
    src/SWFTag.cpp \
    src/SWFStyle.cpp \
    src/SWFStackItem.cpp \
    src/SWFShapeMaker.cpp \
    src/SWFShapeItem.cpp \
    src/SWFReader.cpp \
    src/SWFOpCode.cpp \
    src/swfmill.cpp \
    src/SWFItem.cpp \
    src/SWFImportFreetype.cpp \
    src/SWFImportExport.cpp \
    src/SWFGlyphList.cpp \
    src/SWFFilter.cpp \
    src/SWFFile.cpp \
    src/SWFAction.cpp \
    src/Geom.cpp \
    src/base64.c \
    src/SWFWriter.cpp \
    src/swft/swft.cpp \
    src/swft/swft_path.cpp \
    src/swft/swft_import.cpp \
    src/swft/swft_import_wav.cpp \
    src/swft/swft_import_ttf.cpp \
    src/swft/swft_import_png.cpp \
    src/swft/swft_import_mp3.cpp \
    src/swft/swft_import_jpeg.cpp \
    src/swft/swft_import_binary.cpp \
    src/swft/swft_document.cpp \
    src/swft/SVGTransformParser.cpp \
    src/swft/SVGStyle.cpp \
    src/swft/SVGPointsParser.cpp \
    src/swft/SVGPathParser.cpp \
    src/swft/SVGGradient.cpp \
    src/swft/SVGColor.cpp \
    src/swft/SVGAttributeParser.cpp \
    src/swft/readpng.c \
    src/swft/Parser.cpp \
    src/xslt/xslt_pack.cpp \
    src/gSWFWriteXML.cpp \
    src/gSWFWriter.cpp \
    src/gSWFSize.cpp \
    src/gSWFParseXML.cpp \
    src/gSWFParser.cpp \
    src/gSWFDumper.cpp \
    src/gSWFBasics.cpp

OTHER_FILES += \
    src/Makefile.am \
    src/codegen/writexml.xsl \
    src/codegen/writer.xsl \
    src/codegen/source.xml \
    src/codegen/size.xsl \
    src/codegen/parsexml.xsl \
    src/codegen/parser.xsl \
    src/codegen/mk.xsl \
    src/codegen/header.xsl \
    src/codegen/dumper.xsl \
    src/codegen/basics.xsl \
    src/codegen/basic.xsl \
    src/xslt/simple.xml \
    src/xslt/simple-tools.xslt \
    src/xslt/simple-svg.xslt \
    src/xslt/simple-import.xslt \
    src/xslt/simple-elements.xslt \
    src/xslt/simple-deprecated.xslt \
    src/xslt/README \
    src/xslt/assemble.xsl \
    test/Makefile.am \
    test/compile-and-verify/placeobject3.swf \
    test/compile-and-verify/placeobject3-import.xml.verify.xsl \
    test/compile-and-verify/placeobject3-import.xml \
    test/compile-and-verify/Makefile.am \
    test/compile-and-verify/import-happy-ttf.xml.verify.xsl \
    test/compile-and-verify/import-happy-ttf.xml \
    test/compile-and-verify/run-test.sh \
    test/old/Makefile.am \
    test/old/input.xml \
    test/old/expected-output.xml \
    test/old/test.sh \
    test/transform-and-verify/test.jpg \
    test/transform-and-verify/test-alpha.png \
    test/transform-and-verify/run-test.sh \
    test/transform-and-verify/Makefile.am \
    test/transform-and-verify/import-ttf.xsl.verify.xsl \
    test/transform-and-verify/import-ttf.xsl \
    test/transform-and-verify/import-ttf.xml \
    test/transform-and-verify/import-png.xsl.verify.xsl \
    test/transform-and-verify/import-png.xsl \
    test/transform-and-verify/vera.ttf \
    test/transform-and-verify/test.png \
    test/transform-and-verify/import-png.xml \
    test/transform-and-verify/import-png-with-alpha.xsl.verify.xsl \
    test/transform-and-verify/import-png-with-alpha.xsl \
    test/transform-and-verify/import-png-with-alpha.xml \
    test/transform-and-verify/import-jpega.xsl.verify.xsl \
    test/transform-and-verify/import-jpega.xsl \
    test/transform-and-verify/import-jpega.xml \
    test/transform-and-verify/import-jpeg.xsl.verify.xsl \
    test/transform-and-verify/import-jpeg.xsl \
    test/transform-and-verify/import-jpeg.xml \
    test/transform-and-verify/alpha.png \
    test/xml-swf-round-trip/text-test.xml \
    test/xml-swf-round-trip/text_sizes.xml \
    test/xml-swf-round-trip/text_formatting.xml \
    test/xml-swf-round-trip/TestFunction2.xml \
    test/xml-swf-round-trip/test.xml \
    test/xml-swf-round-trip/test_undefined_v7.xml \
    test/xml-swf-round-trip/test_undefined_v6.xml \
    test/xml-swf-round-trip/test_string.xml \
    test/xml-swf-round-trip/test_shape_tweening.xml \
    test/xml-swf-round-trip/test_shape_tweening-2.xml \
    test/xml-swf-round-trip/test_rotation2.xml \
    test/xml-swf-round-trip/test_rotation.xml \
    test/xml-swf-round-trip/test_rotation_shear.xml \
    test/xml-swf-round-trip/test_long_static_text.xml \
    test/xml-swf-round-trip/test_gradients_no_alpha.xml \
    test/xml-swf-round-trip/test_gradients_alpha.xml \
    test/xml-swf-round-trip/test_gradient_tweening.xml \
    test/xml-swf-round-trip/test_goto_stop.xml \
    test/xml-swf-round-trip/test_goto_play.xml \
    test/xml-swf-round-trip/test_goto_frame.xml \
    test/xml-swf-round-trip/test_function2.xml \
    test/xml-swf-round-trip/test_frame2.xml \
    test/xml-swf-round-trip/test_frame1.xml \
    test/xml-swf-round-trip/test_forin_array.xml \
    test/xml-swf-round-trip/test_colour_tweening.xml \
    test/xml-swf-round-trip/test_clipping_layer.xml \
    test/xml-swf-round-trip/test_cascaded_clipbuttons.xml \
    test/xml-swf-round-trip/test_button_functions.xml \
    test/xml-swf-round-trip/test_basic_types.xml \
    test/xml-swf-round-trip/test_action_order2.xml \
    test/xml-swf-round-trip/test_action_order.xml \
    test/xml-swf-round-trip/subshapes.xml \
    test/xml-swf-round-trip/sr2_title.xml \
    test/xml-swf-round-trip/sound1.xml \
    test/xml-swf-round-trip/slider.xml \
    test/xml-swf-round-trip/shared-fonts.xml \
    test/xml-swf-round-trip/run-test.sh \
    test/xml-swf-round-trip/player.xml \
    test/xml-swf-round-trip/movieclip_test.xml \
    test/xml-swf-round-trip/money3.xml \
    test/xml-swf-round-trip/Makefile.am \
    test/xml-swf-round-trip/input-fields.xml \
    test/xml-swf-round-trip/visible_and_transparency.xml \
    test/xml-swf-round-trip/text-test2.xml \
    test/xml-swf-round-trip/gradient-tests.xml \
    test/xml-swf-round-trip/GotoAndPlayTest.xml \
    test/xml-swf-round-trip/extended_clipping_test_1.xml \
    test/xml-swf-round-trip/event-test1.xml \
    test/xml-swf-round-trip/dlist_test1.xml \
    test/xml-swf-round-trip/display_list_test.xml \
    test/xml-swf-round-trip/counter.xml \
    test/xml-swf-round-trip/clip_as_button2.xml \
    test/xml-swf-round-trip/clip_as_button.xml \
    test/xml-swf-round-trip/car_smash.xml \
    test/xml-swf-round-trip/as3.xml \
    test/old/library/vera.ttf \
    test/old/library/testpng24.png \
    test/old/library/testpng8.png \
    test/old/library/testpng.png \
    test/old/library/testpng-alpha.png \
    test/old/library/testjpg.jpg \
    test/old/library/testgradient.png \
    test/old/library/star.swf \
    test/old/src/org/swfmill/Foo.as \
    test/old/src/Main.as

HEADERS += \
    src/SWFTrait.h \
    src/SWFTag.h \
    src/SWFStyle.h \
    src/SWFStackItem.h \
    src/SWFShapeMaker.h \
    src/SWFShapeItem.h \
    src/SWFReader.h \
    src/SWFOpCode.h \
    src/SWFList.h \
    src/SWFItem.h \
    src/SWFImportExport.h \
    src/SWFIdItems.h \
    src/SWFIdItem.h \
    src/SWFGlyphList.h \
    src/SWFFilter.h \
    src/SWFFile.h \
    src/SWFBasic.h \
    src/SWFAction.h \
	src/Geom.h \
    src/base64.h \
    src/SWFWriter.h \
    src/swft/swft.h \
    src/swft/SVGTransformParser.h \
    src/swft/SVGStyle.h \
    src/swft/SVGPointsParser.h \
    src/swft/SVGPathParser.h \
    src/swft/SVGGradient.h \
    src/swft/SVGColors.h \
    src/swft/SVGColor.h \
    src/swft/SVGAttributeParser.h \
    src/swft/readpng.h \
    src/swft/Parser.h \
    src/xslt/xslt.h \
    src/gSWF.h


unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libxml-2.0
unix: PKGCONFIG += libxslt
unix: PKGCONFIG += libexslt
unix: PKGCONFIG += libpng12

DEFINES += ICONV_CONST=""
