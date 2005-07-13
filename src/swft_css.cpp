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
	bool no_fill, no_stroke;
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

	memset( style, 0, sizeof(Style) );
	
	bool fail=false;
	bool no_fill=false, no_stroke=false;
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

		//std::cerr << "[" << key << "] " << value << " " << (fail?"FAIL":"OK") << std::endl;
		
		if( true || !fail ) {
			if( key == "fill" && value == "none" ) {
				style->no_fill = true;
			} else if( key == "stroke" && value == "none" ) {
				style->no_stroke = true;
			} else if( key == "fill-opacity" ) {
				float f;
				sscanf(value.c_str(),"%f",&f);
				style->fill.a = (unsigned char)(f*0xff);
			} else if( key == "stroke-opacity" ) {
				float f;
				sscanf(value.c_str(),"%f",&f);
				style->stroke.a = (unsigned char)(f*0xff);
			} else if( key == "fill" ) {
				if( style->fill.a == 0 ) style->fill.a = 0xff;
				parse_color( value, &(style->fill) );
			} else if( key == "stroke" ) {
				if( style->stroke.a == 0 ) style->stroke.a = 0xff;
				parse_color( value, &(style->stroke) );
			} else if( key == "stroke-width" ) {
				float f;
				sscanf(value.c_str(),"%f",&f);
				style->width = f*20;
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

	Style style;
	parse_css_simple( (const char *)string, &style );

	/* FIXME: really we should not list a fully transparent style, 
	   but make sure the style is not used (shapes use fillStyle=, lineStyle=,
	   that makes the flash player crash firefox! */
	if( style.no_fill ) style.fill.a = 0;
	if( style.no_stroke ) style.stroke.a = 0;
	
	
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
	} else if( sscanf( (const char*)string, "%fpx", &val ) == 1 ) {
		snprintf(tmp,TMP_STRLEN,"%f", val );
		valuePush( ctx, xmlXPathNewString( (const xmlChar *)tmp ) );
	} else {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:unit() : unknown unit: '%s'\n", (const char*)string );
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
}
