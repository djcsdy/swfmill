#include "SWFItem.h"
#include <cstring>
#include <cctype>
#include "base64.h"
#include <sys/types.h>

namespace SWF {

// ------------ utility functions

int swf_get_bits_needed_for_uint( uint64_t value ) {
	int i=0;
	while( value > 0 ) {
		value >>= 1;
		i++;
	}
	return i;
}

int swf_get_bits_needed_for_int( int64_t value ) {
	if (value < 0) {
		return swf_get_bits_needed_for_uint(~value) + 1;
	} else if (value > 0) {
		return swf_get_bits_needed_for_uint(value) + 1;
	} else {
		// The SWF Specification is a bit vague about whether we require
		// any bits to represent zero as a signed integer, but Flash
		// Player appears to fall over sometimes if we set the size to
		// zero so let's use one bit to be safe.
		return 1;
	}
}

int swf_get_bits_needed_for_fp( double value, int exp = 16 ) {
	return swf_get_bits_needed_for_int( (int64_t)(value * (1 << exp)) );
}

int SWFBitsNeeded( float value, int exp, bool is_signed ) {
	if( !is_signed ) {
		fprintf (stderr, "FIXME: calculate bits for unsigned float\n");
	}

	return swf_get_bits_needed_for_fp( value, exp );
}
	
int SWFBitsNeeded( int32_t value, bool is_signed ) {
	if( is_signed ) {
		return swf_get_bits_needed_for_int( value );
	} else {
		return swf_get_bits_needed_for_uint( value );
	}
}

long SWFMaxBitsNeeded( bool is_signed, int how_many, ... ) {
	long bits = 0;
	va_list ap;
	int n;
	va_start( ap, how_many );
	
	for( int i=0; i<how_many; i++ ) {
		int b = SWFBitsNeeded( va_arg( ap, int ), is_signed );
		if( b > bits ) bits = b;
	}
	
	va_end( ap );
	return bits;
}

Item::Item() {
	file_offset = -1;
	cached_size = -1;
}

size_t Item::getSize( Context *ctx, int start_at ) {
	if( cached_size == -1 ) {
		cached_size = calcSize( ctx, start_at );
	}
	return cached_size;
}

int Item::getHeaderSize( int size ) {
	return 0;
}
	
void Item::writeHeader( Writer *w, Context *ctx, size_t len ) {
}



Rest::Rest() {
	size = 0;
	data = NULL;
}

Rest::~Rest() {
	delete[] data;
}

bool Rest::parse( Reader *r, int end, Context *ctx ) {
	file_offset = r->getPosition();

	size = end - r->getPosition();
	if( size > 0 ) {
		data = new unsigned char[size];
		r->getData( data, size );
	}

	return r->getError() == SWFR_OK;
}

void Rest::dump( int indent, Context *ctx ) {
	for( int i=0; i<indent; i++ ) printf("  ");
	printf("Rest (length %i)\n", size);
	if( size && data != NULL ) {
		int i=0;
		while( i<size ) {
			for( int in=0; in<indent+1; in++ ) printf("  ");
			for( int n=0; n<8 && i<size; n++ ) {
				printf(" %02X",  data[i] );
				i++;
			}
			printf("\n");
		}
	}
}

size_t Rest::calcSize( Context *ctx, int start_at ) {
	int r = start_at;
	r += size * 8;
	r += Item::getHeaderSize(r-start_at);
	return r-start_at;
}

void Rest::write( Writer *w, Context *ctx ) {
	Item::writeHeader( w, ctx, 0 );
	
	if( size && data ) {
		w->putData( data, size );
	}
}

#define TMP_STRLEN 0xFF
void Rest::writeXML( xmlNodePtr xml, Context *ctx ) {
	char tmp[TMP_STRLEN];
	xmlNodePtr node = xml;
	
//	node = xmlNewChild( node, NULL, (const xmlChar *)"Rest", NULL );
	{
		if( size && data ) {
			char *tmp_data = (char *)data;
			int sz = size;
			char *tmpstr = new char[ (sz * 3) ];
			
			int l = base64_encode( tmpstr, tmp_data, sz );
			if( l > 0 ) {
				tmpstr[l] = 0;
				xmlNewTextChild( node, NULL, (const xmlChar *)"data", (const xmlChar *)tmpstr );
			}
			delete[] tmpstr;
		}
	}
}

void Rest::parseXML( xmlNodePtr node, Context *ctx ) {
	data = NULL;
	size = 0;

	xmlChar *xmld = xmlNodeGetContent( node );
	char *d = (char *)xmld;
	if( d ) {
		// unsure if this is neccessary
		for( int i=strlen(d)-1; i>0 && isspace(d[i]); i-- ) d[i]=0;
		while( isspace(d[0]) ) d++;
		int l = strlen(d);
		char *dst = new char[ l ];
		int lout = base64_decode( dst, (char*)d, l );
		if( lout > 0 ) {
			size = lout;
			data = new unsigned char[ lout ];
			memcpy( data, dst, lout );
		}
		delete[] dst;
		xmlFree( xmld );
	} 
}

void Rest::getdata( unsigned char **d, int *s ) {
	*d = data;
	*s = size;
}

void Rest::setdata( unsigned char *d, int s ) {
	delete[] data;
	data = NULL;
	size = s;
	
	if( size ) {
		data = new unsigned char[ size ];
		memcpy( data, d, size );
	}
}

}
