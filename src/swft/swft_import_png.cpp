#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxml/xpathInternals.h>
#include <libxslt/variables.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include "readpng.h"
#include <zlib.h>

#define TMP_STRLEN 0xff

// outLength should contain the allocated size,
// will be updated with the actual size
bool compress( unsigned char *inputBuffer, int inLength, unsigned char *outputBuffer, int *outLength ) {
	z_stream stream;
	int status, count;
	
	stream.avail_in = inLength;
	stream.next_in = inputBuffer;
	stream.next_out = outputBuffer;
	stream.zalloc = (alloc_func) NULL;
	stream.zfree = (free_func) NULL;
	stream.opaque = (voidpf) 0;
	stream.avail_out = *outLength;
	
	status = deflateInit( &stream, Z_BEST_COMPRESSION );
	if( status != Z_OK ) {
		fprintf( stderr, "ERROR: compressing PNG (1): %s\n", stream.msg );
		return false;
	}
	
	while( true ) {
		if( stream.avail_in == 0 ) break;

		status = deflate( &stream, Z_NO_FLUSH );
		
		if( status != Z_OK ) {
			fprintf( stderr, "ERROR: compressing PNG (2): %s\n", stream.msg );
			return false;
		}
	}

	do {
		status = deflate( &stream, Z_FINISH );
	} while( status == Z_OK );
	
	if( status != Z_STREAM_END ) {
		fprintf( stderr, "ERROR: compressing PNG (3): %i, %s\n", status, stream.msg );
		return false;
	}

	status = deflateEnd(&stream);

	if( status != Z_OK ) {
		fprintf( stderr, "ERROR: compressing PNG (4): %s\n", stream.msg );
		return false;
	}
	
	*outLength -= stream.avail_out;
	return true;
}


void swft_import_png( xmlXPathParserContextPtr ctx, int nargs ) {
	xsltTransformContextPtr tctx;
	char *filename;
	xsltDocumentPtr xsltdoc;
	xmlDocPtr doc = NULL;
	xmlNodePtr node;
	xmlXPathObjectPtr obj;
	char tmp[TMP_STRLEN];
	png_colorp palette;
	int n_pal;
	int format = 5;
	int data_size = 0;

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:import-png() : invalid arg expecting a string\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
		
	tctx = xsltXPathGetTransformContext(ctx);
	
	filename = swft_get_filename(obj->stringval, ctx->context->doc->URL);

	bool quiet = true;
	xmlXPathObjectPtr quietObj = xsltVariableLookup( tctx, (const xmlChar*)"quiet", NULL );
	if (quietObj && quietObj->stringval) {
		quiet = !strcmp("true", (const char*)quietObj->stringval );
	}


	FILE *fp = fopen( filename, "rb" );
	if( !fp ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:import-png() : failed to read file '%s'\n", filename);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		goto fail;
	}
	
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"png", NULL );
	node = doc->xmlRootNode;
	
	swft_addFileName( node, filename );
	
	
	// add data
	rewind(fp);
	unsigned char *data, *compressed;
	unsigned long w, h, rowbytes;
	int channels;
	int compressed_size;
	if( !fp ) goto fail;
		
	if( readpng_init( fp, &w, &h ) ) goto fail;
	
	// add w/h
	snprintf(tmp, TMP_STRLEN, "%lu", w);
	xmlSetProp( node, (const xmlChar *)"width", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%lu", h);
	xmlSetProp( node, (const xmlChar *)"height", (const xmlChar *)&tmp );

	data = readpng_get_image( 2.2, &channels, &rowbytes, &palette, &n_pal );
	
	if( !quiet ) {
		fprintf(stderr,"Importing PNG: '%s' (%lu bit/pixel)\n", filename, (rowbytes*8)/w );
	}
	
	if( channels == 4 && rowbytes == (4*w) ) {
		int c;
		float a;
		unsigned char r,g,b;
		for( int i=0; i<w*h*4; i+=4 ) {
			a = data[i+3]/255.0;
			r = (unsigned char)((data[i+0])*a);
			g = (unsigned char)((data[i+1])*a);
			b = (unsigned char)((data[i+2])*a);
			data[i] = data[i+3];
			data[i+1] = r;
			data[i+2] = g;
			data[i+3] = b;
		}
		data_size = w*h*4;
	} else if( channels == 3 && rowbytes == (3*w) ) {
		unsigned char *rgba = new unsigned char[ w*h*4 ];
		
		for( int i=0; i<w*h; i++ ) {
			rgba[i*4] = 0xff;
			rgba[(i*4)+3] = data[(i*3)+2];
			rgba[(i*4)+2] = data[(i*3)+1];
			rgba[(i*4)+1] = data[(i*3)];
		}
		data = rgba;
		data_size = w*h*4;
	} else if( channels == 1 && rowbytes == w ) {
		unsigned char *img_data = data;
		format = 3;
		int bpr = rowbytes;
		bpr += (rowbytes % 4) ? 4 - (rowbytes % 4) : 0;
		if( n_pal ) {
			data_size = (4*n_pal) + (bpr*h);
			data = new unsigned char[ data_size ];
			for( int i=0; i<n_pal; i++ ) {
				unsigned char *entry = &data[(i*4)];
				entry[2] = palette[i].blue;
				entry[1] = palette[i].green;
				entry[0] = palette[i].red;
				entry[3] = 0xff;
			}
		} else {
			n_pal = 0xff;
			data_size = (4*n_pal) + (bpr*h);
			data = new unsigned char[ data_size ];
			for( int i=0; i<n_pal; i++ ) {
				unsigned char *entry = &data[(i*4)];
				entry[2] = i;
				entry[1] = i;
				entry[0] = i;
				entry[3] = 0xff;
			}
		}
		
		/* copy row by row with 32bit alignment */
		unsigned char *dst = &data[ (4*n_pal) ];
		unsigned char *src = img_data;
		memset( dst, 0, bpr*h );
		for( int y=0; y<h; y++ ) {
			memcpy( dst, src, rowbytes );
			dst += bpr;
			src += rowbytes;
		}
			
		snprintf(tmp,TMP_STRLEN,"%i", n_pal-1 );
		xmlSetProp( node, (const xmlChar *)"n_colormap", (const xmlChar *)&tmp );
	} else {
		fprintf(stderr,
				"WARNING: can only import 8bit palette, 24 or 32bit "
				"RGB(A) PNGs (%s has %i channels, rowstride %lu)\n",
				filename, channels, rowbytes);
		goto fail;
	}

	// format is 5 for RGB(A), 3 for palette (4 for 16bit, unused)
	snprintf(tmp,TMP_STRLEN,"%i", format );
	xmlSetProp( node, (const xmlChar *)"format", (const xmlChar *)&tmp );
	
	compressed_size = data_size;
	if( compressed_size < 1024 ) compressed_size = 1024;
	compressed = new unsigned char[ compressed_size ];
	if( compress( data, data_size, compressed, &compressed_size ) ) {
		swft_addData( node, (char*)compressed, compressed_size );
		valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
	}
	
	goto end;
	
fail:
	fprintf( stderr, "WARNING: could not import %s\n", filename );

end:
	if (fp) {
		fclose(fp);
	}
	delete compressed;
	delete filename; 
	readpng_cleanup( true );
}
