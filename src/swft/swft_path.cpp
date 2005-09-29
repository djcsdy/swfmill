#include "SWFShapeMaker.h"
#include "SWF.h"
#include "swft.h"
#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxml/xpathInternals.h>

using namespace SWF;

#define TMP_STRLEN 0xff

// returns true when a segment is finished.
bool mkShapeSegment( ShapeMaker& shaper, double *coord, int *C, char *tmp, char mode, double *smoothx, double *smoothy ) {
	bool fin=true;
	int c = *C;
	
	if( tmp[0] ) {
		*C=c+1;
		coord[c] = atof( tmp );
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
		shaper.setup( coord[0], coord[1] );
	} else if( mode == 'm' && c==1 ) {
		shaper.setupR( coord[0], coord[1] );
	} else if( mode == 'H' && c==0 ) {
		shaper.lineTo( coord[0], shaper.getLastY() );
	} else if( mode == 'V' && c==0 ) {
		shaper.lineTo( shaper.getLastX(), coord[0] );
	} else {
		return false;
	}
	return true;
}
	
/*
	Create a complete DefineShape3 element.
	syntax: swft:path( <svg path string>, <id>, <style attr> [, <xofs>, <yofs>] )
*/
void swft_path( xmlXPathParserContextPtr ctx, int nargs ) {
	xmlChar *string, *styleString, *idString;
	xmlXPathObjectPtr obj;
	Shape shape;
	Context swfctx;
	xmlDocPtr doc;
	xmlNodePtr node, shapeNode, styleNode;
	int fillBits = 0, lineBits = 0;
	double coord[6];
	int c=0;
	char tmp[32]; tmp[0]=0;
	int t=0;
	int mode = 0;
	bool closed = true;
	double smoothx, smoothy;
	double xofs, yofs;

	
	if( (nargs != 3) && (nargs != 5) ) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	if( nargs == 5 ) {
		yofs = xmlXPathPopNumber(ctx);
		xofs = xmlXPathPopNumber(ctx);
		if( xmlXPathCheckError(ctx) )
			return;
	} else {
		yofs = xofs = 0;
	}
/* maybe, unsure. should check FIXME
	xofs*=20;
	yofs*=20;
*/
	
	styleString = xmlXPathPopString(ctx);
	idString = xmlXPathPopString(ctx);
	string = xmlXPathPopString(ctx);
	if( xmlXPathCheckError(ctx) || (string==NULL) || (idString==NULL) || (styleString == NULL) ) {
		return;
	}
	
	CSSStyle style;
	parse_css_simple( (const char *)styleString, &style );

	ShapeMaker shaper( shape.getedges(), 20, 20, xofs, yofs );
	shaper.setStyle( style.no_fill ? -1 : 1, -1, style.no_stroke ? -1 : 1 );
	
//	fprintf(stderr,"making shape from path '%s', style '%s', id '%s'\n", string, styleString, idString );
	
	for( int i=0; i==0 || string[i-1] != 0; i++ ) {
		switch( string[i] ) {
			case 'M':
			case 'm':
				shaper.close();
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
	
	// make the shape xml
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	node = shapeNode = doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"DefineShape3", NULL );
	xmlSetProp( node, (const xmlChar*)"objectID", idString );

		// bounds rectangle
	float border = 0;
	if( !style.no_stroke ) border = style.width;
	
	node = xmlNewChild( shapeNode, NULL, (const xmlChar *)"bounds", NULL ); 
	node = xmlNewChild( node, NULL, (const xmlChar *)"Rectangle", NULL );
	snprintf(tmp,TMP_STRLEN,"%f", shaper.getMinX()-border );
	xmlSetProp( node, (const xmlChar *)"left", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%f", shaper.getMinY()-border );
	xmlSetProp( node, (const xmlChar *)"top", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%f", shaper.getMaxX()+border );
	xmlSetProp( node, (const xmlChar *)"right", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%f", shaper.getMaxY()+border);
	xmlSetProp( node, (const xmlChar *)"bottom", (const xmlChar *)&tmp );

		// style
	
	node = xmlNewChild( shapeNode, NULL, (const xmlChar *)"styles", NULL ); 
	styleNode = xmlNewChild( node, NULL, (const xmlChar *)"StyleList", NULL );

	if( !style.no_fill ) {
		node = xmlNewChild( styleNode, NULL, (const xmlChar *)"fillStyles", NULL );
		
		node = xmlNewChild( node, NULL, (const xmlChar *)"Solid", NULL );
		node = xmlNewChild( node, NULL, (const xmlChar *)"color", NULL );
		node = xmlNewChild( node, NULL, (const xmlChar *)"Color", NULL );
		snprintf(tmp,TMP_STRLEN,"%i", style.fill.r);
		xmlSetProp( node, (const xmlChar *)"red", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%i", style.fill.g);
		xmlSetProp( node, (const xmlChar *)"green", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%i", style.fill.b);
		xmlSetProp( node, (const xmlChar *)"blue", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%i", style.fill.a);
		xmlSetProp( node, (const xmlChar *)"alpha", (const xmlChar *)&tmp );

		shaper.close();
	}

	if( !style.no_stroke ) {
		node = xmlNewChild( styleNode, NULL, (const xmlChar *)"lineStyles", NULL );
		node = xmlNewChild( node, NULL, (const xmlChar *)"LineStyle", NULL );
		snprintf(tmp,TMP_STRLEN,"%f", style.width);
		xmlSetProp( node, (const xmlChar *)"width", (const xmlChar *)&tmp );
		node = xmlNewChild( node, NULL, (const xmlChar *)"color", NULL );
		node = xmlNewChild( node, NULL, (const xmlChar *)"Color", NULL );
		snprintf(tmp,TMP_STRLEN,"%i", style.stroke.r);
		xmlSetProp( node, (const xmlChar *)"red", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%i", style.stroke.g);
		xmlSetProp( node, (const xmlChar *)"green", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%i", style.stroke.b);
		xmlSetProp( node, (const xmlChar *)"blue", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%i", style.stroke.a);
		xmlSetProp( node, (const xmlChar *)"alpha", (const xmlChar *)&tmp );
	}
	
		// the shape itself
	shaper.finish();
	node = xmlNewChild( shapeNode, NULL, (const xmlChar *)"shapes", NULL ); 

	shape.writeXML( node, &swfctx );
	
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
	return;
}

void swft_path_old( xmlXPathParserContextPtr ctx, int nargs ) {
	xmlChar *string;
	xmlXPathObjectPtr obj;
	Shape shape;
	Context swfctx;
	xmlDocPtr doc;
	int fillBits = 0, lineBits = 0;
	double coord[6];
	int c=0;
	char tmp[32]; tmp[0]=0;
	int t=0;
	int mode = 0;
	bool closed = true;
	double smoothx, smoothy;
	double xofs, yofs;

	if( (nargs != 1) && (nargs != 3) ) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	if( nargs == 3 ) {
		yofs = xmlXPathPopNumber(ctx);
		xofs = xmlXPathPopNumber(ctx);
		if( xmlXPathCheckError(ctx) )
			return;
	} else {
		yofs = xofs = 0;
	}
	
	string = xmlXPathPopString(ctx);
	if( xmlXPathCheckError(ctx) || (string == NULL) ) {
		return;
	}
	

	ShapeMaker shaper( shape.getedges(), 20, 20, xofs, yofs );

//	fprintf(stderr,"making shape from path '%s'\n", string );
	
	for( int i=0; i==0 || string[i-1] != 0; i++ ) {
		switch( string[i] ) {
			case 'M':
			case 'm':
				shaper.close();
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

	shaper.finish();
	
	// make the shape xml
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"tmp", NULL );
	
	shape.writeXML( doc->xmlRootNode, &swfctx );
		
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
	return;
}


bool measureShapeSegment( double *coord, int *C, char *tmp, char mode, double*smoothx, double *smoothy, 
			double *lastx, double *lasty, double *minx, double *miny, double *maxx, double *maxy  ) {
	bool fin=true;
	int c = *C;
	
	if( tmp[0] ) {
		*C=c+1;
		coord[c] = atof( tmp );
		//fprintf(stderr, "coord[%i]: %i, mode %c, tmp %s\n", c, coord[c], mode, tmp );
	}
	if( mode == 'Q' && c==3 ) {
		*lastx = coord[2]; *lasty = coord[3];
		*smoothx = coord[2] + (coord[2]-coord[0]);
		*smoothy = coord[3] + (coord[3]-coord[1]);
	} else if( mode == 'q' && c==3 ) {
		*lastx += coord[1]; *lasty += coord[2];
		*lastx += coord[2]; *lasty += coord[3];
	} else if( mode == 'T' && c==1 ) {
		*lastx = coord[0]; *lasty = coord[1];
		*smoothx = coord[0] + (coord[0]-*smoothx);
		*smoothy = coord[1] + (coord[1]-*smoothy);
	} else if( mode == 'C' && c==5 ) {
		*lastx = coord[4]; *lasty = coord[5];
		*smoothx = coord[4] + (coord[4]-coord[2]);
		*smoothy = coord[5] + (coord[5]-coord[3]);
	} else if( mode == 'S' && c==3 ) {
		*lastx = coord[2]; *lasty = coord[3];
		*smoothx = coord[2] + (coord[2]-coord[0]);
		*smoothy = coord[3] + (coord[3]-coord[1]);
	} else if( mode == 'L' && c==1 ) {
		*lastx = coord[0]; *lasty = coord[1];
	} else if( mode == 'l' && c==1 ) {
		*lastx += coord[0]; *lasty += coord[1];
	} else if( mode == 'M' && c==1 ) {
		*lastx = coord[0]; *lasty = coord[1];
	} else if( mode == 'm' && c==1 ) {
		*lastx += coord[0]; *lasty += coord[1];
	} else if( mode == 'H' && c==0 ) {
		*lastx = coord[0];
	} else if( mode == 'V' && c==0 ) {
		*lasty = coord[1];
	} else {
		return false;
	}
	if( *lastx < *minx ) *minx = *lastx;
	if( *lasty < *miny ) *miny = *lasty;
	if( *lastx > *maxx ) *maxx = *lastx;
	if( *lasty > *maxy ) *maxy = *lasty;
	return true;
}
void swft_bounds( xmlXPathParserContextPtr ctx, int nargs ) {
	xmlChar *string;
	xmlXPathObjectPtr obj;
	xmlDocPtr doc;
	xmlNodePtr node;
	double coord[6];
	int c=0;
	char tmp[TMP_STRLEN]; tmp[0]=0;
	int t=0;
	int mode = 0;
	double smoothx, smoothy;
	double lastx=0, lasty=0;
	double minx=1000000, miny=1000000; // FIXME proper limits?
	double maxx=0, maxy=0;
	double xofs, yofs;

	if( (nargs != 1) && (nargs != 3) ) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	if( nargs == 3 ) {
		yofs = xmlXPathPopNumber(ctx);
		xofs = xmlXPathPopNumber(ctx);
		if( xmlXPathCheckError(ctx) )
			return;
	} else {
		yofs = xofs = 0;
	}
	
	string = xmlXPathPopString(ctx);
	if( xmlXPathCheckError(ctx) || (string == NULL) ) {
		return;
	}
	
//	fprintf(stderr,"measuring bounding box for path '%s' with offset %f/%f\n", string, xofs, yofs );
	
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
				if( measureShapeSegment( coord, &c, tmp, mode, &smoothx, &smoothy, &lastx, &lasty, &minx, &miny, &maxx, &maxy ) ) {
					c=0;
				}
				t=0; tmp[0]=0;
				mode = string[i];
				break;
			case 'Z':
			case 'z':
				mode = 0;
				c=0; t=0; tmp[0]=0;
				break;
			case '\t':
			case '\n':
			case '\r':
			case ',':
			case ' ':
			case 0:
				if( measureShapeSegment( coord, &c, tmp, mode, &smoothx, &smoothy, &lastx, &lasty, &minx, &miny, &maxx, &maxy ) ) {
					c=0;
				}
				t=0; tmp[0]=0;
				break;
			default:
				tmp[t++]=string[i]; tmp[t]=0;
				break;
		}
	}
		
	// make the shape xml
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	node = doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"Rectangle", NULL );
	/*
		maxx-=minx;
		maxy-=miny;
		minx=miny=0;
	*/
	/*
		maxx+=10;
		maxy+=10;
		minx-=10;
		miny-=10;
	*/
		maxx+=xofs;
		minx+=xofs;
		maxy+=yofs;
		miny+=yofs;
	
//	printf("BOUNDS: %f %f, %f %f, ofs %f %f\n", minx, miny, maxx, maxy, xofs, yofs );
	
		snprintf(tmp,TMP_STRLEN,"%f", minx*20);
		xmlSetProp( node, (const xmlChar *)"left", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", maxx*20);
		xmlSetProp( node, (const xmlChar *)"right", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", miny*20);
		xmlSetProp( node, (const xmlChar *)"top", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", maxy*20);
		xmlSetProp( node, (const xmlChar *)"bottom", (const xmlChar *)&tmp );
	
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
	return;
}


void swft_transform( xmlXPathParserContextPtr ctx, int nargs ) {
	xmlChar *string;
	xmlXPathObjectPtr obj;
	xmlDocPtr doc;
	xmlNodePtr node;
	char tmp[TMP_STRLEN];
	double xofs, yofs;

	if( (nargs != 1) && (nargs != 3) ) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	if( nargs == 3 ) {
		yofs = xmlXPathPopNumber(ctx);
		xofs = xmlXPathPopNumber(ctx);
		if( xmlXPathCheckError(ctx) )
			return;
	} else {
		yofs = xofs = 0;
	}
	xofs *= 20;
	yofs *= 20;
	
	string = xmlXPathPopString(ctx);
	if( xmlXPathCheckError(ctx) || (string == NULL) ) {
		return;
	}
	
	float a, b, c, d, e, f;
	if( sscanf( (const char*)string, "matrix(%f,%f,%f,%f,%f,%f)", &a, &b, &c, &d, &e, &f ) == 6 ) {
//		fprintf(stderr,"matrix: %f %f %f %f %f %f\n", a, b, c, d, e, f );
	
		doc = xmlNewDoc( (const xmlChar *)"1.0");
		doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"Transform", NULL );
		
		node = doc->xmlRootNode;
		xmlSetProp( node, (const xmlChar *)"generated", (const xmlChar *)"true" );
		
		float factorx, factory;
		factorx = factory = 20;
	
		float scaleX, scaleY, skewX, skewY, transX, transY;
		scaleX = a;
		scaleY = d;
		transX = e*factorx;
		transY = f*factory;
		skewX = b;
		skewY = c;
	
		snprintf(tmp,TMP_STRLEN,"%f", skewX);
		xmlSetProp( node, (const xmlChar *)"skewX", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", skewY);
		xmlSetProp( node, (const xmlChar *)"skewY", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", scaleX);
		xmlSetProp( node, (const xmlChar *)"scaleX", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", scaleY);
		xmlSetProp( node, (const xmlChar *)"scaleY", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", transX+xofs);
		xmlSetProp( node, (const xmlChar *)"transX", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", transY+yofs);
		xmlSetProp( node, (const xmlChar *)"transY", (const xmlChar *)&tmp );
		
		valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
		
	} else if( sscanf( (const char*)string, "translate(%f,%f)", &e, &f ) == 2 ) {
//		fprintf(stderr,"translate: %f %f, offset %f %f\n", e, f, xofs, yofs );
	
		doc = xmlNewDoc( (const xmlChar *)"1.0");
		doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"Transform", NULL );
		
		node = doc->xmlRootNode;
		xmlSetProp( node, (const xmlChar *)"generated", (const xmlChar *)"true" );
		
		float factorx, factory;
		factorx = factory = 20;
	
		float transX, transY;
		transX = e*factorx;
		transY = f*factory;
	
		snprintf(tmp,TMP_STRLEN,"%f", transX+xofs);
		xmlSetProp( node, (const xmlChar *)"transX", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", transY+yofs);
		xmlSetProp( node, (const xmlChar *)"transY", (const xmlChar *)&tmp );

		valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
		
	} else if( sscanf( (const char*)string, "scale(%f,%f)", &a, &b ) == 2 ) {
//		fprintf(stderr,"scale: %f %f\n", a, b );
	
		doc = xmlNewDoc( (const xmlChar *)"1.0");
		doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"Transform", NULL );
		
		node = doc->xmlRootNode;
		xmlSetProp( node, (const xmlChar *)"generated", (const xmlChar *)"true" );
		
		float scaleX, scaleY;
		scaleX = a;
		scaleY = b;
	
		snprintf(tmp,TMP_STRLEN,"%f", scaleX);
		xmlSetProp( node, (const xmlChar *)"scaleX", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", scaleY);
		xmlSetProp( node, (const xmlChar *)"scaleY", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", xofs);
		xmlSetProp( node, (const xmlChar *)"transX", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", yofs);
		xmlSetProp( node, (const xmlChar *)"transY", (const xmlChar *)&tmp );

		valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );

	} else {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:transform() : transformation is not a simple matrix, translate or scale, NYI\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
}