#ifndef SWF_Writer_H
#define SWF_Writer_H

#include <cstdio>
#include <cstdint>

namespace SWF {

	class Writer {
		public:
			Writer(unsigned char *data, size_t length);
			Writer(size_t length);

			static const int ok = 0;
			static const int error = 1;
			static const int full = 2;

			// these implicitly do byteAlign()
			void putData(void *src, size_t length);
			void putByte(std::uint8_t);
			void putWord(std::uint16_t);
			void putInt(std::uint32_t);
			void putInt64(std::uint64_t);
			void putU30(std::uint32_t);
			void putS24(int);
			void putFloat(float);
			void putDouble(double);
			void putHalf(float);
			void putFixed(double v, int bytesize, int decimals);
			void putString(const char *);
			void putPString(const char *);
			void putPStringU30(const char *);
			void putEncodedU32(unsigned int);

			// this one cares about the current bit position
			void putNBitInt(std::int64_t v, int n, bool is_signed = false);
			void putNBitFixed(double v, int n, int m, bool is_signed = false);
			void putNBitFixed2(double v, int n, int m, bool is_signed = false);

			// set bit position to next byte.
			void byteAlign() {
				if (bits) writeByte(buf);
				buf = 0; bits=0;
			}

			int getError() { return err; }

			void dump(int n, int ofs); // dumps n bytes from current position to stdout
			bool assure(int l, int before=-1) {
				int b = (before==-1?length:before);
				if (pos+l > b) {
					fprintf(stderr,"Write buffer full (%i+%i > %i)\n", pos, l, b);
					err = Writer::full;
					return false;
				}
				return true;
			}

			size_t getLength() { return length; }
			unsigned char *getData() { return data; }
			int getPosition() { return pos; }
			int getBits() { return bits; }
			int getBitPosition() { return (pos*8)+(bits); }

		protected:
			void writeByte(std::uint8_t value);

			// the uncompressed swf data and our position within
			unsigned char *data;
			int pos, length;

			// buf is the current byte when in "NBit mode"
			// bits is the number of bits represented in buf.
			std::uint8_t buf, bits;

			int err;

			bool write(void *dst, size_t n);
	};

}

#endif
