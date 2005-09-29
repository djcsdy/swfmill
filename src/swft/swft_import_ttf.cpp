#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

#include "SWF.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "SWFShapeMaker.h"

using namespace SWF;

#define TMP_STRLEN 0xff

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

bool emptyGlyph( FT_Face face, FT_ULong wc ) {
	return( wc != 32 && face->glyph->outline.n_points == 0 );
}

int compareGlyphs( const void *a, const void *b ) {
	int _a = *(int*)a;
	int _b = *(int*)b;
	return( _a - _b );
}

void importDefineFont2( DefineFont2 *tag, const char *filename, const xmlChar *glyphs_xml, Context *ctx ) {
	FT_Library swfft_library;
	FT_Face face;
	int error;
	FT_UInt glyph_index;
	FT_ULong character;
	FT_Outline *outline;
	int *glyphs = NULL;
	
	GlyphList *glyphList = tag->getglyphs();
	List<Short>* advance = tag->getadvance();
	List<Rectangle>* bounds = tag->getbounds();
	// NYI: kerning
	
	GlyphShape *shape;
	int nGlyphs, glyph;
	
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

	FT_Set_Char_Size(face, (1024<<6), (1024<<6), 72, 72);

	// count availably glyphs, yes we have to load them to check if they're empty, sorry.
	nGlyphs = 0;
	if( !glyphs_xml ) {
		if( (character = FT_Get_First_Char( face, &glyph_index )) != 0 ) nGlyphs++;
		while( (character = FT_Get_Next_Char( face, character, &glyph_index )) != 0 ) {
			if( FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP ) ) {
				fprintf( stderr, "WARNING: cannot load glyph %i ('%c') from %s.\n", character, character, filename );
				goto fail;
			}
			if( face->glyph->format != FT_GLYPH_FORMAT_OUTLINE ) {
				fprintf( stderr, "WARNING: %s seems to be a bitmap font.\n", filename );
				goto fail;
			}
			if( !emptyGlyph( face, character ) ) nGlyphs++;
		}
	} else {
		int nGlyphs_ = xmlUTF8Strlen( glyphs_xml );
		glyphs = new int[nGlyphs_];
		int len=0, idx=0;
		const unsigned char *str;
		
		for( int i=0; i<nGlyphs_; i++ ) {
			str = (const unsigned char *)&glyphs_xml[idx];
			len=4;
			glyphs[i]=xmlGetUTF8Char( str, &len );
			idx+=len;
		}

		// sort the list of glyphs
		qsort( glyphs, nGlyphs_, sizeof(int), compareGlyphs );
		
		nGlyphs = 0;
		for( int i=0; i<nGlyphs_; i++ ) {
			glyph_index = FT_Get_Char_Index( face, character );
			if( FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP ) ) {
				fprintf( stderr, "WARNING: cannot load glyph %i ('%c') from %s.\n", character, character, filename );
				goto fail;
			}
			if( face->glyph->format != FT_GLYPH_FORMAT_OUTLINE ) {
				fprintf( stderr, "WARNING: %s seems to be a bitmap font.\n", filename );
				goto fail;
			}
			if( !emptyGlyph( face, glyphs[i] ) ) 
				nGlyphs++;
		}
	}
		
	glyphList->allocate( nGlyphs );
	tag->setglyphCount( nGlyphs );
	
	tag->sethasLayout( 1 );
	
#define SCALING_FACTOR ((int)1024)

	tag->setascent( 1+(SCALING_FACTOR * face->ascender) / face->units_per_EM );
	tag->setdescent( 1+(SCALING_FACTOR * labs(face->descender)) / face->units_per_EM );
	tag->setleading( 1+(SCALING_FACTOR * face->height) / face->units_per_EM );
	
	tag->setwideGlyphOffsets( 1 );
	tag->setwideMap( 1 );
	
	if( face->style_flags & FT_STYLE_FLAG_ITALIC ) tag->setitalic(true);
	if( face->style_flags & FT_STYLE_FLAG_BOLD ) tag->setbold(true);
	tag->setname( face->family_name );

	if( !ctx->quiet ) {
		fprintf( stderr, "Importing TTF: '%s' - '%s'%s%s (%i glyphs)\n", filename, face->family_name,
					face->style_flags & FT_STYLE_FLAG_BOLD ? " bold" : "",
					face->style_flags & FT_STYLE_FLAG_ITALIC ? " italic" : "",
					nGlyphs );
	}

	character = FT_Get_First_Char( face, &glyph_index );
	for( glyph=0; character && glyph<nGlyphs; ) {
		if( glyphs ) {
			character = glyphs[glyph];
			glyph_index = FT_Get_Char_Index( face, character );
		}
		
		if( FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP ) ) {
			fprintf( stderr, "WARNING: cannot load glyph %i ('%c') from %s.\n", character, character, filename );
			goto fail;
		}

		if( face->glyph->format != FT_GLYPH_FORMAT_OUTLINE ) {
			fprintf( stderr, "WARNING: %s seems to be a bitmap font.\n", filename );
			goto fail;
		}

		if( !emptyGlyph( face, character ) ) {
			outline = &face->glyph->outline;
	//		fprintf(stderr,"%i importing glyph %i ('%c') of %s (advance %i, %i points)\n", glyph, character, character, filename, face->glyph->advance.x, outline->n_points );

			Short *adv = new Short();
			adv->setvalue( (short)(2+floor(face->glyph->advance.x >> 6)) );
			advance->append(adv);
			
			Rectangle *r = new Rectangle();
/*			r->settop( -face->bbox.yMax * 1024 / face->units_per_EM );
			r->setright( face->bbox.xMax - face->bbox.xMin );
			r->setbottom( -face->bbox.yMin * 1024 / face->units_per_EM );
*/			r->setbits( SWFMaxBitsNeeded( true, 3, r->gettop(), r->getright(), r->getbottom() ) );
			bounds->append(r);
			
			glyphList->setMapN(glyph, character);
			shape = glyphList->getShapeN(glyph);
			ShapeMaker shaper( shape->getedges(), (1.0/64), -(1.0/64), 0, 0 );
			shaper.setStyle( 1, -1, -1 );
			
/* FIXME this has no effect, done elsewhere
			shape->setfillBits(1);
			shape->setlineBits(0);
*/
			int start = 0, end;
			bool control, cubic;
			int n;
			for( int contour = 0; contour < outline->n_contours; contour++ ) {
				end = outline->contours[contour];
				n=0;

				for( int p = start; p<=end; p++ ) {
					control = !(outline->tags[p] & 0x01);
					cubic = outline->tags[p] & 0x02;
					
					if( p==start ) {
						shaper.setup( outline->points[p-n].x, outline->points[p-n].y );
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
			glyph++;
		}
		
		if( !glyphs ) character = FT_Get_Next_Char( face, character, &glyph_index );
	}
	
	if( glyphs ) delete glyphs;
	return;
		
fail:
	fprintf( stderr, "WARNING: could not import %s\n", filename );
	return;
}

void swft_import_ttf( xmlXPathParserContextPtr ctx, int nargs ) {
	xsltTransformContextPtr tctx;
	xmlChar *filename;
	xsltDocumentPtr xsltdoc;
	xmlDocPtr doc = NULL;
	xmlNodePtr node;
	xmlXPathObjectPtr obj;
	DefineFont2 *tag;
	Context swfctx;
	char tmp[TMP_STRLEN];
	xmlChar *glyphs = NULL;
	
	if( nargs == 2 ) {
		xmlXPathStringFunction(ctx, 1);
		obj = valuePop(ctx);
		if( obj->stringval ) glyphs = xmlStrdup(obj->stringval);
		if( xmlStrlen(glyphs) == 0 ) {
			// import all glyphs
			xmlFree(glyphs);
			glyphs=NULL;
		}
	}

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:import-ttf() : invalid arg expecting a string\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
		
	tctx = xsltXPathGetTransformContext(ctx);
	
	bool quiet = true;
	xmlXPathObjectPtr quietObj = xsltVariableLookup( tctx, (const xmlChar*)"quiet", NULL );
	if( quietObj && quietObj->stringval ) { quiet = !strcmp("true",(const char*)quietObj->stringval ); };
	swfctx.quiet = quiet;
	
	filename = obj->stringval;
	
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar*)"ttf", NULL );
	node = doc->xmlRootNode;
	
	//swft_addFileName( node, (const char *)filename );
	
	
	// create the font tag
	tag = new DefineFont2;
	importDefineFont2( tag, (const char *)filename, glyphs, &swfctx );
	tag->writeXML( node, &swfctx );

/*
	snprintf(tmp,TMP_STRLEN,"%i", 5);
	xmlSetProp( node, (const xmlChar *)"format", (const xmlChar *)&tmp );
*/	
	if( glyphs ) delete glyphs;

	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
	return;
	
fail:
	fprintf( stderr, "WARNING: could not import %s\n", filename );
	return;
}
