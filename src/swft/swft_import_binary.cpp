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

void swft_import_binary( xmlXPathParserContextPtr ctx, int nargs ) {
	xsltTransformContextPtr tctx;
	xmlDocPtr doc = NULL;
	xmlNodePtr node;
	xmlXPathObjectPtr obj;
	unsigned char *data = NULL;

	xmlXPathStringFunction(ctx, 1);
	if (ctx->value->type != XPATH_STRING) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				"swft:import-binary() : invalid arg expecting a string\n");
		ctx->error = XPATH_INVALID_TYPE;
		return;
	}
	obj = valuePop(ctx);
	if (obj->stringval == NULL) {
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		return;
	}

	tctx = xsltXPathGetTransformContext(ctx);
	string filename = swft_get_filename(obj->stringval, ctx->context->doc->URL);

	bool quiet = true;
	xmlXPathObjectPtr quietObj = xsltVariableLookup(tctx, (const xmlChar*)"quiet", NULL);
	if (quietObj && quietObj->stringval) {
		quiet = !strcmp("true",(const char*)quietObj->stringval);
	}

	FILE *fp = fopen(filename.c_str(), "rb");
	if (!fp) {
		xsltTransformError(xsltXPathGetTransformContext(ctx), NULL, NULL,
				"swft:import-binary() : failed to read file '%s'\n", filename.c_str());
		valuePush(ctx, xmlXPathNewNodeSet(NULL));
		goto fail;
	}

	doc = xmlNewDoc((const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode(doc, NULL, (const xmlChar *)"binary", NULL);
	node = doc->xmlRootNode;
	
	swft_addFileName(node, filename.c_str());

	// add data
	int size;
	struct stat filestat;
	if (stat(filename.c_str(), &filestat)) {
		goto fail;
	}
	size = filestat.st_size;
	
	data = new unsigned char[size];
	if (fread(data, 1, size, fp) != size) {
		fprintf(stderr,"WARNING: could not read enough (%i) bytes for binary %s\n", size, filename.c_str());
		goto fail;
	}

	if (!quiet) {
		fprintf(stderr,"Importing binary: '%s'\n", filename.c_str());
	}

	swft_addData(node, (char*)data, size);
	valuePush(ctx, xmlXPathNewNodeSet((xmlNodePtr)doc));

fail:
	if (fp) {
		fclose(fp);
	}
	if (data) {
		delete data;
	}
}

