#include "xslt.h"

const char *swf_internal_stylesheets[6] = {
		"simple2lowlevel.xsl", xslt_simple2lowlevel,
		"svg2lowlevel.xsl", xslt_svg2lowlevel,
		NULL, NULL };