#include "SWFGlyphList.h"
#include <SWFShapeItem.h>
#include "SWF.h"
#include <cstring>

namespace SWF {

GlyphList::GlyphList() {
	nGlyphs = 0;
	glyphs = NULL;
	map = 0;
}

GlyphList::~GlyphList() {
	delete[] glyphs;
	delete[] map;
}

bool GlyphList::parse( Reader *r, int end, Context *ctx ) {
	file_offset = r->getPosition();
	
	int nOffsets;
	int s=0;
	
	if( ctx->tagVersion>1 ) {
		nGlyphs = ctx->glyphCount;
		nOffsets = nGlyphs+1;
	} else {
		nGlyphs = (r->getWord()/2);
		nOffsets = nGlyphs;
		s=1;
	}
	
	if( ctx->debugTrace ) fprintf(stderr, "GL nGlyphs: %i, glyphs %s, map %s\n", (int)nGlyphs, ctx->wideGlyphOffsets?"wide":"narrow", ctx->wideMap?"wide":"narrow" );
	
	int offset[ nGlyphs+1 ];
	memset( offset, 0, sizeof(int)*nGlyphs+1 );
	if( ctx->wideGlyphOffsets ) {
		for( int i=s; i<nOffsets; i++ ) {
			offset[i] = r->getInt();
		}
	} else {
		for( int i=s; i<nOffsets; i++ ) {
			offset[i] = r->getWord();
		}
	}
	
	if( ctx->tagVersion<=1 ) {
		offset[0] = r->getPosition() - file_offset;
		offset[nOffsets] = end-file_offset;
	}
	/*
	int fillBits = r->getNBitInt(4);
	int lineBits = r->getNBitInt(4);
	if( fillBits != 1 || lineBits != 0 ) {
		fprintf(stderr,"WARNING: GlyphList fillBits: %i, lineBits: %i, expected 1/0\n", fillBits, lineBits );
	}
	*/
	if( ctx->debugTrace ) fprintf(stderr, "Glyphs @%i offset[0]: %i\n", r->getPosition(), offset[0] );
	glyphs = new GlyphShape[nGlyphs];
	for( int i=0; i<nGlyphs; i++ ) {
		if( ctx->debugTrace ) fprintf(stderr, "PARSE glyph #%i @%i should be %i-%i\n", i, r->getPosition(), file_offset + offset[i], file_offset + offset[i+1] );
		if( r->getPosition() != file_offset + offset[i] ) {
			fprintf(stderr,"WARNING: wrong offset at glyph %i, ofs %i but pos %i\n", i, offset[i], r->getPosition()-file_offset );
		}
		glyphs[i].parse( r, file_offset + offset[i+1], ctx );
		r->byteAlign();
	}
/*		if( ctx->debugTrace ) fprintf(stderr, "PARSE last gylph %i\n", r->getPosition() );
		glyphs[nGlyphs-1].parse( r, end, ctx );
		r->byteAlign();
*/
	if( ctx->tagVersion>1 ) {
		if( ctx->debugTrace ) fprintf(stderr,"- parse GlyphMap" );
		map = new int[ nGlyphs ];
		if( ctx->wideMap ) {
			for( int i=0; i<nGlyphs; i++ ) {
				map[i] = r->getWord();
			}
		} else {
			for( int i=0; i<nGlyphs; i++ ) {
				map[i] = r->getByte();
			}
		}	
	}

	return r->getError() == SWFR_OK;
}

void GlyphList::dump( int n, Context *ctx ) {
	for( int i=0; i<n; i++ ) printf("  ");
	printf("GlyphList\n");
	
	for( int i=0; i<nGlyphs; i++ ) {
		glyphs[i].dump( n+1, ctx );
	}
	printf("\n");
}

size_t GlyphList::calcSize( Context *ctx, int start_at ) {
	int r=start_at;
	
	if( ctx->tagVersion>1 ) {
		r += ( ctx->wideGlyphOffsets ? 4 : 2 ) * (nGlyphs+1) * 8;
	} else {
		r += (nGlyphs+1) * 16;
	}
/*	
	// fillBits/lineBits
	ctx->fillBits = 1;
	ctx->lineBits = 0;
	r+=8
	*/
	for( int i=0; i<nGlyphs; i++ ) {
		r += glyphs[i].getSize( ctx, r );
		if( r%8 != 0 ) r += 8-(r%8);
	}

	if( ctx->tagVersion>1 ) r += ( ctx->wideMap ? 2 : 1 ) * nGlyphs * 8;
	
/*		fprintf(stderr,"SIZE DefineFont-%i, %s map\n", 
			ctx->tagVersion, ctx->wideMap ? "wide" : "narrow" );
	fprintf(stderr,"GL size: %i %i %i\n", nGlyphs, r, r/8 );
*/	
	r += Item::getHeaderSize(r-start_at);
	return r-start_at;
}

void GlyphList::write( Writer *w, Context *ctx ) {
	Item::writeHeader( w, ctx, 0 );
	int ofs = 0;

	if( ctx->tagVersion>1 ) {
		ofs = (( ctx->wideGlyphOffsets ? 4 : 2 ) * (nGlyphs+1));
	} else {
		ofs = (nGlyphs+1)*2;
	}
	/*
	// fillBits/lineBits
	ofs++;
	*/
	ctx->fillBits = 1;
	ctx->lineBits = 1;

	ctx->wideGlyphOffsets ? w->putInt( ofs ) : w->putWord( ofs );
	for( int i=0; i<nGlyphs; i++ ) {
		int p = glyphs[i].getSize(ctx,ofs);
		if( p%8 != 0 ) p += 8-(p%8);
		ofs += (p)/8;
		ctx->wideGlyphOffsets ? w->putInt( ofs ) : w->putWord( ofs );
	}
	/*
	// fillBits/lineBits
	w->putNBitInt( 1, 4 );
	w->putNBitInt( 0, 4 );
	*/
	ofs = w->getPosition();
	for( int i=0; i<nGlyphs; i++ ) {
		glyphs[i].write( w, ctx );
		w->byteAlign();
	}

	if( ctx->tagVersion>1 ) {
		for( int i=0; i<nGlyphs; i++ ) {
			ctx->wideMap ? w->putWord( map[i] ) : w->putByte( map[i] );
		}
	}
}

void GlyphList::writeXML( xmlNodePtr xml, Context *ctx ) {
	char tmp[32];
/*
	xmlNodePtr node = xmlNewChild( xml, NULL, (const xmlChar *)"GlyphList", NULL );
	
	for( int i=0; i<nGlyphs; i++ ) {
		glyphs[i].writeXML( node, ctx );
	}
*/
	xmlNodePtr node = xml; //xmlNewChild( xml, NULL, (const xmlChar *)"glyphs", NULL );
	for( int i=0; i<nGlyphs; i++ ) {
		xmlNodePtr child = xmlNewChild( node, NULL, (const xmlChar *)"Glyph", NULL );
		glyphs[i].writeXML( child, ctx );
		if( ctx->tagVersion>1 && map ) {
			snprintf( tmp, 32, "%i", map[i] );
			xmlSetProp( child, (const xmlChar *)"map", (const xmlChar *)tmp );
		}
	}
}

void GlyphList::parseXML( xmlNodePtr node, Context *ctx ) {
	{
		nGlyphs = 0;
		xmlNodePtr child = node;
		while( child ) {
			if( !strcmp( (const char *)child->name, "Glyph" ) ) {
				nGlyphs++;
			}
			child = child->next;
		}
		ctx->glyphCount = nGlyphs;

		if( ctx->tagVersion>1 ) {
			map = new int[ nGlyphs ];
			memset( map, 0, sizeof(int)*nGlyphs );
		}
		
		glyphs = new GlyphShape[ nGlyphs ];
		child = node;
		int i=0;
		while( child ) {
			if( !strcmp( (const char *)child->name, "Glyph" ) ) {
				xmlNodePtr shape = child->children;
				while( shape ) {
					if( !strcmp( (const char *)shape->name, "GlyphShape" ) ) {
						glyphs[i].parseXML( shape, ctx );
						
						if( ctx->tagVersion>1 ) {
							xmlChar *tmp;
							tmp = xmlGetProp( child, (const xmlChar *)"map" );
							if( tmp ) {
								sscanf( (char*)tmp, "%i", &map[i]);
								xmlFree( tmp );
								if( map[i] > 0xFF ) ctx->wideMap = true;
							} else {
								map[i] = 0;
							}
						}
						
						i++;
						shape = NULL;
					} else {
						shape = shape->next;
					}
				}
			}
			child = child->next;
		}
		
		if( ctx->swfVersion >= 6 ) ctx->wideMap = true;
		
	}
}

void GlyphList::allocate( int n ) {
	delete[] map;
	delete[] glyphs;

	nGlyphs = n;
	glyphs = new GlyphShape[ nGlyphs ];
	map = new int[ nGlyphs ];
	memset( map, 0, sizeof(int)*nGlyphs );
}

GlyphShape *GlyphList::getShapeN( int n ) {
	return &glyphs[n];
}

void GlyphList::setMapN( int n, int m ) {
	map[n] = m;
}

}
