#!/bin/bash

# Builds swfmill for 32-bit Windows

set -o nounset
set -o errexit
set -o pipefail

mkdir -p ~/i686-w64-mingw32/swfmill

pushd ~/i686-w64-mingw32/swfmill

/vagrant/configure --host=i686-w64-mingw32 \
	--prefix=/usr/local/i686-w64-mingw32 \
	--enable-static --disable-shared \
	--with-libiconv-prefix=/usr/local/i686-w64-mingw32 \
	XML_CFLAGS=-I/usr/local/i686-w64-mingw32/include/libxml2 \
	XML_LIBS="-lxml2" \
	XSLT_CFLAGS=-I/usr/local/i686-w64-mingw32/include/libxml2 \
	XSLT_LIBS="-lxml2 -lxslt" \
	EXSLT_CFLAGS=-I/usr/local/i686-w64-mingw32/include/libxml2 \
	EXSLT_LIBS="-lxml2 -lxslt -lexslt" \
	PNG_CFLAGS=-I/usr/local/i686-w64-mingw32/include/libpng12 \
	PNG_LIBS=-lpng16 \
	CPPFLAGS='-I/usr/local/i686-w64-mingw32/include' \
	LDFLAGS='-L/usr/local/i686-w64-mingw32/lib'

make

popd
