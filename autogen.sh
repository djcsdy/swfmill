#!/bin/sh

set -x
aclocal -I autoconfig/m4
libtoolize --force --copy
autoheader
automake --add-missing --copy
autoconf
cd src
xsltproc xsl/mk.xsl xsl/source.xml
cd ..
./configure
