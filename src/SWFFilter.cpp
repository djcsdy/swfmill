#include <SWFFilter.h>
#include "SWF.h"
#include <string.h>

namespace SWF {

Filter::Filter() {
	type = 0;
}
	
Filter *Filter::get( Reader *r, int end, Context *ctx ) {
	uint16_t h = r->getByte();
	int type = h;
	int len = end - r->getPosition();
	
	Filter *ret = NULL;
	
	for( int i=0; !ret && i<nRegistered; i++ ) {
		if( Registry[i].type == type ) {
			ret = Registry[i].factory();
		}
	}
	
//	printf("FILTER %02X len %i: %p\n", type, end - r->getPosition(), ret );
	
	if (ret == NULL) {
		ret = new UnknownFilter();
	}

	ret->setTypeAndLength( type, len );
	ret->parse( r, end, ctx );
	
	return ret;
}	

Filter *Filter::getByName( const char *name ) {
	Filter *ret = NULL;
	
	for( int i=0; i<nRegistered; i++ ) {
		if( !strcmp( Registry[i].name, name ) ) {
			ret = Registry[i].factory();
			ret->setType( Registry[i].type );
			return ret;
		}
	}
	return NULL;
}

void Filter::writeHeader( Writer *w, Context *ctx, size_t len ) {
//	printf("write filter %02X, len %i, position %i\n", type, len, w->getPosition() );

	w->putByte( type );
}




}
