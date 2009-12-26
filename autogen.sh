#!/bin/sh

if which glibtoolize; then
	LIBTOOLIZE=glibtoolize
else
	LIBTOOLIZE=libtoolize
fi

set -x
aclocal -I autoconfig/m4
$LIBTOOLIZE --force --copy
automake --foreign --add-missing --copy
autoconf
