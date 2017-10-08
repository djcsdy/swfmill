#!/bin/bash

# Prepares the Vagrant virtual machine by installing software required to
# build swfmill.

set -o nounset
set -o errexit
set -o pipefail

sudo apt-get update

sudo apt-get install -y build-essential automake autoconf libtool \
	xsltproc pkg-config libxml2-dev libxslt-dev libfreetype6-dev \
	libpng-dev mtasc git mingw-w64 mingw-w64-tools binutils-mingw-w64 \
	g++-mingw-w64 mingw32-runtime tofrodos zip


# Download source packages for dependencies

mkdir -p ~/dependencies

pushd ~/dependencies

wget --continue 'https://www.swfmill.org/dependencies/libiconv-1.14.tar.gz' \
	'https://www.swfmill.org/dependencies/zlib-1.2.8.tar.gz' \
	'https://www.swfmill.org/dependencies/libpng-1.6.6.tar.gz' \
	'https://www.swfmill.org/dependencies/freetype-2.5.0.1.tar.gz' \
	'https://www.swfmill.org/dependencies/libxml2-2.9.1.tar.gz' \
	'https://www.swfmill.org/dependencies/libxslt-1.1.28.tar.gz' \
	&& wget_err=0 || wget_err=$?

# for some daft reason wget exits with code 8 when all files are already
# downloaded
if [[ ${wget_err} -ne 0 ]] && [[ ${wget_err} -ne 8 ]]; then
	echo "wget exited with error: ${wget_err}" >&2
	exit ${wget_err}
fi

tar zxf libiconv-1.14.tar.gz

tar zxf zlib-1.2.8.tar.gz

tar zxf libpng-1.6.6.tar.gz

tar zxf freetype-2.5.0.1.tar.gz

tar zxf libxml2-2.9.1.tar.gz

tar zxf libxslt-1.1.28.tar.gz

popd


# Install libiconv for 32-bit Windows

mkdir -p ~/i686-w64-mingw32/libiconv-1.14

pushd ~/i686-w64-mingw32/libiconv-1.14

~/dependencies/libiconv-1.14/configure --host=i686-w64-mingw32 \
	--prefix=/usr/local/i686-w64-mingw32 --enable-static \
	--disable-shared \
	CPPFLAGS='-I/usr/local/i686-w64-mingw32/include' \
	LDFLAGS='-L/usr/local/i686-w64-mingw32/lib'

make

sudo make install

popd


# Install zlib for 32-bit Windows

cp -r ~/dependencies/zlib-1.2.8 ~/i686-w64-mingw32/zlib-1.2.8

pushd ~/i686-w64-mingw32/zlib-1.2.8

CC=i686-w64-mingw32-gcc AR=i686-w64-mingw32-ar \
	RANLIB=i686-w64-mingw32-ranlib \
	CPPFLAGS='-I/usr/local/i686-w64-mingw32/include' \
	LDFLAGS='-L/usr/local/i686-w64-mingw32/lib' \
	./configure --static --prefix=/usr/local/i686-w64-mingw32

make

sudo make install

popd


# Install libpng for 32-bit Windows

mkdir -p ~/i686-w64-mingw32/libpng-1.6.6

pushd ~/i686-w64-mingw32/libpng-1.6.6

~/dependencies/libpng-1.6.6/configure --host=i686-w64-mingw32 \
	--prefix=/usr/local/i686-w64-mingw32 --enable-static \
	--disable-shared \
	CPPFLAGS='-I/usr/local/i686-w64-mingw32/include' \
	LDFLAGS='-L/usr/local/i686-w64-mingw32/lib'

make

sudo make install

popd


# Install Freetype for 32-bit Windows

mkdir -p ~/i686-w64-mingw32/freetype-2.5.0.1

pushd ~/i686-w64-mingw32/freetype-2.5.0.1

~/dependencies/freetype-2.5.0.1/configure --host=i686-w64-mingw32 \
	--prefix=/usr/local/i686-w64-mingw32 --enable-static \
	--disable-shared --without-bzip2 --without-png \
	CPPFLAGS='-I/usr/local/i686-w64-mingw32/include' \
	LDFLAGS='-L/usr/local/i686-w64-mingw32/lib'

make

sudo make install

popd


# Install libxml2 for 32-bit Windows

mkdir -p ~/i686-w64-mingw32/libxml2-2.9.1

pushd ~/i686-w64-mingw32/libxml2-2.9.1

~/dependencies/libxml2-2.9.1/configure --host=i686-w64-mingw32 \
	--prefix=/usr/local/i686-w64-mingw32 --enable-static \
	--disable-shared --without-readline --without-python \
	--with-iconv=/usr/local/i686-w64-mingw32 \
	--with-zlib=/usr/local/i686-w64-mingw32 \
	--without-lzma \
	CPPFLAGS='-I/usr/local/i686-w64-mingw32/include' \
	LDFLAGS='-L/usr/local/i686-w64-mingw32/lib'

make

sudo make install

popd


# Install libxslt for 32-bit Windows

mkdir -p ~/i686-w64-mingw32/libxslt-1.1.28

pushd ~/i686-w64-mingw32/libxslt-1.1.28

~/dependencies/libxslt-1.1.28/configure --host=i686-w64-mingw32 \
	--prefix=/usr/local/i686-w64-mingw32 --enable-static \
	--disable-shared --without-python \
	--with-libxml-prefix=/usr/local/i686-w64-mingw32 \
	--without-plugins \
	CPPFLAGS='-I/usr/local/i686-w64-mingw32/include -Dmkdir\(f,p\)=mkdir\(f\)' \
	LDFLAGS='-L/usr/local/i686-w64-mingw32/lib'

make

sudo make install

popd


