#include <libxslt/extensions.h>
#include <libxml/uri.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>
#include <stdlib.h>
#include <string.h>
#include "swft.h"

#define TMP_STRLEN 0xFF
#define SWFT_MAPSIZE 32


void *swft_init( xsltTransformContextPtr ctx, const xmlChar *URI );
void swft_shutdown( xsltTransformContextPtr ctx, const xmlChar *URI, void *data );

static void swft_error( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr comp );

static void swft_nextid( xmlXPathParserContextPtr ctx, int nargs );
static void swft_nextdepth( xmlXPathParserContextPtr ctx, int nargs );

static void swft_pushmap( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr comp );
static void swft_popmap( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr comp );

static void swft_mapid( xmlXPathParserContextPtr ctx, int nargs );
static void swft_setmap( xmlXPathParserContextPtr ctx, int nargs );

// in swft_import_*.cpp
// FIXME why are these not static? any reason?
void swft_import_jpeg( xmlXPathParserContextPtr ctx, int nargs );
void swft_import_jpega( xmlXPathParserContextPtr ctx, int nargs );
void swft_import_png( xmlXPathParserContextPtr ctx, int nargs );
void swft_import_ttf( xmlXPathParserContextPtr ctx, int nargs );

// in swft_document
void swft_document( xmlXPathParserContextPtr ctx, int nargs );

// in swft_path
void swft_path( xmlXPathParserContextPtr ctx, int nargs );
void swft_bounds( xmlXPathParserContextPtr ctx, int nargs );
void swft_transform( xmlXPathParserContextPtr ctx, int nargs );

// in swft_css
void swft_css( xmlXPathParserContextPtr ctx, int nargs );
void swft_unit( xmlXPathParserContextPtr ctx, int nargs );
void swft_css_lookup( xmlXPathParserContextPtr ctx, int nargs );

static void swft_nextid( xmlXPathParserContextPtr ctx, int nargs ) {
	char tmp[TMP_STRLEN];
	swft_ctx *c = (swft_ctx*)xsltGetExtData( xsltXPathGetTransformContext(ctx), SWFT_NAMESPACE );
	snprintf(tmp,TMP_STRLEN,"%i", c->last_id++ );
	valuePush(ctx, xmlXPathNewString((const xmlChar *)tmp));
}

static void swft_nextdepth( xmlXPathParserContextPtr ctx, int nargs ) {
	char tmp[TMP_STRLEN];
	swft_ctx *c = (swft_ctx*)xsltGetExtData( xsltXPathGetTransformContext(ctx), SWFT_NAMESPACE );
	snprintf(tmp,TMP_STRLEN,"%i", c->last_depth++ );
	valuePush(ctx, xmlXPathNewString((const xmlChar *)tmp));
}

static void swft_pushmap( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr comp ) {
	swft_ctx *c = (swft_ctx*)xsltGetExtData( ctx, SWFT_NAMESPACE );
	c->pushMap();
}

static void swft_popmap( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr comp ) {
	swft_ctx *c = (swft_ctx*)xsltGetExtData( ctx, SWFT_NAMESPACE );
	c->popMap();
}

static void swft_setmap( xmlXPathParserContextPtr ctx, int nargs ) {
	swft_ctx *c = (swft_ctx*)xsltGetExtData( xsltXPathGetTransformContext(ctx), SWFT_NAMESPACE );

	if( (nargs != 2) ) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	int to = (int)xmlXPathPopNumber(ctx);
	xmlChar *from = xmlXPathPopString(ctx);
	if( xmlXPathCheckError(ctx) )
		return;
	
	c->setMap((const char *)from,to);
	
	xmlFree( from );
	valuePush(ctx, xmlXPathNewString((const xmlChar *)""));
}

static void swft_mapid( xmlXPathParserContextPtr ctx, int nargs ) {
	char tmp[TMP_STRLEN];
	xmlXPathObjectPtr obj;
	xmlChar *oldID;
	int newID;
	swft_ctx *c = (swft_ctx*)xsltGetExtData( xsltXPathGetTransformContext(ctx), SWFT_NAMESPACE );

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:mapid() : invalid arg expecting a string (the old id)\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
	// (int)xmlXPathStringEvalNumber()
	oldID = obj->stringval;
	newID = c->doMap((const char*)oldID);
	
	xmlFree( oldID );
	
	snprintf(tmp,TMP_STRLEN,"%i", newID );
	valuePush(ctx, xmlXPathNewString((const xmlChar *)tmp));
}

unsigned char *swft_get_filename( xmlChar *filenameUTF ) {
	int l = strlen((const char*)filenameUTF);
	int l2 = l;
	unsigned char *filename = (unsigned char *)malloc(l+1);
	UTF8Toisolat1( filename, &l2, filenameUTF, &l );
	filename[l2] = 0;
	return filename;
}

void swft_register() {
	xsltRegisterExtModule((const xmlChar *)SWFT_NAMESPACE,
		swft_init, swft_shutdown );
}

void *swft_init( xsltTransformContextPtr ctx, const xmlChar *URI ) {
	xsltRegisterExtFunction( ctx, (const xmlChar *) "next-id", SWFT_NAMESPACE, swft_nextid);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "next-depth", SWFT_NAMESPACE, swft_nextdepth);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "map-id", SWFT_NAMESPACE, swft_mapid);
	xsltRegisterExtElement(  ctx, (const xmlChar *) "push-map", SWFT_NAMESPACE, swft_pushmap);
	xsltRegisterExtElement(  ctx, (const xmlChar *) "pop-map", SWFT_NAMESPACE, swft_popmap);
	xsltRegisterExtFunction(  ctx, (const xmlChar *) "set-map", SWFT_NAMESPACE, swft_setmap);

	xsltRegisterExtFunction( ctx, (const xmlChar *) "document", SWFT_NAMESPACE, swft_document);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "path", SWFT_NAMESPACE, swft_path);
//	xsltRegisterExtFunction( ctx, (const xmlChar *) "bounds", SWFT_NAMESPACE, swft_bounds);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "css", SWFT_NAMESPACE, swft_css);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "css-lookup", SWFT_NAMESPACE, swft_css_lookup);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "unit", SWFT_NAMESPACE, swft_unit);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "transform", SWFT_NAMESPACE, swft_transform);

	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-jpeg", SWFT_NAMESPACE, swft_import_jpeg );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-jpega", SWFT_NAMESPACE, swft_import_jpega );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-png", SWFT_NAMESPACE, swft_import_png );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-ttf", SWFT_NAMESPACE, swft_import_ttf );
	
	swft_ctx *c = new swft_ctx;
	return c;
}

void swft_shutdown( xsltTransformContextPtr ctx, const xmlChar *URI, void *data ) {
	swft_ctx *c = (swft_ctx*)data;
	delete c;
}

