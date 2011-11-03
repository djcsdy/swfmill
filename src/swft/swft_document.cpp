#include "SWFShapeMaker.h"
#include "SWFFile.h"
#include "swft.h"
#include <libxslt/extensions.h>
#include <libxml/uri.h>
#include <libxslt/xsltutils.h>
#include <libxml/xpathInternals.h>

using namespace SWF;

void swft_document( xmlXPathParserContextPtr ctx, int nargs ) {
	xsltTransformContextPtr tctx;
	char *filename;
	xsltDocumentPtr xsltdoc;
	xmlDocPtr doc = NULL;
	xmlXPathObjectPtr obj;
	xmlChar *base = NULL;
	char sig;
	File import;

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:document() : invalid arg expecting a string\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
	
	tctx = xsltXPathGetTransformContext(ctx);
	
	filename = swft_get_filename( obj->stringval, ctx->context->doc->URL);
	
	FILE *fp = fopen( filename, "rb" );
	if( !fp ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:document() : failed to read file '%s'\n", filename);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		goto fail;
	}
	
	import.load( fp );
	doc = import.getXML();
	
	if( !doc ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:document() : could not parse SWF '%s'\n", filename);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		goto fail;
	}
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );

fail:
	delete filename;
}
