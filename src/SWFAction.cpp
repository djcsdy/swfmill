#include <SWFAction.h>
#include "SWF.h"
#include <string.h>

namespace SWF {

Action::Action() {
	type = len = 0;
}
	
Action *Action::get( Reader *r, int end, Context *ctx ) {
	uint16_t h = r->getByte();
	int type = h;
	int len = 0;
	if( type >= 0x80 ) {
		len = r->getWord();
	}
	
	if( type == 0 ) return( new EndAction ); // terminator
	
	Action *ret = NULL;
	
	for( int i=0; !ret && i<nRegistered; i++ ) {
		if( Registry[i].type == type ) {
			ret = Registry[i].factory();
		}
	}
	
//	printf("ACTION %02X len %i: %p\n", type, len, ret );

	if( !ret ) {
		ret = new UnknownAction;
	}

	ret->setTypeAndLength( type, len );
	ret->parse( r, r->getPosition()+len, ctx );
	
	return ret;
}	

Action *Action::getByName( const char *name ) {
	Action *ret = NULL;
	
	for( int i=0; i<nRegistered; i++ ) {
		if( !strcmp( Registry[i].name, name ) ) {
			ret = Registry[i].factory();
			ret->setType( Registry[i].type );
			return ret;
		}
	}
	return NULL;
}

void Action::writeHeader( Writer *w, Context *ctx, size_t len ) {
//	printf("write action %02X, len %i, recorded %i(+header, 1 or 3), position %i\n", type, len, this->len, w->getPosition() );

	w->putByte( type );
	if( type >= 0x80 ) {
		w->putWord( len-=3 );
	}
}

/*
bool EndAction::parse( Reader *r, int end, Context *ctx ) {
	return true;
}

void EndAction::dump( int indent ) {
	for( int i=0; i<indent; i++ ) printf("  ");
	printf("[00] End\n", type, len );
}

size_t EndAction::getSize( Context *ctx ) {
	r += Tag::HeaderSize(r);
	return r;
}

void EndAction::write( Writer *w, Context *ctx ) {
	w->putByte(0);
}

bool UnknownAction::parse( Reader *r, int end, Context *ctx ) {
	if( !len ) return true;
	data = new unsigned char[ len ];
	return r->getData( (void*)data, len );
}

void UnknownAction::dump( int indent ) {
	for( int i=0; i<indent; i++ ) printf("  ");
	printf("[0x%02X] unknown action, length %i\n", type, len );
	
	indent++;
	int i=0;
	while( i<len ) {
		for( int in=0; in<indent; in++ ) printf("  ");
		for( int n=0; n<16 && i<len; n++ ) {
			printf("%02X ", data[i] );
			if( n%4==3 ) printf(" ");
			i++;
		}
		printf("\n");
	}
}

size_t UnknownAction::getSize( Context *ctx ) {
	int r = len*8;
	r += Action::getHeaderSize(r);
	return r;
}

void UnknownAction::write( Writer *w, Context *ctx ) {
	Action::writeHeader( w, ctx, getSize(ctx)/8 );
	w->putData( data, len );
}

*/

}
