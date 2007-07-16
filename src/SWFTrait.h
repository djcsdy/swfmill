#ifndef SWF_TRAIT_H
#define SWF_TRAIT_H

#include <SWFIdItem.h>

namespace SWF {
	
class Trait : public IdItem<Trait> {
	protected:
		int getHeaderSize( int size ) { return 4; }
		void writeHeader( Writer *w, Context *ctx, size_t len );
	
	// static stuff
	public:
		static Trait* get( Reader *r, int end, Context *ctx );

	protected:
		static Trait* handleError( int type );
};

}
#endif
