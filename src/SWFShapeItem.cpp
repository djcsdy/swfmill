#include <SWFShapeItem.h>
#include "SWF.h"
#include <cstring>

namespace SWF {

ShapeItem::ShapeItem() {
	type = 0;
}
	
bool ShapeItem::isEnd() {
	if( type != 0 ) return false;
	ShapeSetup *self = (ShapeSetup*)this;
	return( !( self->gethasNewStyles()
			|| self->gethasLineStyle()
			|| self->gethasFillStyle0()
			|| self->gethasFillStyle1()
			|| self->gethasMoveTo()) );
}

int ShapeItem::getHeaderSize(int size) {
	switch( type ) {
		case 0:
			return 1;
		case 1:
		case 2:
			return 2;
	}
	return 0;
}
	
ShapeItem *ShapeItem::get( Reader *r, int end, Context *ctx ) {

	if( r->getPosition() == end && r->getBits() < 6 ) {
		return NULL;
	}
	
	ShapeItem *ret;
	int type = 0;
	int isSetup = !r->getNBitInt(1);

	if( isSetup ) {
		ret = new ShapeSetup;
		type = 0;
	} else {
		int isLine = r->getNBitInt(1);
	
		if( isLine ) {
			ret = new LineTo;
			type = 1;
		} else {
			ret = new CurveTo;
			type = 2;
		}
	}

	ret->setType( type );
	ret->parse( r, end, ctx );
	return ret;
}

ShapeItem *ShapeItem::getByName( const char *name ) {
	
	ShapeItem *ret = NULL;
	int type = 0;
	if( !strcmp( name, "ShapeSetup" ) ) {
		ret = new ShapeSetup;
		type = 0;
	} else if( !strcmp( name, "LineTo" ) ) {
		ret = new LineTo;
		type = 1;
	} else if( !strcmp( name, "CurveTo" ) ) {
		ret = new CurveTo;
		type = 2;
	}
	
	if( ret ) {
		ret->setType( type );
	}

	return ret;
}

void ShapeItem::writeHeader( Writer *w, Context *ctx, size_t len ) {
	if( type == 0 ) {
		w->putNBitInt(0,1);
	} else {
		w->putNBitInt(1,1);
		if( type == 1 ) {
			w->putNBitInt(1,1);
		} else {
			w->putNBitInt(0,1);
		}
	}
}

LineTo::LineTo() {
	bits = x = y = 0;
	type = 1;
}

bool LineTo::parse( Reader *r, int end, Context *ctx ) {
	file_offset = r->getPosition();

	bits = r->getNBitInt( 4 );
	int hasBoth = r->getNBitInt(1);

	x=y=0;
	
	if( hasBoth ) {
		x = r->getNBitInt(bits+2,true);
		y = r->getNBitInt(bits+2,true);
	} else {
		int hasY = r->getNBitInt(1);
		if( hasY ) {
			y = r->getNBitInt(bits+2,true);
		} else {
			x = r->getNBitInt(bits+2,true);
		}
	}
	if( ctx->debugTrace ) {
		fprintf(stderr,"PARSE LineTo @%i: %ibits %i/%i\n", file_offset, bits, x, y );
	}
	return r->getError() == SWFR_OK;
}

void LineTo::dump( int n, Context *ctx ) {
	for( int i=0; i<n; i++ ) printf("  ");
	printf("LineTo (%i+2bit) ", bits);
	if( x ) printf("%i,", x); else printf("-,");
	if( y ) printf("%i", y); else printf("-");
	printf("\n");
}

size_t LineTo::calcSize( Context *ctx, int start_at ) {
	int r=start_at;
	if( x != 0 && y != 0 ) {
		r += 4 + 1 + ((bits+2)*2);
	} else {
		r += 4 + 2 + (bits+2);
	}
	r += ShapeItem::getHeaderSize(r-start_at);
	return r-start_at;
}

void LineTo::write( Writer *w, Context *ctx ) {
	if( ctx->debugTrace ) {
		printf("WRITE LineTo @%i %ibits %i/%i\n", w->getBitPosition()/8, bits, x, y  );
		dump(1,ctx);
	}

	ShapeItem::writeHeader( w, ctx, 0 );
	w->putNBitInt( bits, 4 );
	
	if( x != 0 && y != 0 ) {
		w->putNBitInt(1,1);
		w->putNBitInt(x,bits+2,true);
		w->putNBitInt(y,bits+2,true);
	} else if( x != 0 ) {
		w->putNBitInt(0,1);
		w->putNBitInt(0,1);
		w->putNBitInt(x,bits+2,true);
	} else {
		w->putNBitInt(0,1);
		w->putNBitInt(1,1);
		w->putNBitInt(y,bits+2,true);
	}
}

void LineTo::writeXML( xmlNodePtr xml, Context *ctx ) {
	char tmp[32];
	xmlNodePtr node = xmlNewChild( xml, NULL, (const xmlChar *)"LineTo", NULL );
	
	snprintf( tmp, 32, "%i", x );
	xmlSetProp( node, (const xmlChar *)"x", (const xmlChar *)tmp );

	snprintf( tmp, 32, "%i", y );
	xmlSetProp( node, (const xmlChar *)"y", (const xmlChar *)tmp );
}

void LineTo::parseXML( xmlNodePtr node, Context *ctx ) {
	xmlChar *tmp;
	
	x = y = 0;
	bits = 0;
	
	tmp = xmlGetProp( node, (const xmlChar *)"x" );
	if( tmp ) {
		sscanf( (char*)tmp, "%i", &x);
		xmlFree( tmp );
		int b = SWFBitsNeeded( x, true );
		b -= 2;
		if( b<0 ) b=0;
		if( b > bits ) bits = b;
	}

	tmp = xmlGetProp( node, (const xmlChar *)"y" );
	if( tmp ) {
		sscanf( (char*)tmp, "%i", &y);
		xmlFree( tmp );
		int b = SWFBitsNeeded( y, true );
		b -= 2;
		if( b<0 ) b=0;
		if( b > bits ) bits = b;
	}
}

void LineTo::setx( int _x ) {
	int b = SWFBitsNeeded( _x, true );
	if( b > bits ) bits = b;
	x = _x;
}

void LineTo::sety( int _y ) {
	int b = SWFBitsNeeded( _y, true );
	if( b > bits ) bits = b;
	y = _y;
}

}
