// functions defined here are declared in SWFImportExport.h

#include "SWFImportExport.h"
#include "SWF.h"
#include "SWFGlyphList.h"
#include "SWFShapeMaker.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define FACTOR 10

namespace SWF {

	
void exportDefineFont2( DefineFont2 *tag ) {
	// ah, really? you want to rip a font from an swf? bugger off!
	return;
}

void importGlyphPoints( FT_Vector *points, int n, ShapeMaker& shaper, bool cubic ) {
	if( n==0 )  {
		shaper.lineTo( points[0].x, points[0].y );
	} else if( n==1 ) {
		shaper.curveTo(
			points[0].x, points[0].y,
			points[1].x, points[1].y );
	} else if( n>=2	) {
		if( cubic ) {
			fprintf(stderr,"ERROR: cubic beziers in fonts are not yet implemented.\n");
		} else {
			int x1, y1, x2, y2, midx, midy;
			for( int i=0; i<n-1; i++ ) { 
				x1 = points[i].x;
				y1 = points[i].y;
				x2 = points[i+1].x;
				y2 = points[i+1].y;
				midx = x1 + ((x2-x1)/2);
				midy = y1 + ((y2-y1)/2);
				shaper.curveTo( x1, y1, midx, midy );
			}
			shaper.curveTo( x2, y2, points[n].x, points[n].y );
		}
	} else {
	}
}

void importDefineFont2( DefineFont2 *tag, const char *filename, xmlNodePtr node ) {
	FT_Library swfft_library;
	FT_Face face;
	int error;
	FT_UInt glyph_index;
	FT_ULong character;
	FT_Outline *outline;
	xmlChar *glyphs_xml = xmlGetProp( node, (const xmlChar *)"glyphs" );
	const char *glyphs = (const char *)glyphs_xml;
	
	GlyphList *glyphList = tag->getglyphs();
	List<Short>* advance = tag->getadvance();
	List<Rectangle>* bounds = tag->getbounds();
	// NYI: kerning
	
	Shape *shape;
	int glyph_n;
	int n, ofs;
	
	if( FT_Init_FreeType( &swfft_library ) ) {
		fprintf( stderr, "WARNING: could not initialize FreeType\n" );
		goto fail;
	}
	
	error = FT_New_Face( swfft_library, filename, 0, &face );
	if( error ) {
		fprintf( stderr, "WARNING: FreeType does not like %s\n", filename );
		goto fail;
	}

	if( face->num_faces > 1 ) {
		fprintf( stderr, "WARNING: %s contains %i faces, but only the first is imported.\n", filename, face->num_faces );
	}
	
	if( face->charmap == 0 ) {
		fprintf( stderr, "WARNING: %s doesn't seem to contain a unicode charmap.\n", filename );
	}

	FT_Set_Char_Size(face, 1024<<6, 1024<<6, 75, 75);

	// we should only import @glyphs, lets do all for now.
	
	// count availably glyphs
	n = 0;
	if( (character = FT_Get_First_Char( face, &glyph_index )) != 0 ) n++;
	while( (character = FT_Get_Next_Char( face, character, &glyph_index )) != 0 ) n++;

	if( n>255 ) n=255; // FIXME
	ofs = 0;
	n-=ofs;
	
	fprintf( stderr, "importing %s: %i glyphs\n", filename, n );

	glyphList->allocate( n );
	tag->setglyphCount( n );
	tag->setname( "helvetica" ); // FIXME
	tag->sethasLayout( 1 );
	tag->setascent( face->ascender * 1024 / face->units_per_EM );
	tag->setdescent( labs(face->descender)* 1024 / face->units_per_EM );
	tag->setleading( face->height* 1024 / face->units_per_EM );
	tag->setwideGlyphOffsets( 1 );
	tag->setwideMap( 1 );
	
	character = FT_Get_First_Char( face, &glyph_index );
	for( int i=0; i<ofs; i++ ) character=FT_Get_Next_Char( face, character, &glyph_index );
	for( int glyph_n=0; character && glyph_n<n; glyph_n++ ) {
	
//		glyph_index = FT_Get_Char_Index( face, character );
		if( FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP ) ) {
			fprintf( stderr, "WARNING: couldnt load glyph %i (%c) from %s.\n", character, character, filename );
		}

		if( face->glyph->format != FT_GLYPH_FORMAT_OUTLINE ) {
			fprintf( stderr, "WARNING: %s seems to be a bitmap font.\n", filename );
			goto fail;
		}

		outline = &face->glyph->outline;
		
//		fprintf(stderr,"importing glyph %i ('%c') of %s (advance %i, %i points)\n", character, character, filename, face->glyph->advance.x, outline->n_points );
		{
			Short *adv = new Short();
			adv->setvalue( (short)(face->glyph->advance.x * (1.0/64)) );
			advance->append(adv);
			
			Rectangle *r = new Rectangle();
/*			r->settop( -face->bbox.yMax * 1024 / face->units_per_EM );
			r->setright( face->bbox.xMax - face->bbox.xMin );
			r->setbottom( -face->bbox.yMin * 1024 / face->units_per_EM );
*/			r->setbits( SWFMaxBitsNeeded( true, 3, r->gettop(), r->getright(), r->getbottom() ) );
			bounds->append(r);
			
			glyphList->setMapN(glyph_n, character);
			shape = glyphList->getShapeN(glyph_n);
			ShapeMaker shaper( shape->getedges(), (1.0/64), -(1.0/64), 0, 0 );

			// set fillBits
			shape->setfillBits(1);
			
			int start = 0, end;
			bool control, cubic;
			int n;
			for( int contour = 0; contour < outline->n_contours; contour++ ) {
				end = outline->contours[contour];
	//			fprintf(stderr,"  contour %i: %i-%i\n", contour, start, end );
				n=0;

				for( int p = start; p<=end; p++ ) {
					control = !(outline->tags[p] & 0x01);
					cubic = outline->tags[p] & 0x02;
					
	/*				if( character == 'h' ) 
					fprintf( stderr, "   point %i: %s%s %i %i\n", p, 
						control?(cubic?"third-order ":"second-order "):"",
						control?"control":"on-curve",
						outline->points[p].x, outline->points[p].y);
	*/				
						if( p==start ) {
							shaper.setup( outline->points[p-n].x, outline->points[p-n].y, 1 );
						}
					if( !control && n > 0 ) {
						importGlyphPoints( &(outline->points[(p-n)+1]), n-1, shaper, cubic );
						n=1;
					} else {
						n++;
					}
				}
				
				if( n ) {
					// special case: repeat first point
					FT_Vector points[n+1];
					int s=(end-n)+2;
					for( int i=0; i<n-1; i++ ) {
						points[i].x = outline->points[s+i].x;
						points[i].y = outline->points[s+i].y;
					}
					points[n-1].x = outline->points[start].x;
					points[n-1].y = outline->points[start].y;
					
					importGlyphPoints( points, n-1, shaper, false );
				}
				
				shaper.close();
				
				start = end+1;
			}
				
			shaper.finish();
		}
		
		character = FT_Get_Next_Char( face, character, &glyph_index );
	}
	
	return;
		
fail:
	fprintf( stderr, "WARNING: could not import %s\n", filename );
	return;
}

}
