#!/bin/sh

set -o nounset
set -o errexit

XSL="${1}"
XML="`echo ${1} | "${SED}" 's/\.xsl$/.xml/'`"

XSL_BASENAME="`basename "${XSL}"`"

OUT_XML="${XSL_BASENAME}.out.xml"
VERIFY_XSL="${XSL}.verify.xsl"

"${SWFMILL}" xslt "${XSL}" "${XML}" "${OUT_XML}"

"${XSLTPROC}" "${VERIFY_XSL}" "${OUT_XML}"
