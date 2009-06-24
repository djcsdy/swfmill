#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>

#define TMP_STRLEN 0xff

#define ERROR_NO_WAV              -1
#define ERROR_WRONG_SAMPLING_RATE -2


struct WaveInfo {
	int samplingRate;
	int samples;
	int flashSamplingRateFlag;
	int stereo;
	int is16bit;
	int wave_data_size;
	const char *wave_data_ptr;
	bool valid;
	bool wrongSamplingRate;
};

typedef enum {
	WAVE_INVALID,
	WAVE_PCM = 1,
} WaveFormatCode;


void getWaveInfo( WaveInfo& info, char* data, int size ) {
	info.stereo = 0;
	info.wrongSamplingRate = false;
	int pos = 0;

	info.valid = false;

	const char *ptr = data;

	if (strncmp(ptr, "RIFF", 4) != 0) {
		fprintf(stderr, "Error: RIFF header missing\n");
		return;
	}

	ptr += 4;

	if ( *((unsigned int *)ptr) + 8 != size) {
		fprintf(stderr, "Error: File size differs from that described in RIFF header\n");
		return;
	}

	ptr += 4;

	if (strncmp(ptr, "WAVE", 4) != 0) {
		fprintf(stderr, "Error: Expected WAVE chunk\n");
		return;
	}

	ptr += 4;

	if (strncmp(ptr, "fmt ", 4) != 0) {
		fprintf(stderr, "Error: Expected fmt_ subchunk\n");
		return;
	}

	ptr += 4;

	/* Look ahead a bit */
	unsigned short format_code = *((unsigned short *)(ptr+4));
	if (format_code != WAVE_PCM) {
		fprintf(stderr, "Error: Format code in WAVE file is not PCM (%d)\n", format_code);
	}

	if ( *((unsigned int *)ptr) != 16) {
		fprintf(stderr, "Error: fmt_ subchunk size != 16. Is format code PCM?\n");
		return;
	}

	if (format_code != WAVE_PCM) {
		return;
	}


	/* Skip format code, already checked that */
	ptr += 6;

	unsigned short num_channels = *((unsigned short *)ptr);
	if (num_channels < 1 || num_channels > 2) {
		fprintf(stderr, "Error: Channels should be 1 or 2 (is %d)\n", num_channels);
		return;
	}

	info.stereo = num_channels == 2;

	ptr += 2;

	unsigned int sample_rate = *((unsigned int *)ptr);
	info.samplingRate = sample_rate;
	switch (sample_rate) {
		case 5512:
			info.flashSamplingRateFlag = 0;
			break;
		case 11025:
			info.flashSamplingRateFlag = 1;
			break;
		case 22050:
			info.flashSamplingRateFlag = 2;
			break;
		case 44100:
			info.flashSamplingRateFlag = 3;
			break;
		default:
			fprintf(stderr, "Invalid sampling rate, please use either 5.5k, 11k, 22k or 44k (is: %d Hz)\n", sample_rate);
			info.wrongSamplingRate = true;
			return;
	}

	/* Skip some derived fields */
	ptr += 10;

	unsigned short bits_per_sample = *((unsigned short *)ptr);
	if (bits_per_sample != 8 && bits_per_sample != 16) {
		fprintf(stderr, "Error: Bit per sample should be either 8 or 16 (is: %d)\n", bits_per_sample);
		return;
	}
	info.is16bit = bits_per_sample == 16;

	ptr += 2;

	if (strncmp(ptr, "data", 4) != 0) {
		fprintf(stderr, "Error: Expected data_ subchunk\n");
		return;
	}

	ptr += 4;

	info.wave_data_size = *((unsigned int*)ptr);

	info.samples = info.wave_data_size / num_channels / (info.is16bit ? 2 : 1);

	info.wave_data_ptr = ptr + 4;

	info.valid = true;
}

void swft_import_wav( xmlXPathParserContextPtr ctx, int nargs ) {
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
			 "swft:import-wav() : invalid arg expecting a string\n");
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
				   "swft:import-wav() : failed to read file '%s'\n", (const char *)filename);
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}
	
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"wav", NULL );
	node = doc->xmlRootNode;
	
	swft_addFileName( node, (const char *)filename );
	
	// get file size
	if( stat( (const char *)filename, &filestat ) ) {
		goto fail;
	}
	
	size = filestat.st_size;
	data = new unsigned char[size];

	// read data
	if( fread( data, 1, size, fp ) != size ) {
		fprintf(stderr,"WARNING: could not read enough (%i) bytes from WAVE file %s\n", size, filename );
		goto fail;
	}
	
	if( size == 0 ) {
		fprintf(stderr,"WARNING: WAVE file %s is empty\n", filename );
		goto fail;
	}

	WaveInfo info;
	getWaveInfo(info, (char *)data, size);

	if( !info.valid ) {
		fprintf(stderr,"WARNING: this file is not a valid WAVE file: %s\n", filename );
		goto fail;
	}

	if( info.wrongSamplingRate ) {
		fprintf(stderr,"WARNING: WAVE file %s has a wrong sampling rate\n", filename );
		goto fail;
	}

	xmlSetProp( node, (const xmlChar *)"format", (const xmlChar *)"3" ); // uncompressed little-endian

	snprintf(tmp,TMP_STRLEN,"%i", info.flashSamplingRateFlag);
	xmlSetProp( node, (const xmlChar *)"rate", (const xmlChar *)&tmp );

	snprintf(tmp,TMP_STRLEN,"%i", info.is16bit);
	xmlSetProp( node, (const xmlChar *)"is16bit", (const xmlChar *)&tmp );

	snprintf(tmp,TMP_STRLEN,"%i", info.stereo);
	xmlSetProp( node, (const xmlChar *)"stereo", (const xmlChar *)&tmp );

	snprintf(tmp,TMP_STRLEN,"%i", info.samples); 
	xmlSetProp( node, (const xmlChar *)"samples", (const xmlChar *)&tmp );

	if( !quiet ) {
		fprintf(stderr, "Importing WAVE file: '%s'\n", filename);
	}
	
	swft_addData( node, (char*)info.wave_data_ptr, info.wave_data_size );
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );

fail:
	if( fp ) fclose(fp);
	if( data ) delete data;
}

