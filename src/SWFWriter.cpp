#include "SWFWriter.h"
#include <cstring>
#include <cstdlib>

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

void Writer::putU30( uint32_t value ) {
	byteAlign();
		
	int bytes = 1;
	uint32_t limit = 0x80;
	for(; value > limit - 1; limit *= 0x80) {
		bytes++;
	}

	if( !assure( bytes ) ) return;
	// FIXME x86-centric?
	
	for(; bytes > 0; bytes--) {
		data[pos] = value & 0x7F;
		
		if(bytes > 1) {
			value = value >> 7;
			data[pos] |= 0x80;
		}
		pos++;
	}
}

void Writer::putS24( int value ) {
	byteAlign();
	if( !assure( 3 ) ) return;
	// FIXME x86-centric?
	value = (unsigned int)value;
	data[pos++] = value&0xFF;
	data[pos++] = (value>>8)&0xFF;
	data[pos++] = (value>>16)&0xFF;
}

void Writer::putEncodedU32(unsigned int value) {
	for (size_t i = 0; i < 5; ++i) {
		if (value < 0x80) {
			data[pos++] = value;
			return;
		}
		data[pos++] = (value & 0x7f) | 0x80;
		value >>= 7;
	}
}

void Writer::putFloat( float v ) {
    /*
	byteAlign();
	if( !assure( 4 ) ) return;

	// FIXME x86-centric?
	int value = *((int*)&v);
	data[pos++] = value&0xFF;
	data[pos++] = (value>>8)&0xFF;
	data[pos++] = (value>>16)&0xFF;
	data[pos++] = (value>>24)&0xFF;
    */
	union {
		float f;
		uint32_t ul;
	} u;
	u.f = v;
	putInt(u.ul);
}

void Writer::putDouble( double v ) {
	union {
		double d;
		uint64_t ull;
	} u;
	u.d = v;
	putInt64(u.ull);
}

void Writer::putDouble2( double v ) {
	union {
		double d;
		char c[8];
	} u;
	u.d = v;
	for( int i=0; i < 8; i++ ) {
		data[pos++] = u.c[i];
	}
}

void Writer::putHalf( float v ) {
	union {
		float f;
		uint32_t ul;
	} u;
	u.f = v;

	int sign = u.ul >> 31;
	int exp = (u.ul >> 23) & 0xFF;
	int man = (u.ul & 0x007FFFFF);

	if( exp == 0 ) {
		man = 0;
	} else if( exp == 0xFF ) {
		exp = 0x1F;
		if( man != 0 ) {
			man = 1;
		}
	} else {
		exp -= 127;
		if( exp < -14 ) {
			if( exp >= -24 ) {
				man = (man | 0x800000) >> (-exp - 14 + 13);
			} else {
				man = 0;
			}
			exp = 0;
		} else if( exp > 15 ) {
			exp = 0x1F;
			man = 0;
		} else {
			exp += 15;
			man >>= 13;
		}
	}

	putWord((sign << 15) | (exp << 10) | man);
}

void Writer::putFixed( double value, int bytesize, int exp ) {
	/* putFixed/getFixed are deprecated: they implicitly to byteAlign */
	
	if( !assure( bytesize ) ) return;

	value *= ((double)(1<<exp));
	uint64_t v = (uint64_t)value;
	for( int i=0; i<bytesize; i++ ) {
		data[pos++] = v>>(8*i);
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

void Writer::putPStringU30( const char *value ) {
	byteAlign();
	int len = strlen(value);
	putU30(len);
	if( !assure( len ) ) return;
	memcpy( &data[pos], value, len );
	pos += len;
}

void Writer::putNBitInt( int64_t value, int n, bool is_signed ) {
	// FIXME: unefficient, maybe incorrect!
	if( !assure( ((n+bits)/8)+1 ) ) return;
		
	if( is_signed ) {
		uint64_t *v = (uint64_t*)&value;
		uint64_t sign = (1<<(n-1));
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
	v *= ((double)(1<<m)); 
	putNBitInt( (int64_t)v, n, is_signed ); 
}

void Writer::putNBitFixed2( double v, int n, int m, bool is_signed ) {
	if( n%8 != 0 ) {
		fprintf(stderr,"WARNING: fixedpoint2 needs a size that is divisible by 8");
		return;
	}
	putFixed( v, n / 8, m ); 
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
