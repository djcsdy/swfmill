/* ultra-simple css "parser". only works for simple inline styles like sodipodi/inkscape use. */

#include <string>
#include <iostream>
#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxml/xpathInternals.h>

using namespace std;
#define TMP_STRLEN 0xff

namespace SWFT {

namespace CSS {

struct Color {
	Color() {
			r=g=b=a=0;
		};
		
	unsigned char r, g, b, a;
};

struct Style {
	Color fill;
	Color stroke;
	double width;
};

void parse_color( std::string str_c, Color* color ) {
	// this is a bit stupid really.
	const char *s = str_c.c_str();
	if( s[0] == '#' ) s++;

	unsigned char bg[3];
	
	char str[3];
	str[2] = 0;
	for( int c=0; c<3; c++ ) {
		str[0] = *s++;
		str[1] = *s++;
		sscanf(str,"%x",&bg[c]);
	}
	
	color->r = bg[0];
	color->g = bg[1];
	color->b = bg[2];
}

char *skipws( char *p ) {
	while( *p && isspace(*p) ) p++;
	return p;
}

char *parse_string( char *p, string& s ) {
	s="";
	while( *p && (isalnum(*p) || *p == '-' || *p == '#' || *p == '.') ) {
		s.push_back(*p);
		p++;
	}
	return p;
}

#define EXPECT(p,c) if( *p != c ) fail = true; else p++;
void parse_css_simple( const char *style_str, Style *style ) {
	char *p = (char *)style_str; // casting away const, but, hey- wtf.

	bool fail=false;
	string key, value;
	
	while( !fail ) {
		p = skipws(p);
		p = parse_string(p,key);
		p = skipws(p);
		EXPECT(p,':');
		p = skipws(p);
		p = parse_string(p,value);
		p = skipws(p);
		EXPECT(p,';');
		if( value.length() == 0 || key.length() == 0 ) fail = true;

//		std::cerr << "[" << key << "] " << value << std::endl;
		
		if( !fail ) {
			if( key == "fill" ) {
				parse_color( value, &(style->fill) );
			} else if( key == "stroke" ) {
				parse_color( value, &(style->stroke) );
			} else if( key == "fill-opacity" ) {
				float f;
				sscanf(value.c_str(),"%f",&f);
				style->fill.a = (unsigned char)(f*0xff);
			} else if( key == "stroke-opacity" ) {
				float f;
				sscanf(value.c_str(),"%f",&f);
				style->stroke.a = (unsigned char)(f*0xff);
			} else if( key == "stroke-width" ) {
				float f;
				sscanf(value.c_str(),"%f",&f);
				style->width = f;
			}
			// the rest is ignored
		}
	}
};
}
}

using namespace SWFT::CSS;

void swft_css( xmlXPathParserContextPtr ctx, int nargs ) {
	xmlChar *string;
	xmlXPathObjectPtr obj;
	xmlDocPtr doc;
	xmlNodePtr node;
	char tmp[TMP_STRLEN];

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
	
	//fprintf(stderr,"getting style from '%s'\n", string );

	Style style;
	parse_css_simple( (const char *)string, &style );

	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"tmp", NULL );

	node = xmlNewChild( doc->xmlRootNode, NULL, (const xmlChar *)"fillStyles", NULL );
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

	node = xmlNewChild( doc->xmlRootNode, NULL, (const xmlChar *)"lineStyles", NULL );
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

	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
}

void swft_transform( xmlXPathParserContextPtr ctx, int nargs ) {
	xmlChar *string;
	xmlXPathObjectPtr obj;
	xmlDocPtr doc;
	xmlNodePtr node;
	char tmp[TMP_STRLEN];

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:transform() : invalid arg expecting a transformation string\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
		
	string = obj->stringval;
	
	fprintf(stderr,"getting transformation from '%s'\n", string );

	float a, b, c, d, e, f;
	if( sscanf( (const char*)string, "matrix(%f,%f,%f,%f,%f,%f)", &a, &b, &c, &d, &e, &f ) == 6 ) {
		fprintf(stderr,"matrix: %f %f %f %f %f %f\n", a, b, c, d, e, f );
	
		doc = xmlNewDoc( (const xmlChar *)"1.0");
		doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"Transform", NULL );
		
		node = doc->xmlRootNode;
		xmlSetProp( node, (const xmlChar *)"generated", (const xmlChar *)"true" );
		
		float scaleX, scaleY, skewX, skewY, transX, transY;
		transX = e;
		transY = f;
		scaleX = a;
		scaleY = d;
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
		snprintf(tmp,TMP_STRLEN,"%f", transX);
		xmlSetProp( node, (const xmlChar *)"transX", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", transY);
		xmlSetProp( node, (const xmlChar *)"transY", (const xmlChar *)&tmp );
		
		valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
		
	} else if( sscanf( (const char*)string, "translate(%f,%f)", &e, &f ) == 2 ) {
		fprintf(stderr,"matrix: %f %f\n", e, f );
	
		doc = xmlNewDoc( (const xmlChar *)"1.0");
		doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"Transform", NULL );
		
		node = doc->xmlRootNode;
		xmlSetProp( node, (const xmlChar *)"generated", (const xmlChar *)"true" );
		
		float transX, transY;
		transX = e;
		transY = f;
	
		snprintf(tmp,TMP_STRLEN,"%f", transX);
		xmlSetProp( node, (const xmlChar *)"transX", (const xmlChar *)&tmp );
		snprintf(tmp,TMP_STRLEN,"%f", transY);
		xmlSetProp( node, (const xmlChar *)"transY", (const xmlChar *)&tmp );

		valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
		
	} else {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:transform() : transformation is not a simple matrix(a,b,c,d,e,f), NYI\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
}
