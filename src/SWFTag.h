#ifndef SWF_TAG_H
#define SWF_TAG_H

#include <SWFIdItem.h>

namespace SWF {
	
#define SWF_LONG_TAG 8
class Tag : public IdItem<Tag> {
	protected:
		int getHeaderSize( int size ) { return (size/8)>=SWF_LONG_TAG ? (6*8) : (2*8); }
		void writeHeader( Writer *w, Context *ctx, size_t len );
	
	// static stuff
	public:
		static Tag* get( Reader *r, int end, Context *ctx );
};
	
}
#endif
