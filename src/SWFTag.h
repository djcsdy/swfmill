#ifndef SWF_TAG_H
#define SWF_TAG_H

#include <SWFItem.h>

namespace SWF {
	
#define SWF_LONG_TAG 8
class Tag : public Item {
	public:
		Tag();
		void setTypeAndLength( int t, int l );
		void setType( int t ) { type = t; }
		int getType() { return type; }
		int getLength() { return len; }
		
	protected:
		int type;
		int len;
		int getHeaderSize( int size ) { return (size/8)>=SWF_LONG_TAG ? (6*8) : (2*8); }
		void writeHeader( Writer *w, Context *ctx, size_t len );
	
	// static stuff
	public:
		static Tag* get( Reader *r, int end, Context *ctx );
		static Tag* getByName( const char *name );
	
	protected:
		struct Description {
			typedef Tag * FactoryFunction();
			int type;
			const char *name;
			FactoryFunction *factory;
		};
		static Description Registry[];
		static int nRegistered;
};
	
}
#endif
