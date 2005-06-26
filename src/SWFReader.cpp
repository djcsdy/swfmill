#include "SWFReader.h"
#include "SWFTag.h"
#include "SWFAction.h"
#include <string.h>

namespace SWF {

Reader::Reader( const unsigned char *d, size_t l ) {
	data = d;
	length = l;
	pos = 0;
	buf = bits = 0;
	err = 0;
}

bool Reader::getData( void *dst, size_t len ) {
	if( pos+len > length ) {
		err = SWFR_EOF;
		pos = length+1;
		return false;
	} else {
		memcpy( dst, &data[pos], len );
		pos += len;
	}
}

uint8_t Reader::getByte() {
	if( pos+1 > length ) {
		err = SWFR_EOF;
		pos = length+1;
		return 0;
	}
	return data[pos++];
}

uint16_t Reader::getWord() {
	if( pos+2 > length ) {
		err = SWFR_EOF;
		pos = length+1;
		return 0;
	}
	// FIXME x86-centric?
	int r = data[pos++];
	r += data[pos++]<<8;
	return r;
}

uint32_t Reader::getInt() {
	if( pos+4 > length ) {
		err = SWFR_EOF;
		pos = length+1;
		return 0;
	}
	int r;

	// FIXME x86-centric?
	r = data[pos++];
	r += data[pos++]<<8;
	r += data[pos++]<<16;
	r += data[pos++]<<24;

	return r;
}

uint64_t Reader::getInt64() {
	if( pos+8 > length ) {
		err = SWFR_EOF;
		pos = length+1;
		return 0;
	}
	uint64_t val;
	val = (uint64_t)getInt() << 32;
	val |= (uint64_t)getInt();
	return( val );
}

float Reader::getFloat() {
	if( pos+4 > length ) {
		err = SWFR_EOF;
		pos = length+1;
		return 0;
	}
	// FIXME x86-centric?
	int r = data[pos++];
	r += data[pos++]<<8;
	r += data[pos++]<<16;
	r += data[pos++]<<24;
	return *((float*)&r);
}

double Reader::getDouble() {
/*	if( pos+8 > length ) {
		err = SWFR_EOF;
		pos = length+1;
		return 0;
	}
	// FIXME x86-centric?
	char value[8];
	value[4] = data[pos++];
	value[5] = data[pos++];
	value[6] = data[pos++];
	value[7] = data[pos++];
	value[0] = data[pos++];
	value[1] = data[pos++];
	value[2] = data[pos++];
	value[3] = data[pos++];

	return *(double*)value;
*/
    union {
        double d;
        uint64_t ull;
    } u;

    u.ull = getInt64();
    return u.d;
}

double Reader::getFixed( int bytesize, int exp ) {
	fprintf(stderr,"WARNING: Reader::getFixed is deprecated\n");
	
	double r = 0;
	if( pos+bytesize > length ) {
		err = SWFR_EOF;
		pos = length+1;
		return 0;
	}

	for( int i=0; i<bytesize; i++ ) {
		r += data[pos++]<<(8*i);
	}
	return r / ((double)(1<<exp));
}

char *Reader::getString() {
	byteAlign();
	int len = strlen((const char *)&data[pos]);
	pos += len+1;
	return strdup( (const char *)&data[pos-(len+1)] );
}

char *Reader::getPString() {
	byteAlign();
	int len = getByte();
	char *dst = new char[len+1];
	getData( dst, len );
	dst[len]=0;
	return( dst );
}

long Reader::getNBitInt( int n, bool is_signed ) {
	// FIXME: unefficient, maybe incorrect!
	int orig_n = n;
	long r = buf;
	if( n == bits ) {
		bits = buf = 0;
		goto ret;
	}
	if( n > bits ) {
		n -= bits;
		while( n > 8 ) {
			r <<= 8;
			r += getByte();
			n-=8;
		}
		buf = getByte();
		
		if( n > 0 ) {
			r <<= n;
			bits = 8-n;

			r += buf >> (8-n);
			buf &= (1<<bits)-1;
		}
		goto ret;
	}
		
	r = buf >> (bits-n);
	bits -= n;
	buf &= (1<<bits)-1;
	
ret:	
	long sign = (1<<(orig_n-1));
	if( is_signed && (r&sign)!=0 ) {
		r |= - (long) sign;
	}
	
	return r;
}

double Reader::getNBitFixed( int n, int m, bool is_signed ) {
	double d = getNBitInt( n, is_signed );
	d /= (double)(1<<m);
	return d;
}

void Reader::dump( int len, int ofs ) {
	const unsigned char *d = &data[pos+ofs];
	
	int i=0;
	printf("DumpingReader %i - %i\n", pos+ofs, pos+ofs+len );
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
