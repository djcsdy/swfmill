#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxml/xpathInternals.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define TMP_STRLEN 0xff
const char jpeg_header[] = { 0xff, 0xd9, 0xff, 0xd8, 0 };

int getJpegWord( FILE *fp ) {
	int r = fgetc(fp);
	r <<= 8;
	r += fgetc(fp);
	return r;
}

void swft_import_jpeg( xmlXPathParserContextPtr ctx, int nargs ) {
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
	
	filename = obj->stringval;
		
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
	unsigned char *data = NULL;
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

	swft_addData( node, (char*)data, size+ofs );
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );

fail:	
	if( fp ) fclose(fp);
	if( data ) delete data;
}
