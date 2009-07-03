#include <SWFAction.h>
#include "SWF.h"
#include <cstring>

namespace SWF {

Action *Action::get( Reader *r, int end, Context *ctx ) {
	uint16_t h = r->getByte();
	int type = h;
	int len = 0;
	if( type >= 0x80 ) {
		len = r->getWord();
	}
	
	if( type == 0 ) return( new EndAction ); // terminator
	
	Action *ret = getByType( type );
	
//	printf("ACTION %02X len %i: %p\n", type, len, ret );

	if( !ret ) {
		ret = new UnknownAction;
	}

	ret->setType( type );
	ret->setLength( len );
	ret->parse( r, r->getPosition()+len, ctx );
	
	return ret;
}	

void Action::writeHeader( Writer *w, Context *ctx, size_t len ) {
//	printf("write action %02X, len %i, recorded %i(+header, 1 or 3), position %i\n", type, len, this->len, w->getPosition() );

	w->putByte( type );
	if( type >= 0x80 ) {
		w->putWord( len-=3 );
	}
}

}
