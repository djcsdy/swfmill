#ifndef SWF_SWFT_H
#define SWF_SWFT_H

#include <libxml/xpathInternals.h>
#include <string>
#include <stack>
#include <map>
#include "SVGGradient.h"
#include "SVGStyle.h"

void swft_register();

// utility functions for import (in swft_import.cpp)
void swft_addFileName( xmlNodePtr node, const char *filename );
void swft_addData( xmlNodePtr node, char *data, int length );

char *swft_get_filename( const xmlChar *uri, const xmlChar *baseUri );

#define SWFT_NAMESPACE ((const xmlChar*)"http://subsignal.org/swfml/swft")

class swft_ctx {
public:
	std::stack<SWF::SVGStyle> styles;
	std::map<std::string, SWF::SVGGradient*> gradients;

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
        int id = atoi( oldID );
        if( id == 65535 ) return id;
            
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

#endif
