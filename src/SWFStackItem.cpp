#include <SWFStackItem.h>
#include "SWF.h"
#include <cstring>

namespace SWF {

StackItem::StackItem() {
	type = 0;
}
	
StackItem *StackItem::get( Reader *r, int end, Context *ctx ) {

	int type = r->getByte();
	
	StackItem *ret = NULL;
	
	for( int i=0; i<nRegistered; i++ ) {
		if( Registry[i].type == type ) {
			ret = Registry[i].factory();
		}
	}

	if( ret ) {
		ret->setType( type );
		ret->parse( r, end, ctx );
	} else {
		fprintf(stderr,"Warning: unknown stack item type 0x%02X\n", type );
	}

	return ret;
}

StackItem *StackItem::getByName( const char *name ) {
	StackItem *ret = NULL;
	
	for( int i=0; i<nRegistered; i++ ) {
		if( !strcmp( Registry[i].name, name ) ) {
			ret = Registry[i].factory();
			ret->setType( Registry[i].type );
			return ret;
		}
	}
	return NULL;
}

void StackItem::writeHeader( Writer *w, Context *ctx, size_t len ) {
	w->putByte( type );
}

}
