#!/bin/sh

set -x
aclocal -I autoconfig/m4
libtoolize --force --copy
automake --foreign --add-missing --copy
autoconf
