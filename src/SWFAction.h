#ifndef SWF_ACTION_H
#define SWF_ACTION_H

#include <SWFIdItem.h>

namespace SWF {
	
class Action : public IdItem<Action> {
	protected:
		int getHeaderSize( int size ) { return( (type>=0x80 || size>0) ? 24 : 8 ); }
		void writeHeader( Writer *w, Context *ctx, size_t len );
	
	// static stuff
	public:
		static Action* get( Reader *r, int end, Context *ctx );
};

}
#endif
