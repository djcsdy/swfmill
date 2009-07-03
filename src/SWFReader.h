#ifndef SWF_Reader_H
#define SWF_Reader_H

#include <cstdio>
#include <stdint.h>
#include "SWFBasic.h"

namespace SWF {
	
#define SWFR_OK		0	
#define SWFR_ERROR	1
#define SWFR_EOF	2

class Reader {
	public:
		Reader( const unsigned char *data, size_t length );
	
		// these implicitly do byteAlign()
		bool getData( void *dst, size_t length );
		uint8_t getByte();
		uint16_t getWord();
		uint32_t getInt();
		uint64_t getInt64();
		uint32_t getU30();
		int getS24();
		float getFloat();
		double getDouble();
		double getDouble2();
		float getHalf();
		double getFixed( int bytesize, int decimals );
		char *getString();
		char *getPString();
		char *getPStringU30();
		
		unsigned int getEncodedU32();	
	
		// this one cares about the current bit position
		long getNBitInt( int n, bool is_signed = false );
		double getNBitFixed( int n, int m, bool is_signed = false ); // n bits total, m bits after decimal point.
		double getNBitFixed2( int n, int m, bool is_signed = false );
	
		// set bit position to next byte.
		void byteAlign() { 
			if( buf != 0 ) fprintf(stderr,"buf is nonzero in byteAlign() @%i\n", pos );
			buf = bits = 0; }
				
		int getError() { return err; }

		const unsigned char *getRawData() { return data; }
		int getPosition() { return pos; }
		int getLength() { return length; }
		int getBits() { return bits; }
		void setEOF() { pos=length; }
		void seekTo( int p ) { pos=p; }
		
		void dump( int n, int ofs ); // dumps n bytes from current position to stdout
		
	protected:
		// the uncompressed swf data and our position within
		const unsigned char *data;
		int pos, length;
	
		// buf is the current byte when in "NBit mode"
		// bits is the number of bits represented in buf.
		uint8_t buf, bits;
	
		int err;

		bool read( void *dst, size_t n );
};

}
#endif
