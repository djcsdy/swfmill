#ifndef SWF_ITEM_H
#define SWF_ITEM_H

#include <SWFReader.h>
#include <SWFWriter.h>
#include <libxml/tree.h>

namespace SWF {

class Context;
	
int SWFBitsNeeded( int32_t value, bool is_signed = false );
int SWFBitsNeeded( float value, int exp, bool is_signed = false );
long SWFMaxBitsNeeded( bool is_signed, int how_many, ... );

class Item {
	public:
		Item();
		virtual ~Item() {}
	
		virtual bool parse( Reader *r, int end, Context *ctx ) = 0;
		virtual void dump( int indent, Context *ctx ) = 0;
		virtual size_t calcSize( Context *ctx, int start_at ) = 0;
		virtual void write( Writer *r, Context *ctx ) = 0;
	
		virtual void writeXML( xmlNodePtr xml, Context *ctx ) = 0;
		virtual void parseXML( xmlNodePtr xml, Context *ctx ) = 0;
			
		// wont touch the context if size is cached.
		size_t getSize( Context *ctx, int start_at );
	
	protected:
		int getHeaderSize( int size );
		void writeHeader( Writer *w, Context *ctx, size_t len );
	
		int file_offset; // bytes after header for SWF, line for XML (NYI)
		int cached_size; // cached size
};
	
// to save the rest of a tag as binary data...
class Rest : public Item {
	public:
		Rest();
		virtual ~Rest();
		virtual bool parse( Reader *r, int end, Context *ctx );
		virtual void dump( int indent, Context *ctx );
		virtual size_t calcSize( Context *ctx, int start_at );
		virtual void write( Writer *w, Context *ctx );
		virtual void writeXML( xmlNodePtr node, Context *ctx );
		virtual void parseXML( xmlNodePtr node, Context *ctx );
	
		void getdata( unsigned char **data, int *size );
		void setdata( unsigned char *data, int size );
	
	protected:
		int size;
		unsigned char *data;
};
	
}
#endif
