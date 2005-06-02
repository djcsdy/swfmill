#include <SWFTag.h>
#include "SWF.h"
#include <string.h>

namespace SWF {

Tag::Tag() {
	type = len = 0;
}
	
Tag *Tag::get( Reader *r, int end, Context *ctx ) {
	uint16_t h = r->getWord();
	int type = h>>6;
	int len = h&0x3F;
	if( len == 0x3F ) { // long size
		len = r->getInt();
	}
	
	// sanity check
	if( len > 100000000 ) {
		fprintf(stderr,"ridiculous tag size: %i, ignoring.\n", len );
//		r->setEOF();
		return NULL;
	}

	Tag *ret = NULL;
	
	for( int i=0; !ret && i<nRegistered; i++ ) {
		if( Registry[i].type == type ) {
			ret = Registry[i].factory();
		}
	}
	
	if( !ret ) {
		ret = new UnknownTag;
	}
	
	ret->setTypeAndLength( type, len );
	ret->parse( r, r->getPosition()+len, ctx );

	return ret;
}

Tag *Tag::getByName( const char *name ) {
	Tag *ret = NULL;
	
	for( int i=0; i<nRegistered; i++ ) {
		if( !strcmp( Registry[i].name, name ) ) {
			ret = Registry[i].factory();
			ret->setType( Registry[i].type );
			return ret;
		}
	}
	return NULL;
}
	
void Tag::setTypeAndLength( int t, int l ) {
	// must be called before parse()!
	type = t;
	len = l;
}

void Tag::writeHeader( Writer *w, Context *ctx, size_t len ) {
//	printf("write tag %02X, len %i, recorded %i(+header, 2 or 6), position %i\n", type, len, this->len, w->getPosition() );
	// shouldnt this be 0x3f - macromedia flash seems to use long length for > 1F...
	len -= (len >= SWF_LONG_TAG + 6) ? 6 : 2;
	uint16_t h = type<<6;
	if( len >= SWF_LONG_TAG ) {
		h |= 0x3F;
		w->putWord(h);
		w->putInt(len);
	} else {
		h |= len;
		w->putWord(h);
	}
}
/*
bool UnknownTag::parse( Reader *r, int end, Context *ctx ) {
	data = new unsigned char[ len ];
	return r->getData( (void*)data, len );
}

void UnknownTag::dump( int indent ) {
	for( int i=0; i<indent; i++ ) printf("  ");
	printf("[unknown tag 0x%02X, len %i]\n", type, len );
	
	indent++;
	int i=0;
	while( i<len ) {
		for( int in=0; in<indent; in++ ) printf("  ");
		for( int n=0; n<16 && i<len; n++ ) {
			for( int b=0; b<8; b++ ) {
				printf( data[i] & (1<<b) ? "1" : "0" );
			}
			printf(" ");
//			printf("%02X ", data[i] );
			if( n%4==3 ) printf(" ");
			i++;
		}
		printf("\n");
	}
}

size_t UnknownTag::getSize( Context *ctx ) {
	int r = len*8;
	r += Tag::getHeaderSize(r);
	return r;
}

void UnknownTag::write( Writer *w, Context *ctx ) {
	Tag::writeHeader( w, ctx, getSize(ctx)/8 );
	w->putData( data, len );
}
*/
}
