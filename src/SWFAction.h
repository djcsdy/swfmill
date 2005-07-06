#ifndef SWF_ACTION_H
#define SWF_ACTION_H

#include <SWFItem.h>

namespace SWF {
	
class Action : public Item {
	public:
		Action();
		void setTypeAndLength( int t, int l ) {
			// must be called before parse()!
			type = t;
			len = l;
		}
		void setType( int t ) { type = t; }
		int getType() { return type; }
		int getLength() { return len; }
			
	protected:
		int type;
		int len;
		int getHeaderSize( int size ) { return( (type>=0x80 || size>0) ? 24 : 8 ); }
		void writeHeader( Writer *w, Context *ctx, size_t len );
	
	// static stuff
	public:
		static Action* get( Reader *r, int end, Context *ctx );
		static Action* getByName( const char *name );

	protected:
		struct Description {
			typedef Action * FactoryFunction();
			int type;
			const char *name;
			FactoryFunction *factory;
		};
		static Description Registry[];
		static int nRegistered;
};
}
#endif
