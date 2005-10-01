#ifndef SWF_SWFT_H
#define SWF_SWFT_H

#include <libxml/xpathInternals.h>
#include <string>
#include <stack>
#include <map>
	
void swft_register();
	
// utility functoins for import (in swft_import.cpp)
void swft_addFileName( xmlNodePtr node, const char *filename );
void swft_addData( xmlNodePtr node, char *data, int length );

#define SWFT_NAMESPACE ((const xmlChar*)"http://subsignal.org/swfml/swft")

class swft_ctx {
public:

	int last_id;
	int last_depth;
	std::stack<std::map<std::string,int>*> maps;

	bool quiet;

	swft_ctx() {
		last_id = last_depth = 1;
		pushMap();
	}

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
	
	void setMap( const char *oldID, int newID ) {
		std::map<std::string,int>& m = *(maps.top());
		m[oldID] = newID;
	}
};

// CSS style stuff

struct CSSColor {
	CSSColor() {
			r=g=b=a=0;
		};
		
	unsigned char r, g, b, a;
};

struct CSSStyle {
	CSSStyle() {
		no_fill = no_stroke = false;
		width = 0;
	}
	
	bool no_fill, no_stroke;
	CSSColor fill;
	CSSColor stroke;
	double width;
	
	std::map<std::string,std::string> styles;
};

void parse_css_simple( const char *style_str, CSSStyle *style );

#endif
