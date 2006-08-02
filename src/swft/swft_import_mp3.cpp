#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxslt/variables.h>
#include <libxml/xpathInternals.h>
#include "swft.h"
#include <sys/types.h>
#include <sys/stat.h>

#define TMP_STRLEN 0xff

const int mp3SamplesPerFrame = 1152;
const int mp3Bitrates[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};
const int mp3SamplingRates[] = {44100, 48000, 32000};

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
	
	//TODO: load all of this from the sound file.
	int format = 2; //MP3
	int rate = 3; //44100 Hz
	int is16bit = 1;
	int stereo = 1;
	
	// figure number of samples
	/* The frame's header has 11 consecutive 1s set (FF E0) at the beggining.
	 * Since we are only supporting MP3s (MPEG 1 Layer 3) we are going to chech
	 * that MPEG version is 1 and the Layer is 3.
	 */
	int lastChar;
	int mpegVersion;
	int layer;
	int bitrate;
	int samplingRate;
	int padding;
	int frameSize;
	int frames = 0;
	bool first = true;
	bool finished = false;
	if (!feof(fp)) {
		lastChar = fgetc(fp);
	}
	while( !feof( fp ) && !finished) {
		if( lastChar == 0xFF) {
			lastChar = fgetc(fp);
			if ((lastChar & 0xE0) == 0xE0) {
				//fprintf(stdout,"INFO: Frame found. LastChar: %i\n", lastChar );
				//We found a frame!
				frames++;
				first = false; //We already found the first frame
				//Lets get the header info.
				mpegVersion = (lastChar & 0x18) >> 3;
				layer = (lastChar & 0x06) >> 1;
				lastChar = fgetc(fp); //Read the next byte
				bitrate = (lastChar & 0xF0) >> 4;
				samplingRate = (lastChar & 0x0C) >> 2;
				padding = (lastChar & 0x02) >> 1;
				//Verify that this a MP3 file
				//mpegVersion == 3 --> MPEG version 1
				//layer == 1 --> Layer 3
				if( mpegVersion != 3 || layer != 1) {
					fprintf(stderr,"WARNING: this file is not a valid MP3 %s\n", filename );
					goto fail;
				}
				//Calculate the frame size in bytes
				frameSize = (
								(mp3SamplesPerFrame / 8) *
								(mp3Bitrates[bitrate] * 1000) / 
								mp3SamplingRates[samplingRate]
							) + 
							padding;
				//fprintf(stdout,"INFO: Frame size: %i\n", frameSize );
				//We are going to skip the rest of the data and go straight to the next frame.
				//We have already read 3 bytes
				for (int i = 0; i < frameSize - 3 && !feof(fp); i++) {
					fgetc(fp);
				}
				if (!feof(fp))
					lastChar = fgetc(fp); //Hopefully, the first byte of the next frame.
				//fprintf(stdout,"INFO: Data skipped. LastChar: %i\n", lastChar );
			} else {
				//If we are still looking for the first frame, move on, otherwise, we are done.
				//fprintf(stderr,"WARNING: Frame search finished (second byte). LastChar: %i\n", lastChar );
				finished = !first;
			}
		} else {
			//If we are still looking for the first frame, move on, otherwise, we are done.
			if (first)
				lastChar = fgetc(fp);
			else {
				//fprintf(stderr,"WARNING: Frame search finished (first byte). LastChar: %i\n", lastChar );
				finished = true;
			}
		}
	}
	snprintf(tmp,TMP_STRLEN,"%i", format);
	xmlSetProp( node, (const xmlChar *)"format", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%i", rate);
	xmlSetProp( node, (const xmlChar *)"rate", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%i", is16bit);
	xmlSetProp( node, (const xmlChar *)"is16bit", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%i", stereo);
	xmlSetProp( node, (const xmlChar *)"stereo", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%i", frames * 1152); //Each frame has 1152 samples.
	xmlSetProp( node, (const xmlChar *)"samples", (const xmlChar *)&tmp );
	
	// add data
	if( stat( (const char *)filename, &filestat ) ) goto fail;
	size = filestat.st_size;
	
	rewind(fp);
	data = new unsigned char[size];
	if( fread( &data[0], 1, size, fp ) != size ) {
		fprintf(stderr,"WARNING: could not read enough (%i) bytes for MP3 %s\n", size, filename );
		goto fail;
	}
	if( !quiet ) {
		fprintf(stderr,"Importing MP3: '%s'\n",filename);
	}
	
	swft_addData( node, (char*)data, size/*+ofs*/ );
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );

fail:	
	if( fp ) fclose(fp);
	if( data ) delete data;
}

