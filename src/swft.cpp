#include <libxslt/extensions.h>
#include <libxml/uri.h>
#include <libxslt/xsltutils.h>
#include <libxml/xpathInternals.h>
#include <stdlib.h>
#include <string.h>
#include "swft.h"

#include <string>
#include <stack>
#include <map>

#define TMP_STRLEN 0xFF
#define SWFT_MAPSIZE 32
#define SWFT_NAMESPACE ((const xmlChar*)"http://subsignal.org/swfml/swft")

class swft_ctx {
public:
	int last_id;
	std::stack<std::map<std::string,int>*> maps;
	
	void pushMap() {
		maps.push( new std::map<std::string,int> );
	}
	void popMap() {
		maps.pop();
	}
	
	int doMap( const char *oldID ) {
		std::map<std::string,int>& m = *(maps.top());
		int r = m[oldID];
		if( r == 0 ) {
			r = last_id++;
			m[oldID] = r;
		}
		return r;
	}
};


void *swft_init( xsltTransformContextPtr ctx, const xmlChar *URI );
void swft_shutdown( xsltTransformContextPtr ctx, const xmlChar *URI, void *data );

static void swft_nextid( xmlXPathParserContextPtr ctx, int nargs );

static void swft_pushmap( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr comp );
static void swft_popmap( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr comp );

static void swft_mapid( xmlXPathParserContextPtr ctx, int nargs );

// in swft_import_*.cpp
// FIXME why are these not static?
void swft_import_jpeg( xmlXPathParserContextPtr ctx, int nargs );
void swft_import_png( xmlXPathParserContextPtr ctx, int nargs );
void swft_import_ttf( xmlXPathParserContextPtr ctx, int nargs );

// in swft_document
void swft_document( xmlXPathParserContextPtr ctx, int nargs );

// in swft_path
void swft_path( xmlXPathParserContextPtr ctx, int nargs );

// in swft_css
void swft_css( xmlXPathParserContextPtr ctx, int nargs );


static void swft_nextid( xmlXPathParserContextPtr ctx, int nargs ) {
	char tmp[TMP_STRLEN];
	swft_ctx *c = (swft_ctx*)xsltGetExtData( xsltXPathGetTransformContext(ctx), SWFT_NAMESPACE );
	snprintf(tmp,TMP_STRLEN,"%i", c->last_id++ );
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



void swft_register() {
	xsltRegisterExtModule((const xmlChar *)SWFT_NAMESPACE,
		swft_init, swft_shutdown );
}

void *swft_init( xsltTransformContextPtr ctx, const xmlChar *URI ) {
	xsltRegisterExtFunction( ctx, (const xmlChar *) "next-id", SWFT_NAMESPACE, swft_nextid);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "map-id", SWFT_NAMESPACE, swft_mapid);
	xsltRegisterExtElement(  ctx, (const xmlChar *) "push-map", SWFT_NAMESPACE, swft_pushmap);
	xsltRegisterExtElement(  ctx, (const xmlChar *) "pop-map", SWFT_NAMESPACE, swft_popmap);

	xsltRegisterExtFunction( ctx, (const xmlChar *) "document", SWFT_NAMESPACE, swft_document);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "path", SWFT_NAMESPACE, swft_path);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "css", SWFT_NAMESPACE, swft_css);

	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-jpeg", SWFT_NAMESPACE, swft_import_jpeg );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-png", SWFT_NAMESPACE, swft_import_png );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-ttf", SWFT_NAMESPACE, swft_import_ttf );
	
	swft_ctx *c = new swft_ctx;
	c->last_id = 1;
	c->pushMap();
	return c;
}

void swft_shutdown( xsltTransformContextPtr ctx, const xmlChar *URI, void *data ) {
	swft_ctx *c = (swft_ctx*)data;
	delete c;
}

