#include <libxslt/extensions.h>
#include <libxml/uri.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>
#include <cstdlib>
#include <cstring>
#include "swft.h"

#define TMP_STRLEN 0xFF
#define SWFT_MAPSIZE 32

using namespace SWF;

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
void swft_import_mp3( xmlXPathParserContextPtr ctx, int nargs );
void swft_import_wav( xmlXPathParserContextPtr ctx, int nargs );
void swft_import_binary( xmlXPathParserContextPtr ctx, int nargs );

// in swft_document
void swft_document( xmlXPathParserContextPtr ctx, int nargs );

// in swft_path
void swft_path( xmlXPathParserContextPtr ctx, int nargs );
void swft_bounds( xmlXPathParserContextPtr ctx, int nargs );
void swft_transform( xmlXPathParserContextPtr ctx, int nargs );

static void swft_pushgradient( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr com ) {
	swft_ctx *c = (swft_ctx*)xsltGetExtData( ctx, SWFT_NAMESPACE );
	xmlChar *id, *href;
	SVGGradient *gradient;

	id = xmlGetProp(node, (const xmlChar *)"id");
	if(id) {
		if(!xmlStrcmp(node->name, (const xmlChar *)"linearGradient")) {
			gradient = new SVGLinearGradient();
		} else if(!xmlStrcmp(node->name, (const xmlChar *)"radialGradient")) {
			gradient = new SVGRadialGradient();
		}

		href = xmlGetProp(node, (const xmlChar *)"href");
		if (href) {
			string hrefStr = (const char *)href;
			hrefStr.erase(0, 1);

			map<string, SVGGradient*>::iterator i = c->gradients.find(hrefStr);
			if(i != c->gradients.end()) {
				*gradient = *((*i).second);
			}			

			xmlFree(href);
		}

		gradient->parse(node);
		c->gradients[(char *)id] = gradient;
		xmlFree(id);
	}

}

static void swft_popstyle( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr com ) {
	swft_ctx *c = (swft_ctx*)xsltGetExtData( ctx, SWFT_NAMESPACE );
	c->styles.pop();
}

static void swft_pushstyle( xsltTransformContextPtr ctx, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr com ) {
	swft_ctx *c = (swft_ctx*)xsltGetExtData( ctx, SWFT_NAMESPACE );

	SVGStyle style;
	if(c->styles.size() > 0) {
		style = c->styles.top();
	}
	style.parseNode(node, c->gradients);
	c->styles.push(style);
}

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

static void swft_bump_id( xmlXPathParserContextPtr ctx, int nargs ) {
	swft_ctx *c = (swft_ctx*)xsltGetExtData( xsltXPathGetTransformContext(ctx), SWFT_NAMESPACE );

	if( (nargs != 1) ) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	int offset = (int)xmlXPathPopNumber(ctx);
	if( xmlXPathCheckError(ctx) )
		return;
	
	if( offset >= c->last_id ) c->last_id = offset+1;
	
	valuePush(ctx, xmlXPathNewString((const xmlChar *)""));
}

static void swft_bump_depth( xmlXPathParserContextPtr ctx, int nargs ) {
	swft_ctx *c = (swft_ctx*)xsltGetExtData( xsltXPathGetTransformContext(ctx), SWFT_NAMESPACE );

	if( (nargs != 1) ) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	int offset = (int)xmlXPathPopNumber(ctx);
	if( xmlXPathCheckError(ctx) )
		return;
	
	if( offset >= c->last_depth ) c->last_depth = offset+1;
	
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
	xsltRegisterExtFunction(  ctx, (const xmlChar *) "bump-id", SWFT_NAMESPACE, swft_bump_id);
	xsltRegisterExtFunction(  ctx, (const xmlChar *) "bump-depth", SWFT_NAMESPACE, swft_bump_depth);

	xsltRegisterExtFunction( ctx, (const xmlChar *) "document", SWFT_NAMESPACE, swft_document);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "path", SWFT_NAMESPACE, swft_path);
//	xsltRegisterExtFunction( ctx, (const xmlChar *) "bounds", SWFT_NAMESPACE, swft_bounds);
	xsltRegisterExtFunction( ctx, (const xmlChar *) "transform", SWFT_NAMESPACE, swft_transform);
	xsltRegisterExtElement( ctx, (const xmlChar *) "push-style", SWFT_NAMESPACE, swft_pushstyle);
	xsltRegisterExtElement( ctx, (const xmlChar *) "pop-style", SWFT_NAMESPACE, swft_popstyle);
	xsltRegisterExtElement( ctx, (const xmlChar *) "push-gradient", SWFT_NAMESPACE, swft_pushgradient);

	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-jpeg", SWFT_NAMESPACE, swft_import_jpeg );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-jpega", SWFT_NAMESPACE, swft_import_jpega );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-png", SWFT_NAMESPACE, swft_import_png );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-ttf", SWFT_NAMESPACE, swft_import_ttf );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-mp3", SWFT_NAMESPACE, swft_import_mp3 );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-wav", SWFT_NAMESPACE, swft_import_wav );
	xsltRegisterExtFunction( ctx, (const xmlChar *) "import-binary", SWFT_NAMESPACE, swft_import_binary );
	
	swft_ctx *c = new swft_ctx;
	return c;
}

void swft_shutdown( xsltTransformContextPtr ctx, const xmlChar *URI, void *data ) {
	swft_ctx *c = (swft_ctx*)data;
	delete c;
}


