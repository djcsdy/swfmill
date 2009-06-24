#include <SWFStyle.h>
#include <cstring>

namespace SWF {

Style::Style() {
	type = 0;
}

Style *Style::get( Reader *r, int end, Context *ctx ) {
	int type = r->getByte();
	Style *ret = NULL;
	
	for( int i=0; i<nRegistered; i++ ) {
		if( Registry[i].type == type ) {
			ret = Registry[i].factory();
		}
	}

	if( ret ) {
		ret->setType( type );
		ret->parse( r, end, ctx );
	} else {
		fprintf(stderr,"WARNING: unknown fill style type 0x%02X\n", type );
	}
	
	return ret;
}

Style *Style::getByName( const char *name ) {
	Style *ret = NULL;
	
	for( int i=0; i<nRegistered; i++ ) {
		if( !strcmp( Registry[i].name, name ) ) {
			ret = Registry[i].factory();
			ret->setType( Registry[i].type );
			return ret;
		}
	}
	return NULL;
}

void Style::writeHeader( Writer *w, Context *ctx, size_t len ) {
	w->putByte( type );
}
	
}
