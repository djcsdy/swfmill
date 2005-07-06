#ifndef SWF_SHAPEITEM_H
#define SWF_SHAPEITEM_H

#include <SWFItem.h>

namespace SWF {
	
class ShapeItem : public Item {
	public:
		ShapeItem();
		void setType( unsigned char t ) { type = t; }
		unsigned char type;

		int getHeaderSize( int size );
		void writeHeader( Writer *w, Context *ctx, size_t len );
		
		bool isEnd();
		
	// static stuff
	public:
		static ShapeItem* get( Reader *r, int end, Context *ctx );
		static ShapeItem* getByName( const char *name );
};

class LineTo : public ShapeItem {
	/* plain edges are a bit hairy, so we do them manually */
	public:
		LineTo();
		virtual bool parse( Reader *r, int end, Context *ctx );
		virtual void dump( int indent, Context *ctx );
		virtual size_t calcSize( Context *ctx, int start_at );
		virtual void write( Writer *w, Context *ctx );
		virtual void writeXML( xmlNodePtr node, Context *ctx );
		virtual void parseXML( xmlNodePtr node, Context *ctx );
	
		void setx( int x );
		void sety( int y );
	
	protected:
		int bits;
		int x, y;
};

}
#endif
