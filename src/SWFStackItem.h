#ifndef SWF_STACKITEM_H
#define SWF_STACKITEM_H

#include <SWFItem.h>

namespace SWF {
	
class StackItem : public Item {
	protected:
		StackItem();
		int getHeaderSize( int size ) { return 8; }
		void writeHeader( Writer *w, Context *ctx, size_t len );
		void setType( int t ) { type=t; }
		unsigned char type;

	// static stuff
	public:
		static StackItem* get( Reader *r, int end, Context *ctx );
		static StackItem* getByName( const char *name );

	protected:
		struct Description {
			typedef StackItem * FactoryFunction();
			int type;
			const char *name;
			FactoryFunction *factory;
		};
		static Description Registry[];
		static int nRegistered;
};

}
#endif
