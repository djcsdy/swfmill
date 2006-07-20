#ifndef SWF_FILTER_H
#define SWF_FILTER_H

#include <SWFItem.h>

namespace SWF {
	
class Filter : public Item {
	public:
		Filter();
		void setTypeAndLength( int t, int l ) { 
			type = t; 
			len = l;
		}
		void setType( int t ) { type = t; }
		int getType() { return type; }
		int getLength() { return len; }
			
	protected:
		int type;
		int len;
		int getHeaderSize( int size ) { return( 8 ); }
		void writeHeader( Writer *w, Context *ctx, size_t len );
	
	// static stuff
	public:
		static Filter* get( Reader *r, int end, Context *ctx );
		static Filter* getByName( const char *name );

	protected:
		struct Description {
			typedef Filter * FactoryFunction();
			int type;
			const char *name;
			FactoryFunction *factory;
		};
		static Description Registry[];
		static int nRegistered;
};

}
#endif
