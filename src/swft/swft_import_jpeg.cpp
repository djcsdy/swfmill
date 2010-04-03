#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include "readpng.h"
#include <zlib.h>

#define TMP_STRLEN 0xff
const char jpeg_header[] = { 0xff, 0xd9, 0xff, 0xd8, 0 };

// defined in swft_import_png
bool compress( unsigned char *inputBuffer, int inLength, unsigned char *outputBuffer, int *outLength );


int getJpegWord( FILE *fp ) {
	int r = fgetc(fp);
	r <<= 8;
	r += fgetc(fp);
	return r;
}

bool getJpegDimensions (FILE *infile, int &image_width, int &image_height) {
	image_width = image_height = 0;
	
	if (fgetc(infile) != 0xff || fgetc(infile) != 0xd8) {
		return false;
	}
	
	while (!feof(infile)) {
		if (fgetc(infile) != 0xff) {
			return false;
		}
		
		int marker;
		do {
			marker = fgetc(infile);
		} while (marker == 0xff);
		
		switch (marker) {
			case 0xc0:
			case 0xc1:
			case 0xc2:
			case 0xc3:
			case 0xc5:
			case 0xc6:
			case 0xc7:
			case 0xc9:
			case 0xca:
			case 0xcb:
			case 0xcd:
			case 0xce:
			case 0xcf:
			getJpegWord(infile);
			fgetc(infile);
			image_height = getJpegWord(infile);
			image_width = getJpegWord(infile);
			fgetc(infile);
			return true;
			
			case 0xda:
			case 0xd9:
			return false;
			
			default:
			int length = getJpegWord(infile);
			if (length < 2) {
				return false;
			} else {
				fseek(infile, length-2, SEEK_CUR);
			}
			break;
		}
	}
	
	return false;
}

void swft_import_jpeg( xmlXPathParserContextPtr ctx, int nargs ) {
	xsltTransformContextPtr tctx;
	unsigned char *filename;
	xsltDocumentPtr xsltdoc;
	xmlDocPtr doc = NULL;
	xmlNodePtr node;
	xmlXPathObjectPtr obj;
	char tmp[TMP_STRLEN];

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:import-jpeg() : invalid arg expecting a string\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
		
	tctx = xsltXPathGetTransformContext(ctx);
	filename = swft_get_filename( obj->stringval );

	bool quiet = true;
	xmlXPathObjectPtr quietObj = xsltVariableLookup( tctx, (const xmlChar*)"quiet", NULL );
	if( quietObj && quietObj->stringval ) { quiet = !strcmp("true",(const char*)quietObj->stringval ); };
	
	FILE *fp = fopen( (const char *)filename, "rb" );
	if( !fp ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:import-jpeg() : failed to read file '%s'\n", (const char *)filename);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
	
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"jpeg", NULL );
	node = doc->xmlRootNode;
	
	swft_addFileName( node, (const char *)filename );
	
	unsigned char *data = NULL;
	int width, height;
	if (!getJpegDimensions(fp, width, height)) {
		fprintf(stderr, "WARNING: could not extract dimensions for jpeg %s\n", filename);
		goto fail;
	}
	
	snprintf(tmp,TMP_STRLEN,"%i", width);
	xmlSetProp( node, (const xmlChar *)"width", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%i", height);
	xmlSetProp( node, (const xmlChar *)"height", (const xmlChar *)&tmp );
	
	// add data
	int size, ofs;
	struct stat filestat;
	if( stat( (const char *)filename, &filestat ) ) goto fail;
	size = filestat.st_size;
	
	ofs = strlen(jpeg_header);
	
	rewind(fp);
	data = new unsigned char[size+ofs];
	memcpy( data, jpeg_header, ofs );
	if( fread( &data[ofs], 1, size, fp ) != size ) {
		fprintf(stderr,"WARNING: could not read enough (%i) bytes for jpeg %s\n", size, filename );
		goto fail;
	}

	if( !quiet ) {
		fprintf(stderr,"Importing JPG: '%s'\n",filename);
	}
	
	swft_addData( node, (char*)data, size+ofs );
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );

fail:	
	if( fp ) fclose(fp);
	if( data ) delete data;
}


void swft_import_jpega( xmlXPathParserContextPtr ctx, int nargs ) {
	xsltTransformContextPtr tctx;
	xmlChar *filename, *maskfilename;
	xsltDocumentPtr xsltdoc;
	xmlDocPtr doc = NULL;
	xmlNodePtr node;
	xmlXPathObjectPtr obj;
	char tmp[TMP_STRLEN];

	if( (nargs != 2) ) {
		xmlXPathSetArityError(ctx);
		return;
	}

	maskfilename = swft_get_filename( xmlXPathPopString(ctx) );
	filename = swft_get_filename( xmlXPathPopString(ctx) );
	if( xmlXPathCheckError(ctx) || (filename==NULL) || (maskfilename==NULL) ) {
		return;
	}
		
	tctx = xsltXPathGetTransformContext(ctx);
		
	FILE *fp = fopen( (const char *)filename, "rb" );
	if( !fp ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:import-jpega() : failed to read file '%s'\n", (const char *)filename);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
	
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"jpega", NULL );
	node = doc->xmlRootNode;
	
	swft_addFileName( node, (const char *)filename );
	
	// figure width/height
	int width=-1, height=-1;
	while( !feof( fp ) ) { // could do a && width==-1 here, but that captures preview imgs...
		if( fgetc(fp) == 0xff ) {
			if( fgetc(fp) == 0xc0 ) {
				// StartOfFrame
				// skip length and precision (UGLY, eh?)
				fgetc(fp); fgetc(fp); fgetc(fp);
				
				// read height, width
				height = getJpegWord( fp );
				width = getJpegWord( fp );
			}
		}
	}
	snprintf(tmp,TMP_STRLEN,"%i", width);
	xmlSetProp( node, (const xmlChar *)"width", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%i", height);
	xmlSetProp( node, (const xmlChar *)"height", (const xmlChar *)&tmp );
	
	// add data
	unsigned char *jpegdata = NULL;
	unsigned char *data = NULL;
	int data_size, mask_size;
	int size, ofs;
	struct stat filestat;
	unsigned char *maskdata;
	unsigned long maskw, maskh, rowbytes;
	int channels;
	png_colorp palette;
	int n_pal;
	unsigned char *mask;
	if( stat( (const char *)filename, &filestat ) ) goto fail;
	size = filestat.st_size;
	
	ofs = strlen(jpeg_header);
	
	rewind(fp);
	jpegdata = new unsigned char[size+ofs];
	memcpy( jpegdata, jpeg_header, ofs );
	if( fread( &jpegdata[ofs], 1, size, fp ) != size ) {
		fprintf(stderr,"WARNING: could not read enough (%i) bytes for jpeg %s\n", size, filename );
		goto fail;
	}

	snprintf(tmp,TMP_STRLEN,"%i", size+ofs);
	xmlSetProp( node, (const xmlChar *)"offset_to_alpha", (const xmlChar *)&tmp );
	
	fclose(fp);
		
	// jpegdata is filled, now read the mask png.
	fp = fopen( (const char *)maskfilename, "rb" );
	if( !fp ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:import-jpega() : failed to read mask file '%s'\n", (const char *)maskfilename);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		goto fail;
	}

	data_size = size + ofs + (width*height);
	data = new unsigned char[ data_size ];
	memcpy( data, jpegdata, size+ofs );
	maskdata = &data[ size+ofs ];
	
	if( readpng_init( fp, &maskw, &maskh ) ) goto fail;
	
	if( maskw != width || maskh != height ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:import-jpega() : mask has different size than jpeg image: %i/%i and %i/%i\n", width, height, maskw, maskh );
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		goto fail;
	}

	mask = readpng_get_image( 2.2, &channels, &rowbytes, &palette, &n_pal );
	if( channels != 1 || rowbytes != width ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:import-jpega() : mask is not 8bit grayscale\n");
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		goto fail;
	}
	
	mask_size = data_size;
	if( compress( mask, width*height, maskdata, &mask_size ) ) {
		data_size = size + ofs + mask_size;
	} else {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:import-jpega() : could not compress mask\n" );
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		goto fail;
	}
	
	swft_addData( node, (char*)data, data_size );
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );

fail:	
	if( fp ) fclose(fp);
	if( data ) delete data;
	if( jpegdata ) delete jpegdata;
}

