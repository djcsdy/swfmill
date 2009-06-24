#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>

#define TMP_STRLEN 0xff

#define ERROR_NO_MP3              -1
#define ERROR_WRONG_SAMPLING_RATE -2

enum {
	MPEG_V25 = 0,
	MPEG_RESERVED,
	MPEG_V2,
	MPEG_V1,
};

const int mpegVersionBitrate[] = {
	1, // V2.5, bitrates same as V2
	-1, 
	1, // V2
	0  // V1
}; 

// Only Layer3 is supported
const int mp3Bitrates[][15] = {
	{0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320}, // V1
	{0,  8, 16, 24, 32, 40, 48, 56,  64,  80,  96, 112, 128, 144, 160},   // V2 & V2.5
};


/* As required in DefineSound, as a function of mpegVersion */
const int flashSamplingRates[] = {
	1,  // V25 -> 11025,   
	-1, // dummy
	2,  // V2 -> 22050,   
	3,  // V1 -> 44100;
};


const int samplingRates[][4] = {
	{11025,  12000,    8000,  -1}, // V2.5
	{   -1,     -1,      -1,  -1}, // dummy
	{22050,  24000,   12000,  -1}, // V2
	{44100,  48000,   32000,  -1}, // V1
};

struct MP3Info {
	int samplingRate;
	int samplesPerFrame;
	int flashSamplingRateFlag;
	int frames;
	int stereo;
	bool validMP3;
	bool wrongSamplingRate;
};

int findFrame( const unsigned char* data, int size, int start ) {
	int pos = start;

	while( pos < size ) {
		if( data[pos] == 0xFF && (data[pos + 1] & 0xE0) == 0xE0 ) {
			return pos;
		}
		pos++;
	}

	return -1;
}

int getFrameSize( const unsigned char* data, int size, int pos, MP3Info &info) {
	if( pos + 2 >= size ) {
		return ERROR_NO_MP3;
	}

	unsigned char c = data[pos + 1];
	int mpegVersion = (c & 0x18) >> 3;  // 0:V2.5   1:reserved  2:V2  3:V1
	int layer = (c & 0x06) >> 1;  // 1 means Layer III
	
	// An MP3 file is Layer III, MPEG version any
	if( layer != 1) {
		fprintf(stderr, "Error: Layer should be III.\n");
		return ERROR_NO_MP3;
	}

	if (mpegVersion == MPEG_RESERVED) {
		fprintf(stderr, "Error: Unknown MPEG version (reserved).\n");
		return ERROR_NO_MP3;
	}

	c = data[pos + 2];
	int bitrate = (c & 0xF0) >> 4;
	int samplingRate = (c & 0x0C) >> 2;
	int padding = (c & 0x02) >> 1;
	
	if (bitrate > 14) {
		fprintf(stderr, "MP3 bitrate field invalid. Corrupt MP3 file?");
		return ERROR_NO_MP3;
	}

	info.samplingRate = samplingRates[mpegVersion][samplingRate];
	info.flashSamplingRateFlag = flashSamplingRates[mpegVersion];

	if( samplingRate != 0 ) {
		fprintf(stderr, "Sampling rate: %d\n", info.samplingRate);
		fprintf(stderr, "Error: Flash only supports sampling rates of 44100, 22050 and 11025 Hz\n");
		return ERROR_WRONG_SAMPLING_RATE;
	}
   
	info.samplesPerFrame = mpegVersion == MPEG_V1 ? 1152 : 576; // Since we deal with Layer III only

	//Calculate the frame size in bytes
	int br_table = mpegVersionBitrate[mpegVersion];
	int frameSize = (info.samplesPerFrame / 8) * (mp3Bitrates[br_table][bitrate] * 1000) / info.samplingRate + padding;

	return frameSize;
}

void getMP3Info( MP3Info& info, const unsigned char* data, int size ) {
	info.frames = 0;
	info.stereo = 0;
	info.validMP3 = true;
	info.wrongSamplingRate = false;
	int pos = 0;
	bool first = true;
	
	while( (pos = findFrame( data, size, pos)) >= 0 ) {
		int frameSize =	getFrameSize( data, size, pos, info );
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
		fprintf(stderr,"WARNING: MP3 file %s has a wrong sampling rate\n", filename );
		goto fail;
	}

	xmlSetProp( node, (const xmlChar *)"format", (const xmlChar *)"2" ); //MP3
	
	snprintf(tmp,TMP_STRLEN,"%i", info.flashSamplingRateFlag);
	xmlSetProp( node, (const xmlChar *)"rate", (const xmlChar *)&tmp );
	
	xmlSetProp( node, (const xmlChar *)"is16bit", (const xmlChar *)"1" ); //MP3 is always 16bit
	
	snprintf(tmp,TMP_STRLEN,"%i", info.stereo);
	xmlSetProp( node, (const xmlChar *)"stereo", (const xmlChar *)&tmp );
	
	snprintf(tmp,TMP_STRLEN,"%i", info.frames * info.samplesPerFrame); 
	xmlSetProp( node, (const xmlChar *)"samples", (const xmlChar *)&tmp );
	
	if( !quiet ) {
		fprintf(stderr, "Importing MP3: '%s'\n", filename); 
	}
	
	swft_addData( node, (char*)data, size + 2 );
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );

fail:
	if( fp ) fclose(fp);
	if( data ) delete data;
}

