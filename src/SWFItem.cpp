#include "SWFItem.h"
#include <string.h>
#include <ctype.h>
#include "base64.h"
#include <sys/types.h>

namespace SWF {

// ------------ utility function

int SWFBitsNeeded( int32_t value, bool is_signed ) {
	long cnt;
	if( is_signed ) {
		if(value < 0) {
			// a special case
			if(value == -1) {
				return 1;
			}
			cnt = 33;
			do {
				cnt--;
				value *= 2;
			} while(value < 0);
			if( value == 0 ) return cnt+1;
		}
		else {
			// a special case
			if(value == 0) {
				return 1;
			}
			cnt = 33;
			do {
				cnt--;
				value *= 2;
			} while(value > 0);
			if( value == 0 ) return cnt+1;
		}
	} else {
	if( value < 0 ) {
		printf("ERROR: unsigned, but <0? %s:%i\n", __FILE__, __LINE__);
		abort();
	}
		// a special case
		if(value == 0) {
			return 1;
		}
		cnt = 32;
		while((long) value > 0) {
			cnt--;
			value *= 2;
		}
		if( value == 0 ) return cnt+1;
	}
	return cnt;
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
	if( data ) delete data;
}

bool Rest::parse( Reader *r, int end, Context *ctx ) {
	file_offset = r->getPosition();

	size = end - r->getPosition();
	if( size ) {
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

size_t Rest::getSize( Context *ctx, int start_at ) {
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
			delete tmpstr;
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
		delete dst;
		xmlFree( xmld );
	} 
}

void Rest::getdata( unsigned char **d, int *s ) {
	*d = data;
	*s = size;
}

void Rest::setdata( unsigned char *d, int s ) {
	if( data ) delete data;
	data = NULL;
	size = s;
	
	if( size ) {
		data = new unsigned char[ size ];
		memcpy( data, d, size );
	}
}

}
