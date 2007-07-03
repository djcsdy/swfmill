#!/bin/sh

set -x
aclocal -I autoconfig/m4
libtoolize --force --copy
autoheader
automake --add-missing --copy
autoconf
cd src
xsltproc codegen/mk.xsl codegen/source.xml
cd ..
./configure $*
