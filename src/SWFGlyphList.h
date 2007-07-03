#ifndef SWF_GLYPHLIST_H
#define SWF_GLYPHLIST_H

#include <SWFItem.h>

namespace SWF {

class GlyphShape;
	
class GlyphList : public Item {
	/* glyph lists have strange offsetting stuff, so we do them manually */
	public:
		GlyphList();
		virtual ~GlyphList();
		virtual bool parse( Reader *r, int end, Context *ctx );
		virtual void dump( int indent, Context *ctx );
		virtual size_t calcSize( Context *ctx, int start_at );
		virtual void write( Writer *w, Context *ctx );
		virtual void writeXML( xmlNodePtr node, Context *ctx );
		virtual void parseXML( xmlNodePtr node, Context *ctx );
	
		void allocate( int n );
		GlyphShape *getShapeN( int n );
		void setMapN( int n, int m );
	
	protected:
		int nGlyphs;
		GlyphShape *glyphs;
		int *map;
};

}

#endif
