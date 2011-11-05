#!/bin/sh

set -o nounset
set -o errexit

MTASC=`which mtasc` || true

if [ "${MTASC}" = "" ]; then
	echo "Skipping simple_import because mtasc is not installed." &&
	exit 77
fi

SWFMILL="../../src/swfmill"

if [ \! -d "build" ]; then
	mkdir "build"
fi

if [ \! -d "build/library" ]; then
	mkdir "build/library"
fi

cp "${srcdir}/test.swfml" "build"

cp "${srcdir}/library/star.swf" \
	"${srcdir}/library/testgradient.png" \
	"${srcdir}/library/testjpg.jpg" \
	"${srcdir}/library/testpng.png" \
	"${srcdir}/library/testpng8.png" \
	"${srcdir}/library/testpng24.png" \
	"${srcdir}/library/testpng-alpha.png" \
	"${srcdir}/library/vera.ttf" \
	"build/library"

${MTASC} -swf "build/classes.swf" -header 320:240:25 \
	-cp "${srcdir}/src" "${srcdir}/src/Main.as"

${SWFMILL} -v simple "build/test.swfml" "build/output.swf"
