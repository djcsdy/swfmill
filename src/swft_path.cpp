#include "SWFShapeMaker.h"
#include "SWF.h"
#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxml/xpathInternals.h>

using namespace SWF;

// returns true when a segment is finished.
bool mkShapeSegment( ShapeMaker& shaper, int *coord, int *C, char *tmp, char mode, int *smoothx, int *smoothy ) {
	bool fin=true;
	int c = *C;
	
	if( tmp[0] ) {
		*C=c+1;
		coord[c] = atoi( tmp );
		//fprintf(stderr, "coord[%i]: %i, mode %c, tmp %s\n", c, coord[c], mode, tmp );
	}
	if( mode == 'Q' && c==3 ) {
		shaper.curveTo( coord[0], coord[1], coord[2], coord[3] );
		*smoothx = coord[2] + (coord[2]-coord[0]);
		*smoothy = coord[3] + (coord[3]-coord[1]);
	} else if( mode == 'q' && c==3 ) {
		shaper.curveToR( coord[0], coord[1], coord[2], coord[3] );
	} else if( mode == 'T' && c==1 ) {
		shaper.curveTo( *smoothx, *smoothy, coord[0], coord[1] );
		*smoothx = coord[0] + (coord[0]-*smoothx);
		*smoothy = coord[1] + (coord[1]-*smoothy);
	} else if( mode == 'C' && c==5 ) {
		shaper.cubicTo( coord[0], coord[1], coord[2], coord[3], coord[4], coord[5] );
		*smoothx = coord[4] + (coord[4]-coord[2]);
		*smoothy = coord[5] + (coord[5]-coord[3]);
	} else if( mode == 'S' && c==3 ) {
		shaper.cubicTo( *smoothx, *smoothy, coord[0], coord[1], coord[2], coord[3] );
		*smoothx = coord[2] + (coord[2]-coord[0]);
		*smoothy = coord[3] + (coord[3]-coord[1]);
	} else if( mode == 'L' && c==1 ) {
		shaper.lineTo( coord[0], coord[1] );
	} else if( mode == 'l' && c==1 ) {
		shaper.lineToR( coord[0], coord[1] );
	} else if( mode == 'M' && c==1 ) {
		shaper.setup( coord[0], coord[1], 1, -1, 1 );
	} else if( mode == 'm' && c==1 ) {
		shaper.setupR( coord[0], coord[1], 1, -1, 1 );
	} else if( mode == 'H' && c==0 ) {
		shaper.lineTo( coord[0], shaper.getLastY() );
	} else if( mode == 'V' && c==0 ) {
		shaper.lineTo( shaper.getLastX(), coord[0] );
	} else {
		return false;
	}
	return true;
}
	
void swft_path( xmlXPathParserContextPtr ctx, int nargs ) {
	xmlChar *string;
	xmlXPathObjectPtr obj;
	Shape shape;
	ShapeMaker shaper( shape.getedges() );
	Context swfctx;
	xmlDocPtr doc;
	int fillBits = 0, lineBits = 0;
	int coord[6];
	int c=0;
	char tmp[32]; tmp[0]=0;
	int t=0;
	int mode = 0;
	bool closed = true;
	int smoothx, smoothy;

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:path() : invalid arg expecting a path string\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
		
	string = obj->stringval;
	
	// fprintf(stderr,"making shape from path '%s'\n", string );
	
	for( int i=0; i==0 || string[i-1] != 0; i++ ) {
		switch( string[i] ) {
			case 'M':
			case 'm':
			case 'L':
			case 'l':
			case 'C':
			case 'c':
			case 'S':
			case 's':
			case 'Q':
			case 'q':
			case 'T':
			case 't':
			case 'H':
			case 'h':
			case 'V':
			case 'v':
				if( mkShapeSegment( shaper, coord, &c, tmp, mode, &smoothx, &smoothy ) ) {
					c=0; closed=false;
				}
				t=0; tmp[0]=0;
				mode = string[i];
				break;
			case 'Z':
			case 'z':
				shaper.close();
				mode = 0;
				c=0; t=0; tmp[0]=0;
				closed = true;
				break;
			case '\t':
			case '\n':
			case '\r':
			case ',':
			case ' ':
			case 0:
				if( mkShapeSegment( shaper, coord, &c, tmp, mode, &smoothx, &smoothy ) ) {
					c=0; closed=false;
				}
				t=0; tmp[0]=0;
				break;
			default:
				tmp[t++]=string[i]; tmp[t]=0;
				break;
		}
	}
		
	//	if( !closed ) shaper.close();
	shaper.finish();
	
	// make the shape xml
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"tmp", NULL );
	
	shape.writeXML( doc->xmlRootNode, &swfctx );
	
/*
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:document() : failed to read file '%s'\n", (const char *)uri);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
*/
	
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
	return;
}

