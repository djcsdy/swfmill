#!/bin/bash
VERSION=$1

if [ -z "$VERSION" ]; then
	echo usage: $0 [version]
	exit 1
fi

echo compiling swfmill $VERSION for macosx
wget http://swfmill.org/pre/swfmill-$VERSION.tar.gz
tar xvfz swfmill-$VERSION.tar.gz
cd swfmill-$VERSION
./configure
make
echo "Linking Again (workaround for static linkin crt0 problem)"
cd src
g++ -g -O2 -o swfmill swfmill-base64.o swfmill-SWFReader.o swfmill-SWFWriter.o swfmill-SWFFile.o swfmill-SWFItem.o swfmill-SWFTag.o swfmill-SWFAction.o swfmill-SWFStackItem.o swfmill-SWFStyle.o swfmill-SWFShapeItem.o swfmill-SWFGlyphList.o swfmill-SWFShapeMaker.o swfmill-gSWFParseXML.o swfmill-gSWFWriteXML.o swfmill-gSWFParser.o swfmill-gSWFWriter.o swfmill-gSWFDumper.o swfmill-gSWFBasics.o swfmill-gSWFSize.o swfmill-swfmill.o -Wl,-bind_at_load  -L/Users/dan/usr//lib swft/.libs/libswft.a /Users/dan/usr//lib/libexslt.a /Users/dan/usr//lib/libxslt.a /Users/dan/usr//lib/libxml2.a -ldl -lpthread /usr/lib/libiconv.dylib /Users/dan/usr//lib/libfreetype.a /Users/dan/usr//lib/libpng12.a -lz -lm xslt/.libs/libswfmillxslt.a
strip swfmill
cd ../..
mkdir swfmill-$VERSION-macosx
cp swfmill-$VERSION/README swfmill-$VERSION-macosx/
cp swfmill-$VERSION/COPYING swfmill-$VERSION-macosx/
cp swfmill-$VERSION/NEWS swfmill-$VERSION-macosx/
cp swfmill-$VERSION/src/swfmill swfmill-$VERSION-macosx/

tar cvfz swfmill-$VERSION-macosx.tar.gz swfmill-$VERSION-macosx/

echo "Copying release (swfmill-$VERSION-macosx.tar.gz) to swfmill.org:/tmp/"
scp swfmill-$VERSION-macosx.tar.gz swfmill.org:/tmp/
