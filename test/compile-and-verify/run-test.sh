#!/bin/sh

set -o nounset
set -o errexit

XML="${1}"
XML_BASENAME="`basename "${XML}"`"

XML_SWF="${XML_BASENAME}.swf"
XML_SWF_XML="${XML_SWF}.xml"
VERIFY_XSL="${XML}.verify.xsl"

"${SWFMILL}" simple "${XML}" "${XML_SWF}"
"${SWFMILL}" swf2xml "${XML_SWF}" "${XML_SWF_XML}"

"${XSLTPROC}" "${VERIFY_XSL}" "${XML_SWF_XML}"
