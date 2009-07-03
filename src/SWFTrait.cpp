#include <SWFAction.h>
#include "SWF.h"
#include <cstring>

namespace SWF {

Trait *Trait::get( Reader *r, int end, Context *ctx ) {
	int type = r->getNBitInt( 4 );
	int len = end - r->getPosition();
	
	Trait* ret = getByType( type );

	if( !ret ) {
		ret = handleError( type );
	}

	if( ret ) {
		ret->setType( type );
		ret->setLength( len );
		ret->parse( r, end, ctx );
	} 

	return ret;
}	

void Trait::writeHeader( Writer *w, Context *ctx, size_t len ) {
	w->putNBitInt( type, 4 );
}

Trait* Trait::handleError( int type ) {
	fprintf(stderr,"WARNING: unknown trait type 0x%02X\n", type );
	return NULL;
}

}
