#ifndef SWF_FILE_H
#define SWF_FILE_H

#include "SWF.h"

namespace SWF {

class File {
	public:
		File();
		~File();
		int load( FILE *fp, Context *ctx = NULL, unsigned int filesize=0 );
		int save( FILE *fp, Context *ctx = NULL );
		void dump();
		int saveXML( FILE *fp, Context *ctx = NULL );
		int loadXML( const char *filename, Context *ctx = NULL );

		xmlDocPtr getXML( Context *ctx = NULL );
		int setXML( xmlNodePtr root, Context *ctx = NULL );
	
	protected:
		void compress( unsigned char *data, size_t len, FILE *fp );
		void decompress( unsigned char *data, size_t len, FILE *fp );
	
		Header *header;
	
		bool compressed;
		unsigned char version;
		uint32_t length;
};
	
	
}
#endif
