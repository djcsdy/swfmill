#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxml/xpathInternals.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
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
		fprintf( stderr, "Error compressing PNG (1): %s\n", stream.msg );
		return false;
	}
	
	while( true ) {
		if( stream.avail_in == 0 ) break;

		status = deflate( &stream, Z_NO_FLUSH );
		
		if( status != Z_OK ) {
			fprintf( stderr, "Error compressing PNG (2): %s\n", stream.msg );
			return false;
		}
	}

	do {
		status = deflate( &stream, Z_FINISH );
	} while( status == Z_OK );
	
	if( status != Z_STREAM_END ) {
		fprintf( stderr, "Error compressing PNG (3): %s\n", stream.msg );
		return false;
	}

	status = deflateEnd(&stream);

	if( status != Z_OK ) {
		fprintf( stderr, "Error compressing PNG (4): %s\n", stream.msg );
		return false;
	}
	
	*outLength -= stream.avail_out;
	return true;
}

		
void swft_import_png( xmlXPathParserContextPtr ctx, int nargs ) {
	xsltTransformContextPtr tctx;
	xmlChar *filename;
	xsltDocumentPtr xsltdoc;
	xmlDocPtr doc = NULL;
	xmlNodePtr node;
	xmlXPathObjectPtr obj;
	char tmp[TMP_STRLEN];

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
	
	filename = obj->stringval;
		
	FILE *fp = fopen( (const char *)filename, "rb" );
	if( !fp ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:import-png() : failed to read file '%s'\n", (const char *)filename);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
	
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"png", NULL );
	node = doc->xmlRootNode;
	
	swft_addFileName( node, (const char *)filename );
	
	
	// add data
	rewind(fp);
	unsigned char *data, *compressed;
	unsigned long w, h, rowbytes;
	int channels;
	int compressed_size;
	if( !fp ) goto fail;
		
	if( readpng_init( fp, &w, &h ) ) goto fail;
	// add w/h
	snprintf(tmp,TMP_STRLEN,"%i", w);
	xmlSetProp( node, (const xmlChar *)"width", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%i", h);
	xmlSetProp( node, (const xmlChar *)"height", (const xmlChar *)&tmp );

	data = readpng_get_image( 2.2, &channels, &rowbytes );
	
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
	} else if( channels == 3 && rowbytes == (3*w) ) {
		unsigned char *rgba = new unsigned char[ w*h*4 ];
		
		for( int i=0; i<w*h; i++ ) {
			rgba[i*4] = 0xff;
			rgba[(i*4)+3] = data[(i*3)+2];
			rgba[(i*4)+2] = data[(i*3)+1];
			rgba[(i*4)+1] = data[(i*3)];
		}
		data = rgba;
	} else {
		fprintf( stderr, "WARNING: can only import 24 or 32bit RGB(A) PNGs (%s has %i channels, rowstride %i)\n", filename, channels, rowbytes );
		goto fail;
	}

	// format is always 5.
	snprintf(tmp,TMP_STRLEN,"%i", 5);
	xmlSetProp( node, (const xmlChar *)"format", (const xmlChar *)&tmp );
	
	compressed_size = w*h*4;
	if( compressed_size < 64 ) compressed_size = 64;
	compressed = new unsigned char[ compressed_size ];
	if( compress( data, w*h*4, compressed, &compressed_size ) ) {
		swft_addData( node, (char*)compressed, compressed_size );
		valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
	}
	
	delete compressed;
	readpng_cleanup( true );
	if( fp ) fclose(fp);
	return;
	
fail:
	readpng_cleanup( true );
	if( fp ) fclose(fp);
	fprintf( stderr, "WARNING: could not import %s\n", filename );
	return;
}
