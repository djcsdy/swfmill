#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define TMP_STRLEN 0xff

#define ERROR_NO_MP3              -1
#define ERROR_WRONG_SAMPLING_RATE -2

const int mp3SamplesPerFrame = 1152;
const int mp3Bitrates[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};

int findFrame( const unsigned char* data, int size, int start ) {
	int pos = start;

	while( pos < size ) {
		if( data[pos] == 0xFF && (data[pos + 1] & 0xE0) == 0xE0 ) 
			return pos;
		pos++;
	}

	return -1;
}

int getFrameSize( const unsigned char* data, int size, int pos ) {
	if( pos + 2 >= size ) {
		return ERROR_NO_MP3;
	}

	unsigned char c = data[pos + 1];
	int mpegVersion = (c & 0x18) >> 3;
	int layer = (c & 0x06) >> 1;
	
	//Verify that this a MP3 file
	//mpegVersion == 3 --> MPEG version 1
	//layer == 1 --> Layer 3
	if( mpegVersion != 3 || layer != 1) {
		return ERROR_NO_MP3;
	}

	c = data[pos + 2];
	int bitrate = (c & 0xF0) >> 4;
	int samplingRate = (c & 0x0C) >> 2;
	int padding = (c & 0x02) >> 1;

	//only 44100Hz is supported
	//this seems to be the only common sampling rate in flash and mp3
	if( samplingRate != 0 ) {
		return ERROR_WRONG_SAMPLING_RATE;
	}

	//Calculate the frame size in bytes
	int frameSize = (mp3SamplesPerFrame / 8) * (mp3Bitrates[bitrate] * 1000) / 44100;
	frameSize += padding;

	return frameSize;
}

struct MP3Info {
	int frames;
	int stereo;
	bool validMP3;
	bool wrongSamplingRate;
};

void getMP3Info( MP3Info& info, const unsigned char* data, int size ) {
	info.frames = 0;
	info.stereo = 0;
	info.validMP3 = true;
	info.wrongSamplingRate = false;
	int pos = 0;
	bool first = true;
	
	while( (pos = findFrame( data, size, pos )) >= 0 ) {
		int frameSize =	getFrameSize( data, size, pos );
		if( frameSize > 0 ) {
			if( first ) {
				if(pos + 3 < size) {
					if((data[pos + 3] & 0xC0) != 0xC0)
						info.stereo = 1;
				}
				first = false;
			}

			pos += frameSize;
			info.frames++;
		} else {
			if ( frameSize == ERROR_WRONG_SAMPLING_RATE ) {
				info.wrongSamplingRate = true;
			} else {
				info.validMP3 = false;
			}
			return;
		}
	}

	//no frames found -> no valid mp3
	if( info.frames == 0 ) {
		info.validMP3 = false;
	}
}


void swft_import_mp3( xmlXPathParserContextPtr ctx, int nargs ) {
	xsltTransformContextPtr tctx;
	xmlChar *filename;
	xsltDocumentPtr xsltdoc;
	xmlDocPtr doc = NULL;
	xmlNodePtr node;
	xmlXPathObjectPtr obj;
	char tmp[TMP_STRLEN];
	//data variables
	unsigned char *data = NULL;
	int size;
	struct stat filestat;

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
			 "swft:import-mp3() : invalid arg expecting a string\n");
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

	bool quiet = true;
	xmlXPathObjectPtr quietObj = xsltVariableLookup( tctx, (const xmlChar*)"quiet", NULL );
	if( quietObj && quietObj->stringval ) { quiet = !strcmp("true",(const char*)quietObj->stringval ); };

	
	FILE *fp = fopen( (const char *)filename, "rb" );
	if( !fp ) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				   "swft:import-mp3() : failed to read file '%s'\n", (const char *)filename);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
	
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"mp3", NULL );
	node = doc->xmlRootNode;
	
	swft_addFileName( node, (const char *)filename );
	
	// get file size
	if( stat( (const char *)filename, &filestat ) ) goto fail;
	size = filestat.st_size;
	
	// flash requires an initial latency value in front of the mp3 data
	// TODO: check the meaning of this value and set it correctly
	data = new unsigned char[size + 2];
	data[0] = 0;
	data[1] = 0;

	// read data
	if( fread( &data[2], 1, size, fp ) != size ) {
		fprintf(stderr,"WARNING: could not read enough (%i) bytes for MP3 %s\n", size, filename );
		goto fail;
	}
	
	if( size == 0 ) {
		fprintf(stderr,"WARNING: MP3 %s is empty\n", filename );
		goto fail;
	}

	MP3Info info;
	getMP3Info( info, &data[2], size );

	if( !info.validMP3 ) {
		fprintf(stderr,"WARNING: this file is not a valid MP3 %s\n", filename );
		goto fail;
	}

	if( info.wrongSamplingRate ) {
		fprintf(stderr,"WARNING: MP3 file %s has a wrong sampling rate (not 44.1kHz)\n", filename );
		goto fail;
	}

	xmlSetProp( node, (const xmlChar *)"format", (const xmlChar *)"2" ); //MP3
	xmlSetProp( node, (const xmlChar *)"rate", (const xmlChar *)"3" );
	xmlSetProp( node, (const xmlChar *)"is16bit", (const xmlChar *)"1" ); //MP3 is always 16bit
	snprintf(tmp,TMP_STRLEN,"%i", info.stereo);
	xmlSetProp( node, (const xmlChar *)"stereo", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%i", info.frames * 1152); //each frame has 1152 samples
	xmlSetProp( node, (const xmlChar *)"samples", (const xmlChar *)&tmp );
	
	if( !quiet ) {
		fprintf(stderr,"Importing MP3: '%s'\n",filename);
	}
	
	swft_addData( node, (char*)data, size + 2 );
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );

fail:	
	if( fp ) fclose(fp);
	if( data ) delete data;
}

