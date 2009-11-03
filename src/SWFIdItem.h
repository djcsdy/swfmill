#ifndef SWF_IDITEM_H
#define SWF_IDITEM_H

#include <SWFItem.h>
#include <cstring>

namespace SWF {

template <class T>
class IdItem : public Item {
	protected:
		IdItem() { type = 0; len = 0; }

		int getHeaderSize( int size ) { return 8; }
		void writeHeader( Writer *w, Context *ctx, size_t len ) { w->putByte( type ); }

		void setType( int t ) { type = t; }
		int getType() { return type; }

		void setLength( int l ) { len = l; }
		int getLength() { return len; }

		int type;
		int len;

	// static stuff
	public:
		static T* get( Reader *r, int end, Context *ctx ) {
			int type = r->getByte();
			int len = end - r->getPosition();
			
			T* ret = getByType( type );

			if( ret ) {
				ret->setType( type );
				ret->setLength( len );
				ret->parse( r, end, ctx );
			} else {
				fprintf(stderr,"WARNING: unknown %s type 0x%02X\n", itemName, type );
			}

			return ret;
		}

		static T* getByName( const char *name ) {
			T *ret = NULL;
			
			for( int i=0; i<nRegistered; i++ ) {
				if( !strcmp( Registry[i].name, name ) ) {
					ret = Registry[i].factory();
					ret->setType( Registry[i].type );
					return ret;
				}
			}
			return NULL;
		}
		
	protected:
		static T* getByType( int type ) {
			T *ret = NULL;
			
			for( int i=0; i<nRegistered; i++ ) {
				if( Registry[i].type == type ) {
					ret = Registry[i].factory();
				}
			}

			return ret;
		}

		struct Description {
			typedef T * FactoryFunction();
			int type;
			const char *name;
			FactoryFunction *factory;
		};
		static Description Registry[];
		static int nRegistered;

		static const char* itemName;
};

}
#endif

