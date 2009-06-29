#!/bin/sh

set -x
aclocal -I autoconfig/m4
libtoolize --force --copy
automake --add-missing --copy
autoconf
