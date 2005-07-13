#ifndef SWF_SWFT_H
#define SWF_SWFT_H

#include <libxml/xpathInternals.h>
#include <string>
#include <stack>
#include <map>

#ifdef _cplusplus
extern "C" {
#endif
	
void swft_register();
	
// utility functoins for import (in swft_import.cpp)
void swft_addFileName( xmlNodePtr node, const char *filename );
void swft_addData( xmlNodePtr node, char *data, int length );

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

#ifdef _cplusplus
}
#endif

#endif
