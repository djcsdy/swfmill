#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cmath>

#include "SWF.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include "SWFShapeMaker.h"

#include <freetype/tttables.h>

using namespace SWF;

#define TMP_STRLEN 0xff

int moveTo( const FT_Vector *to, void *shaper ) {
	((ShapeMaker*)shaper)->setup( to->x, to->y );

	return 0;
}

int lineTo( const FT_Vector *to, void *shaper ) {
	((ShapeMaker*)shaper)->lineTo( to->x, to->y );

	return 0;
}

int conicTo( const FT_Vector *control, const FT_Vector *to, void *shaper ) {
	((ShapeMaker*)shaper)->curveTo( control->x, control->y, to->x, to->y );

	return 0;
}

int cubicTo( const FT_Vector *control1, const FT_Vector *control2, const FT_Vector *to, void *shaper ) {

	((ShapeMaker*)shaper)->cubicTo (
		control1->x, control1->y,
		control2->x, control2->y,
		to->x, to->y
	);

	return 0;
}

FT_Outline_Funcs decomposeFunctions = {
	(FT_Outline_MoveToFunc)moveTo,
	(FT_Outline_LineToFunc)lineTo,
	(FT_Outline_ConicToFunc)conicTo,
	(FT_Outline_CubicToFunc)cubicTo,
	0,
	0
};

bool emptyGlyph( FT_Face face, FT_ULong wc ) {
	return( wc != 32 && face->glyph->outline.n_points == 0 );
}

int compareGlyphs( const void *a, const void *b ) {
	int _a = *(int*)a;
	int _b = *(int*)b;
	return( _a - _b );
}

void getVerticalMetrics (FT_Face face, int &ascender, int &descender, int &lineGap)
{
	unsigned char *pOS2 = (unsigned char *) FT_Get_Sfnt_Table(face, ft_sfnt_os2);
	if (pOS2)
	{
		ascender = (short) (pOS2 [76] | (pOS2 [77] << 8));
		descender = (short) (pOS2 [78] | (pOS2 [79] << 8));
		lineGap = (short) (pOS2 [74] | (pOS2 [75] << 8));
	}
	else
	{
		unsigned char *pHhea = (unsigned char *) FT_Get_Sfnt_Table(face, ft_sfnt_hhea);
		if (pHhea)
		{
			ascender = (short) (pHhea [4] | (pHhea [5] << 8));
			descender = -(short) (pHhea [6] | (pHhea [7] << 8));
			lineGap = (short) (pHhea [8] | (pHhea [9] << 8));
		}
	}
}

void importDefineFont2( DefineFont2 *tag, const char *filename, const char *fontname, const xmlChar *glyphs_xml, Context *ctx, swft_ctx *swftctx, int offset ) {
	FT_Library swfft_library;
	FT_Face face;
	int error;
	FT_UInt glyph_index;
	FT_ULong character;
	FT_Outline *outline;
	int *glyphs = NULL;
	int i=0;
	char *font_ascentmap;
	
	GlyphList *glyphList = tag->getglyphs();
	List<Short>* advance = tag->getadvance();
	List<Rectangle>* bounds = tag->getbounds();
	List<WideKerning>* kernings = tag->getwideKerning();
	// NYI: kerning
	
	GlyphShape *shape;
	int nGlyphs, glyph;
	
	const int SCALING_FACTOR = 1024;

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
		fprintf(stderr,
				"WARNING: %s contains %li faces, but only the first is "
				"imported.\n", filename, face->num_faces);
	}

	FT_Set_Pixel_Sizes(face, SCALING_FACTOR, SCALING_FACTOR);

	// count availably glyphs, yes we have to load them to check if they're empty, sorry.
	nGlyphs = 0;
	if( !glyphs_xml ) {
		if( (character = FT_Get_First_Char( face, &glyph_index )) != 0 ) nGlyphs++;
		while( (character = FT_Get_Next_Char( face, character, &glyph_index )) != 0 ) {
			if( FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP ) ) {
				fprintf( stderr,
						"WARNING: cannot load glyph %lu ('%c') from "
						"%s.\n", character, character, filename );
				goto fail;
			}
			if( face->glyph->format != FT_GLYPH_FORMAT_OUTLINE ) {
				fprintf( stderr, "WARNING: %s seems to be a bitmap font.\n", filename );
				goto fail;
			}
			
			nGlyphs++;
		}
		
		glyphs = new int[nGlyphs+1];
		i=0;
		if( (character = FT_Get_First_Char( face, &glyph_index )) != 0 ) glyphs[i++] = character;
		while( (character = FT_Get_Next_Char( face, character, &glyph_index )) != 0 ) {
			//if( !emptyGlyph( face, character ) )
			glyphs[i++] = character;
		}

	} else {
		int nGlyphs_ = xmlUTF8Strlen( glyphs_xml );
		glyphs = new int[nGlyphs_];
		int len=0, idx=0;
		const unsigned char *str;
		
		for( int i=0; i<nGlyphs_; i++ ) {
			str = (const unsigned char *)&glyphs_xml[idx];
			len=4;
			glyphs[i]=xmlGetUTF8Char( str, &len )-offset;
			idx+=len;
		}

		// sort the list of glyphs
		qsort( glyphs, nGlyphs_, sizeof(int), compareGlyphs );
		
		nGlyphs = nGlyphs_;
	}
		
	glyphList->allocate( nGlyphs );
	tag->setglyphCount( nGlyphs );
	
	tag->sethasLayout( 1 );
	
	{
		int ascender, descender, lineGap;
		getVerticalMetrics (face, ascender, descender, lineGap);
		tag->setascent (ascender * SCALING_FACTOR / face->units_per_EM);
		tag->setdescent (descender * SCALING_FACTOR / face->units_per_EM);
		tag->setleading (lineGap * SCALING_FACTOR / face->units_per_EM);
	}

	tag->setwideGlyphOffsets( 1 );
	tag->setwideMap( 1 );
	
	if( face->style_flags & FT_STYLE_FLAG_ITALIC ) tag->setitalic(true);
	if( face->style_flags & FT_STYLE_FLAG_BOLD ) tag->setbold(true);
	if( !fontname ) fontname = face->family_name;
	tag->setname( strdup(fontname) );

	if( !ctx->quiet ) {
		fprintf( stderr, "Importing TTF: '%s' - '%s'%s%s (%i glyphs) charcode offset %i\n", filename, fontname,
					face->style_flags & FT_STYLE_FLAG_BOLD ? " bold" : "",
					face->style_flags & FT_STYLE_FLAG_ITALIC ? " italic" : "",
					nGlyphs, offset );
	}

	for( glyph=0; glyph<nGlyphs; glyph++ ) {

		character = glyphs[glyph];
		glyph_index = FT_Get_Char_Index( face, character );
		
		if( FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP ) ) {
			fprintf( stderr, "WARNING: cannot load glyph %lu ('%c') "
					"from %s.\n", character, character, filename );
			goto fail;
		}

		if( face->glyph->format != FT_GLYPH_FORMAT_OUTLINE ) {
			fprintf( stderr, "WARNING: %s seems to be a bitmap font.\n", filename );
			goto fail;
		}

		outline = &face->glyph->outline;
//			fprintf(stderr,"%i importing glyph %i ('%c') of %s (advance %i, %i points)\n", glyph, character, character, filename, face->glyph->advance.x, outline->n_points );

		Short *adv = new Short();
		adv->setvalue( (short)(ceil(1+(face->glyph->advance.x >> 6))) );
		advance->append(adv);
		
		glyphList->setMapN(glyph, character+offset);
		shape = glyphList->getShapeN(glyph);
		ShapeMaker shaper( shape->getedges(), (1.0/64), -(1.0/64), 0, 0 );
		shaper.setStyle( 1, -1, -1 );
		
		FT_Outline_Decompose( outline, &decomposeFunctions, &shaper );

		shaper.finish();

		Rectangle *r = new Rectangle();

		Rect rect = shaper.getBounds();
		r->setleft( (int)rect.left );
		r->settop( (int)rect.top );
		r->setright( (int)rect.right );
		r->setbottom( (int)rect.bottom );

		r->setbits( SWFMaxBitsNeeded( true, 3, r->gettop(), r->getright(), r->getbottom() ) );
		bounds->append(r);
	}
	
	if( FT_HAS_KERNING(face) ) {
		FT_Vector vec;
		int l, r;
		for( int left=0; left<nGlyphs; left++ ) {
			for( int right=0; right<nGlyphs; right++ ) {
				l = FT_Get_Char_Index( face, glyphs[left] );
				r = FT_Get_Char_Index( face, glyphs[right] );
				if( !FT_Get_Kerning( face, l, r, FT_KERNING_DEFAULT, &vec ) ) {
					if( vec.x ) {
		//				fprintf(stderr,"------ %i -> %i: %i\n", glyphs[left], glyphs[right], vec.x );
						
						WideKerning *kern = new WideKerning();
						kern->seta( glyphs[left] );
						kern->setb( glyphs[right] );
						kern->setadjustment( (short)(floor(vec.x >> 6)) );
						kernings->append(kern);
					}
				}
			}
		}
	}
	
	if( glyphs ) delete glyphs;
		
// hacky: store the ascent in the idmap.
	font_ascentmap = new char[0xff];
	snprintf( font_ascentmap, 0xff, "%s_ascent", fontname );
	swftctx->setMap( font_ascentmap, 1+(SCALING_FACTOR * face->ascender) / face->units_per_EM );
	delete font_ascentmap;
//	fprintf( stderr, "StoreAscent: %s %i\n", fontname, 1+(SCALING_FACTOR * face->ascender) / face->units_per_EM );
	
	return;
		
fail:
	fprintf( stderr, "WARNING: could not import %s\n", filename );
	return;
}

void importDefineFont3( DefineFont3 *tag, const char *filename, const char *fontname, const xmlChar *glyphs_xml, Context *ctx, swft_ctx *swftctx, int offset ) {
	FT_Library swfft_library;
	FT_Face face;
	int error;
	FT_UInt glyph_index;
	FT_ULong character;
	FT_Outline *outline;
	int *glyphs = NULL;
	int i=0;
	char *font_ascentmap;
	
	GlyphList *glyphList = tag->getglyphs();
	List<Short>* advance = tag->getadvance();
	List<Rectangle>* bounds = tag->getbounds();
	List<WideKerning>* kernings = tag->getwideKerning();
	// NYI: kerning
	
	GlyphShape *shape;
	int nGlyphs, glyph;

	const int SCALING_FACTOR = 1024 * 20;

	
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
		fprintf( stderr, "WARNING: %s contains %li faces, but only the "
				"first is imported.\n", filename, face->num_faces );
	}

	FT_Set_Char_Size(face, SCALING_FACTOR << 6, SCALING_FACTOR << 6, 72, 72);

	// count availably glyphs, yes we have to load them to check if they're empty, sorry.
	nGlyphs = 0;
	if( !glyphs_xml ) {
		if( (character = FT_Get_First_Char( face, &glyph_index )) != 0 ) nGlyphs++;
		while( (character = FT_Get_Next_Char( face, character, &glyph_index )) != 0 ) {
			if( FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP ) ) {
				fprintf(stderr,
						"WARNING: cannot load glyph %lu ('%c') "
						"from %s.\n", character, character, filename);
				goto fail;
			}
			if( face->glyph->format != FT_GLYPH_FORMAT_OUTLINE ) {
				fprintf( stderr, "WARNING: %s seems to be a bitmap font.\n", filename );
				goto fail;
			}
			//if( !emptyGlyph( face, character ) ) 
				nGlyphs++;
		}
		
		glyphs = new int[nGlyphs+1];
		i=0;
		if( (character = FT_Get_First_Char( face, &glyph_index )) != 0 ) glyphs[i++] = character;
		while( (character = FT_Get_Next_Char( face, character, &glyph_index )) != 0 ) {
			//if( !emptyGlyph( face, character ) )
			glyphs[i++] = character;
		}

	} else {
		int nGlyphs_ = xmlUTF8Strlen( glyphs_xml );
		glyphs = new int[nGlyphs_];
		int len=0, idx=0;
		const unsigned char *str;
		
		for( int i=0; i<nGlyphs_; i++ ) {
			str = (const unsigned char *)&glyphs_xml[idx];
			len=4;
			glyphs[i]=xmlGetUTF8Char( str, &len )-offset;
			idx+=len;
		}

		// sort the list of glyphs
		qsort( glyphs, nGlyphs_, sizeof(int), compareGlyphs );
		
		nGlyphs = nGlyphs_;
	}
		
	glyphList->allocate( nGlyphs );
	tag->setglyphCount( nGlyphs );
	
	tag->sethasLayout( 1 );
	
	{
		int ascender, descender, lineGap;
		getVerticalMetrics (face, ascender, descender, lineGap);
		tag->setascent (ascender * SCALING_FACTOR / face->units_per_EM);
		tag->setdescent (descender * SCALING_FACTOR / face->units_per_EM);
		tag->setleading (lineGap * SCALING_FACTOR / face->units_per_EM);
	}
	
	tag->setwideGlyphOffsets( 1 );
	tag->setwideMap( 1 );
	
	if( face->style_flags & FT_STYLE_FLAG_ITALIC ) tag->setitalic(true);
	if( face->style_flags & FT_STYLE_FLAG_BOLD ) tag->setbold(true);
	if( !fontname ) fontname = face->family_name;
	tag->setname( strdup(fontname) );

	if( !ctx->quiet ) {
		fprintf( stderr, "Importing TTF: '%s' - '%s'%s%s (%i glyphs) charcode offset %i\n", filename, fontname,
					face->style_flags & FT_STYLE_FLAG_BOLD ? " bold" : "",
					face->style_flags & FT_STYLE_FLAG_ITALIC ? " italic" : "",
					nGlyphs, offset );
	}

	for( glyph=0; glyph<nGlyphs; glyph++ ) {

		character = glyphs[glyph];
		glyph_index = FT_Get_Char_Index( face, character );
		
		if( FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP ) ) {
			fprintf(stderr,
					"WARNING: cannot load glyph %lu ('%c') from %s.\n",
					character, character, filename);
			goto fail;
		}

		if( face->glyph->format != FT_GLYPH_FORMAT_OUTLINE ) {
			fprintf( stderr, "WARNING: %s seems to be a bitmap font.\n", filename );
			goto fail;
		}

		outline = &face->glyph->outline;
//			fprintf(stderr,"%i importing glyph %i ('%c') of %s (advance %i, %i points)\n", glyph, character, character, filename, face->glyph->advance.x, outline->n_points );

		Short *adv = new Short();
		adv->setvalue( (short)(ceil(1+(face->glyph->advance.x >> 6))) );
		advance->append(adv);
		
		glyphList->setMapN(glyph, character+offset);
		shape = glyphList->getShapeN(glyph);
		ShapeMaker shaper( shape->getedges(), (1.0/64), -(1.0/64), 0, 0 );
		shaper.setStyle( 1, -1, -1 );
		
		FT_Outline_Decompose( outline, &decomposeFunctions, &shaper );

		shaper.finish();

		Rectangle *r = new Rectangle();

		Rect rect = shaper.getBounds();
		r->setleft( (int)rect.left );
		r->settop( (int)rect.top );
		r->setright( (int)rect.right );
		r->setbottom( (int)rect.bottom );

		r->setbits( SWFMaxBitsNeeded( true, 3, r->gettop(), r->getright(), r->getbottom() ) );
		bounds->append(r);
	}
	
	if( FT_HAS_KERNING(face) ) {
		FT_Vector vec;
		int l, r;
		for( int left=0; left<nGlyphs; left++ ) {
			for( int right=0; right<nGlyphs; right++ ) {
				l = FT_Get_Char_Index( face, glyphs[left] );
				r = FT_Get_Char_Index( face, glyphs[right] );
				if( !FT_Get_Kerning( face, l, r, FT_KERNING_DEFAULT, &vec ) ) {
					if( vec.x ) {
		//				fprintf(stderr,"------ %i -> %i: %i\n", glyphs[left], glyphs[right], vec.x );
						
						WideKerning *kern = new WideKerning();
						kern->seta( glyphs[left] );
						kern->setb( glyphs[right] );
						kern->setadjustment( (short)(floor(vec.x >> 6)) );
						kernings->append(kern);
					}
				}
			}
		}
	}
	
	if( glyphs ) delete glyphs;
		
// hacky: store the ascent in the idmap.
	font_ascentmap = new char[0xff];
	snprintf( font_ascentmap, 0xff, "%s_ascent", fontname );
	swftctx->setMap( font_ascentmap, 1+(SCALING_FACTOR * face->ascender) / face->units_per_EM );
	delete font_ascentmap;
//	fprintf( stderr, "StoreAscent: %s %i\n", fontname, 1+(SCALING_FACTOR * face->ascender) / face->units_per_EM );
	
	return;
		
fail:
	fprintf( stderr, "WARNING: could not import %s\n", filename );
	return;
}


void swft_import_ttf( xmlXPathParserContextPtr ctx, int nargs ) {
	xsltTransformContextPtr tctx;
	char *filename;
	xsltDocumentPtr xsltdoc;
	xmlDocPtr doc = NULL;
	xmlNodePtr node;
	xmlXPathObjectPtr obj;
	Context swfctx;
	char tmp[TMP_STRLEN];
	xmlChar *glyphs = NULL;
	int offset;
	double movieVersion;
	bool quiet = true;
	xmlXPathObjectPtr quietObj = NULL;
	swft_ctx *c = NULL;
	
	const char *fontname = NULL;
	
	if( (nargs < 2) || (nargs > 5) ) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	if( nargs >= 5 ) {
		offset = (int)xmlXPathPopNumber(ctx);
	}
	if( nargs >= 4 ) {
		fontname = (const char *)xmlXPathPopString(ctx);
		if( fontname[0] == 0 ) fontname = NULL;
	}
	if( nargs >= 3 ) {
		glyphs = xmlXPathPopString(ctx);
		if( glyphs[0] == 0 ) glyphs = NULL;
	}

	movieVersion = xmlXPathPopNumber(ctx);
	filename = swft_get_filename(xmlXPathPopString(ctx), ctx->context->doc->URL);
	if (xmlXPathCheckError(ctx)) {
		goto fail;
	}
	
	tctx = xsltXPathGetTransformContext(ctx);
	
	quietObj = xsltVariableLookup( tctx, (const xmlChar*)"quiet", NULL );
	if( quietObj && quietObj->stringval ) { quiet = !strcmp("true",(const char*)quietObj->stringval ); };
	swfctx.quiet = quiet;
		
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar*)"ttf", NULL );
	node = doc->xmlRootNode;
	
	c = (swft_ctx*)xsltGetExtData( xsltXPathGetTransformContext(ctx), SWFT_NAMESPACE );

	// create the font tag	
	if( movieVersion >= 8 ) {
		DefineFont3 *tag = new DefineFont3();
		importDefineFont3( tag, (const char *)filename, fontname, glyphs, &swfctx, c, offset );
		tag->writeXML( node, &swfctx );
	} else {
		DefineFont2 *tag = new DefineFont2();
		importDefineFont2( tag, (const char *)filename, fontname, glyphs, &swfctx, c, offset );
		tag->writeXML( node, &swfctx );
	}
	
	if( glyphs )
		xmlFree( glyphs );

	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
	goto end;
	
fail:
	fprintf( stderr, "WARNING: could not import %s\n", filename );

end:
	delete filename;
}
