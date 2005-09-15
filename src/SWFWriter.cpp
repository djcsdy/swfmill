#include "SWFWriter.h"
#include <string.h>
#include <stdlib.h>

namespace SWF {

Writer::Writer( unsigned char *d, size_t l ) {
	data = d;
	length = l;
	pos = 0;
	buf = bits = 0;
	err = 0;
}

Writer::Writer( size_t l ) {
	data = new unsigned char[l];
	length = l;
	pos = 0;
	buf = bits = 0;
	err = 0;
}

void Writer::putData( void *src, size_t len ) {
	byteAlign();
	if( !assure( len ) ) return;
	memcpy( &data[pos], src, len );
	pos += len;
}

void Writer::putByte( uint8_t value ) {
	byteAlign();
	writeByte(value);
}

void Writer::putWord( uint16_t value ) {
	byteAlign();
	if( !assure( 2 ) ) return;
	// FIXME x86-centric?
	data[pos++] = value&0xFF;
	data[pos++] = value>>8;
}

void Writer::putInt( uint32_t value ) {
	byteAlign();
	if( !assure( 4 ) ) return;
	// FIXME x86-centric?
	data[pos++] = value&0xFF;
	data[pos++] = (value>>8)&0xFF;
	data[pos++] = (value>>16)&0xFF;
	data[pos++] = (value>>24)&0xFF;
}

void Writer::putInt64( uint64_t value ) {
	byteAlign();
	if( !assure( 8 ) ) return;

	putInt( value >> 32 );
	putInt( value & 0xffffffff );
}

void Writer::putFloat( float v ) {
	byteAlign();
	if( !assure( 4 ) ) return;

	// FIXME x86-centric?
	int value = *((int*)&v);
	data[pos++] = value&0xFF;
	data[pos++] = (value>>8)&0xFF;
	data[pos++] = (value>>16)&0xFF;
	data[pos++] = (value>>24)&0xFF;
}

void Writer::putDouble( double v ) {
    union {
        double d;
        uint64_t ull;
    } u;
    u.d = v;
    putInt64(u.ull);
/*
	byteAlign();
	if( !assure( 8 ) ) return;
	// FIXME x86-centric?
	char value[8];
	(*(double*)value) = v;
	
	data[pos++] = value[4];
	data[pos++] = value[5];
	data[pos++] = value[6];
	data[pos++] = value[7];
	data[pos++] = value[0];
	data[pos++] = value[1];
	data[pos++] = value[2];
	data[pos++] = value[3];
*/
}

void Writer::putFixed( double value, int bytesize, int exp ) {
	if( !assure( bytesize ) ) return;

	value *= ((double)(1<<exp));
	uint64_t v = (uint64_t)value;
	for( int i=0; i<bytesize; i++ ) {
		data[pos++] = v>>(8*i);
		v/=(1<<(8*i));
	}
}

void Writer::putString( const char *value ) {
	byteAlign();
	int len = strlen(value);
	if( !assure( len+1 ) ) return;
	memcpy( &data[pos], value, len+1 );
	pos += len+1;
}

void Writer::putPString( const char *value ) {
	byteAlign();
	int len = strlen(value);
	if( !assure( len+1 ) ) return;
	putByte(len);
	memcpy( &data[pos], value, len );
	pos += len;
}

void Writer::putNBitInt( int value, int n, bool is_signed ) {
	// FIXME: unefficient, maybe incorrect!
	if( !assure( ((n+bits)/8)+1 ) ) return;
		
	if( is_signed ) {
		unsigned int *v = (unsigned int*)&value;
		unsigned int sign = (1<<(n-1));
		if( *v & sign ) {
			value = (*v&(sign-1)) | (1<<(n-1));
		}
	}

	// sanity check
	if( !is_signed || value >= 0 ) {
		if( value >= (uint64_t)1<<(n+(is_signed?2:1) ) ) {
			fprintf(stderr,"WARNING: (%ssigned) value %i is too big to be represented in %i bits (max %i)\n", is_signed?"":"un", value, n, 1<<(n+(is_signed?1:0) ) );
		}
	} else {
		if( value < ((uint64_t)-1)<<(n+(is_signed?2:1) ) ) {
			fprintf(stderr,"WARNING: (%ssigned) value %i is too small to be represented in %i bits (max %i)\n", is_signed?"":"un", value, n, 1<<(n+(is_signed?1:0) ) );
		}
	}
	
	if( n==(8-bits) ) {
		writeByte( buf | value );
		buf = bits = 0;
		return;
	}
	
	if( n > (8-bits) ) {
		int bs = n-(8-bits);
		writeByte( buf | value>>bs );
		n-=(8-bits);
		
		while( n>=8 ) {
			bs-=8;
			writeByte( value >> bs );
			n-=8;
		}
		
		if( n>0 ) {
			bits = n;
			buf = value << (8-bits);
		} else {
			bits = buf = 0;
		}
		
		return;
	}
	
	// n < (8-bits)
	bits += n;
	buf |= value << (8-bits);
	return;
}

void Writer::putNBitFixed( double v, int n, int m, bool is_signed ) {
	v *= (1<<m);
	putNBitInt( (int)v, n, is_signed );
}

void Writer::writeByte( uint8_t value ) {
	data[pos++] = value;
}

void Writer::dump( int len, int ofs ) {
	const unsigned char *d = &data[pos+ofs];
	
	int i=0;
	printf("DumpingWriter %i - %i\n", pos+ofs, pos+ofs+len );
	while( i<len ) {
		for( int n=0; n<4 && i<len; n++ ) {
			for( int b=7; b>=0; b-- ) {
				printf( d[i] & (1<<b) ? "1" : "0" );
			}
			printf(" 0x%02X %03i  ", d[i], d[i] );
			printf(" ");
			i++;
		}
		printf("\n");
	}
}
	
}
