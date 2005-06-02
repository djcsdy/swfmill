#ifndef SWF_STYLE_H
#define SWF_STYLE_H

#include <SWFItem.h>

namespace SWF {
	
class Style : public Item {
	public:
		Style();
	
	protected:
		int getHeaderSize( int size ) { return 8; }
		void writeHeader( Writer *w, Context *ctx, size_t len );
		void setType( int t ) { type=t; }
		unsigned char type;

	// static stuff
	public:
		static Style* get( Reader *r, int end, Context *ctx );
		static Style* getByName( const char *name );

	protected:
		struct Description {
			typedef Style * FactoryFunction();
			int type;
			const char *name;
			FactoryFunction *factory;
		};
		static Description Registry[];
		static int nRegistered;
};

}
#endif
