#!/bin/sh

set -o nounset
set -o errexit

XML="${1}"
XML_BASENAME="`basename "${XML}"`"

XML_SWF="${XML_BASENAME}.swf"
XML_SWF_XML="${XML_SWF}.xml"

XML_DIFF="${XML_BASENAME}.diff"

"${SWFMILL}" xml2swf "${XML}" "${XML_SWF}"
"${SWFMILL}" swf2xml "${XML_SWF}" "${XML_SWF_XML}"

diff -u "${XML}" "${XML_SWF_XML}" > "${XML_DIFF}"
