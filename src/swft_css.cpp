
#include <string>
#include <iostream>
#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>

using namespace std;
#define TMP_STRLEN 0xff


void swft_css( xmlXPathParserContextPtr ctx, int nargs ) {
	xmlChar *string;
	xmlXPathObjectPtr obj;
	xmlDocPtr doc;
	xmlNodePtr node;
	char tmp[TMP_STRLEN];
	bool quiet = xsltVariableLookup( xsltXPathGetTransformContext(ctx), (const xmlChar*)"quiet", NULL ) != NULL;

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:css() : invalid arg expecting a transformation string\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
		
	string = obj->stringval;

	//fprintf(stderr,"getting style from '%s'\n", string );

	doc = xmlNewDoc( (const xmlChar *)"1.0");
	node = doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"tmp", NULL );
	xmlSetProp( node, (const xmlChar *)"red", (const xmlChar *)"NOT YET IMPLEMENTED" );
	
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
}

void swft_unit( xmlXPathParserContextPtr ctx, int nargs ) {
	xmlChar *string;
	xmlXPathObjectPtr obj;
	xmlDocPtr doc;
	xmlNodePtr node;
	char tmp[TMP_STRLEN];

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:unit() : invalid arg, expecting a string\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
		
	string = obj->stringval;
	float val;
	
	if( sscanf( (const char*)string, "%fpt", &val ) == 1 ) {
	//  fprintf( stderr, "---- UNIT: %fpt -> %fpx \n", val, val*(100.0/80));
		snprintf(tmp,TMP_STRLEN,"%f", val * (100.0/80));
		
		valuePush( ctx, xmlXPathNewString( (const xmlChar *)tmp ) );
	} else {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:unit() : unknown unit: '%s'\n", (const char*)string );
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
}
