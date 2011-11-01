#include <libexslt/exslt.h>
#include "SWFFile.h"
#include "swft.h"
#include "../src/xslt/xslt.h"
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <dirent.h>

using namespace SWF;

bool quiet = false;
bool verbose = false;
bool dump = false;
bool nonet = false;
const char *swf_encoding = "UTF-8";
const char *internal_stylesheet = NULL;

void usage() {
	fprintf( stderr,
		"\n"PACKAGE_NAME" "PACKAGE_VERSION"\n"
		"    XML-based SWF processing tool\n"
		"\n"
		"usage: swfmill [<options>] <command>\n"
		"\n"
		"<command> is one of:\n"
		"    swf2xml <in> [<out>]\n"
		"        convert from SWF to XML.\n"
		"        <in> is a single SWF file, or 'stdin'\n"
		"        <out> is a single XML file, or (by default) 'stdout'\n"
		"\n"
		"    xml2swf <in> [<out>]\n"
		"        convert from XML to SWF.\n"
		"        <in> is a single XML file, or 'stdin'\n"
		"        <out> is a single SWF file, or (by default) 'stdout'\n"
		"\n"
		"    simple <in> [<out>]\n"
		"        convert from a movie definition file to SWF.\n"
		"        <in> is a single XML file, or 'stdin'\n"
		"        <out> is a single SWF file, or (by default) 'stdout'\n"
		"        (for details, see README)\n"
		"\n"
		"    xslt <xsl> <in> [<out>]\n"
		"        transform <in> to <out> as described by <xsl>.\n"
		"        <xsl> is the XSLT stylesheet,\n"
		"            and can use the swft: extension.\n"
		"        <in>  must be some XML (depends on <xsl>)\n"
		"        <out> is either SWF (when it ends in .swf)\n"
		"            or XML, by default on 'stdout'\n"
		"\n"
		"<option>s are:\n"
		"    -h print this help and quit\n"
		"    --version print the version number and quit\n"
		"    -v verbose output\n"
		"    -V extra-verbose debugging output\n"
		"    -d dump SWF data when loaded (for debugging)\n"
		"    -e specify text encoding in SWF (for SWF 5 and earlier only;\n"
		"           default: UTF-8).\n"
		"    -n deactivate libxml network access\n"
		"\n"
		"Please report bugs at https://github.com/djcsdy/swfmill/issues\n\n"
	);
}

void printVersion() {
	fprintf(stderr, PACKAGE_NAME" "PACKAGE_VERSION"\n");
}

// helper func
xsltStylesheetPtr xsltParseStylesheetMemory( const char *buffer, int size ) {
	xmlDocPtr doc = xmlParseMemory( buffer, size );
	if( !doc ) return NULL;
	xsltStylesheetPtr ret = xsltParseStylesheetDoc( doc );
	return ret;
}

int swfmill_swf2xml( int argc, char *argv[] ) {
	const char *infile = NULL, *outfile = "stdout";
	bool success = false;
	FILE *in_fp=0, *out_fp=0;
	bool std_in, std_out;

	File input;
	unsigned int filesize, size, xmlsize;
	struct stat filestat;
	char sig;
	Context ctx;

	// parse filenames
	if( argc < 1 || argc > 2 ) {
		usage();
		goto fail;
	}
	infile = argv[0];
	if( argc>1 ) outfile = argv[1];
	
	// open files
	std_in = !strncmp( infile, "stdin", 5 );
	std_out = !strncmp( outfile, "stdout", 6 );
	in_fp = std_in ? stdin : fopen( infile, "rb" );
	if( !in_fp ) {
		fprintf(stderr,"ERROR: could not open file %s for reading\n", infile?infile:"stdin" );
		goto fail;
	}
	
	if( !quiet ) fprintf(stderr,"Reading from %s\n", infile );
	
	// stat filesize
	filesize = (unsigned int)-1;
	if( !std_in ) {
		stat( infile, &filestat );
	filesize = filestat.st_size;
	}
	
	// setup context
	ctx.debugTrace = verbose;
	ctx.quiet = quiet;

	// setup encoding conversion.
	if (strcmp(swf_encoding, "UTF-8")) {
		ctx.convertEncoding = true;
		ctx.swf_encoding = swf_encoding;
	}

	// treat input as SWF, produce XML
	if( (size = input.load( in_fp, &ctx, filesize )) != 0 ) {
		if( dump ) input.dump();
		out_fp = std_out ? stdout : fopen( outfile, "wb" );
		if( !out_fp ) {
			fprintf(stderr,"ERROR: could not open file %s for writing\n", outfile );
			goto fail;
		}
		if( !quiet ) fprintf(stderr,"Writing XML to %s\n", outfile );
		if( (xmlsize = input.saveXML( out_fp, &ctx )) != 0 ) {
			if( !quiet ) fprintf(stderr,"XML saved to %s (%i bytes).\n", outfile, xmlsize );
			success = true;
		}
	}
fail:
	if( in_fp && !std_in ) fclose(in_fp);
	if( out_fp && !std_out ) fclose(out_fp);
	
	return success ? 0 : -1;
}


int swfmill_xml2swf( int argc, char *argv[] ) {
	const char *infile = NULL, *outfile = "stdout";
	bool success = false;
	FILE *in_fp=0, *out_fp=0;
	bool std_in, std_out;
	const char *filename;
	xsltStylesheetPtr transform = NULL;
	
	File input;
	int size, xmlsize;
	Context ctx;
	xmlDocPtr doc = NULL, doc2;

	// parse filenames
	if( argc < 1 || argc > 2 ) {
		usage();
		goto fail;
	}
	infile = argv[0];
	if( argc>1 ) outfile = argv[1];
	
	// open files
	std_in = !strncmp( infile, "stdin", 5 );
	std_out = !strncmp( outfile, "stdout", 6 );
	in_fp = std_in ? stdin : fopen( infile, "rb" );
	if( !in_fp ) {
		fprintf(stderr,"ERROR: could not open file %s for reading\n", infile?infile:"stdin" );
		goto fail;
	}
	
	if( !quiet ) fprintf(stderr,"Reading from %s\n", infile );
	
	// setup context
	ctx.debugTrace = verbose;
	ctx.quiet = quiet;

	// setup encoding conversion.
	if (strcmp(swf_encoding, "UTF-8")) {
		ctx.convertEncoding = true;
		ctx.swf_encoding = swf_encoding;
	}

	{
		filename = std_in ? "-" : infile ;
		doc = xmlParseFile( filename );
		if( !doc ) {
			fprintf( stderr, "ERROR: input document %s could not be read.\n", infile );
			goto fail;
		}

		// see if one of the internal stylesheets should be used
		if( internal_stylesheet ) {
			transform = xsltParseStylesheetMemory( internal_stylesheet, strlen(internal_stylesheet) );
			if( !transform ) {
				fprintf( stderr, "ERROR: internal stylesheet could not be read.\n" );
				goto fail;
			}

			const char *params[3];
			params[0] = "quiet";
			params[1] = "\"false\"";
			params[2] = NULL;
			if( quiet ) {
				params[1] = "\"true\"";
			}
			doc2 = xsltApplyStylesheet( transform, doc, (const char **)&params );

			if( !doc2 ) {
				fprintf( stderr, "ERROR: transformation failed.\n" );
				goto fail;
			}	
			
			xmlFreeDoc( doc );
			doc = doc2;
		}
		
		// treat input as XML, produce SWF
		input.setXML( doc->xmlRootNode, &ctx );
		if( dump ) input.dump();
		out_fp = std_out ? stdout : fopen( outfile, "wb" );
		if( !out_fp ) {
			fprintf(stderr,"ERROR: could not open file '%s' for writing\n", outfile );
			goto fail;
		}
		if( !quiet ) fprintf(stderr,"Writing SWF to %s\n", outfile );
		if( (size = input.save( out_fp, &ctx )) != 0 ) {
			success = true;
			if( !quiet ) fprintf(stderr,"SWF saved to %s (%i bytes).\n", outfile, size );
		}
	}
	
fail:
	if( doc ) xmlFreeDoc( doc );
	if( in_fp && !std_in ) fclose(in_fp);
	if( out_fp && !std_out ) fclose(out_fp);
	
	return success ? 0 : -1;
}


int swfmill_do_xslt( xmlDocPtr doc, xsltStylesheetPtr transform, const char *outfile );

int swfmill_xslt( int argc, char *argv[] ) {
	const char *xslfile, *infile, *outfile = "stdout";
	xmlDocPtr doc = NULL;
	xsltStylesheetPtr transform;
	
	// parse filenames
	if( argc < 2 || argc > 3 ) {
		usage();
		goto fail;
	}
	xslfile = argv[0];
	infile = argv[1];
	if( argc>2 ) outfile = argv[2];

	doc = xmlParseFile( infile );
	if( !doc ) {
		fprintf( stderr, "ERROR: input document %s could not be read.\n", infile );
		goto fail;
	}

	transform = xsltParseStylesheetFile( (const xmlChar *)xslfile );
	if( !transform ) {
		fprintf( stderr, "ERROR: stylesheet %s could not be read.\n", xslfile );
		goto fail;
	}
	
	return swfmill_do_xslt( doc, transform, outfile );
	
	fail:
		return -1;
}

int swfmill_do_xslt( xmlDocPtr doc, xsltStylesheetPtr transform, const char *outfile ) {
	xmlDocPtr doc2 = NULL;
	FILE *out_fp;
	const char *ext;
	Context ctx;
	int size;

	const char *params[3];
	params[0] = "quiet";
	params[1] = "\"FALSE\"";
	params[2] = NULL;
	if( quiet ) {
		params[1] = "\"FALSE\"";
	}
	
	// setup context
	ctx.debugTrace = verbose;
	ctx.quiet = quiet;

	
	if( !quiet ) fprintf( stderr, "Applying XSLT...\n" );
	doc2 = xsltApplyStylesheet( transform, doc, params );
	
	if( !doc2 ) {
		fprintf( stderr, "ERROR: transformation failed.\n" );
		goto fail;
	}	
	
	if( (ext = strrchr(outfile,'.'))!=NULL && !strncmp( ".swf", ext, 4 ) ) {
		if( !quiet ) fprintf( stderr, "Saving to SWF %s\n", outfile);		
		
		File swf;
		swf.setXML( doc2->xmlRootNode, NULL );

		out_fp = fopen( outfile, "wb" );
		if( !out_fp ) {
			fprintf(stderr,"ERROR: could not open file '%s' for writing\n", outfile );
			goto fail;
		}
		if( !quiet ) fprintf(stderr,"Writing SWF to %s\n", outfile );
		if( (size = swf.save( out_fp, &ctx )) != 0 ) {
			if( !quiet ) fprintf(stderr,"SWF saved to %s (%i bytes).\n", outfile, size );
			if( doc ) xmlFreeDoc(doc);
			if( doc2 ) xmlFreeDoc(doc2);
			if( transform ) xsltFreeStylesheet( transform );
			return 0;
		}
	} else {
		bool std_out = !strncmp( outfile, "stdout", 6 );
		const char *file = std_out ? "-" : outfile;
		return xsltSaveResultToFilename( file, doc2, transform, 0 );
	}
	
fail:
	if( doc ) xmlFreeDoc(doc);
	if( doc2 ) xmlFreeDoc(doc2);
	if( transform ) xsltFreeStylesheet( transform );
	return -1;
}


void swfmill_create_library( xmlNodePtr lib, const char *filename );

void swfmill_create_library_dir( xmlNodePtr lib, const char *dir ) {
	char tmp[1024];
	
	struct dirent *e;
	DIR *d = opendir( dir );
	if( d == NULL ) return;
		
	while( true ) {
		e = readdir(d);
		if( e==NULL ) break;
		if( e->d_name[0] == '.' ) continue; // no hidden files
		
		snprintf( tmp, sizeof(tmp), "%s/%s", dir, e->d_name );
		swfmill_create_library( lib, tmp );
	}
}

void swfmill_create_library( xmlNodePtr lib, const char *filename ) {
	struct stat s;
	if( stat(filename,&s) ) return;

/*
	int i=strlen(filename)-1;
	while( i>0 && filename[i]!='.' ) i--;
	const char *ext = &filename[i];
*/	
	if( S_ISDIR(s.st_mode) ) {
		swfmill_create_library_dir( lib, filename );
	} else {
//		if( !strcmp( ext, ".jpg" ) || !strcmp( ext, ".jpeg" ) ) {
		
		xmlNodePtr node = xmlNewChild( lib, NULL, (const xmlChar*)"clip", NULL );
		xmlSetProp( node, (const xmlChar *)"id", (const xmlChar *)filename );
		xmlSetProp( node, (const xmlChar *)"import", (const xmlChar *)filename );
	}
	
//	fprintf(stderr, "clip %s: %s (%s)\n", filename, S_ISDIR(s.st_mode)?"dir":"file", ext );
	
}

int swfmill_library( int argc, char *argv[] ) {
	if( argc < 2 ) {
		fprintf( stderr, "library creation needs at least two arguments\n\tswfmill library <input file(s)/dir(s)> <output>.swf\n" );
		return -1;
	}
	const char *outfile = argv[argc-1];
	argc--;
		
	internal_stylesheet = xslt_simple;
	
	xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
	doc->children = xmlNewDocNode( doc, NULL, (const xmlChar*)"movie", NULL );
	
	xmlNodePtr lib = xmlNewChild( doc->children, NULL, (const xmlChar *)"library", NULL );
	
	for( int i=0; i<argc; i++ ) {
		swfmill_create_library( lib, argv[i] );
	}
	
	xmlNodePtr node = xmlNewChild( lib, NULL, (const xmlChar *)"frame", NULL );
//	xmlSetProp( node, (const xmlChar*)"id", (const xmlChar *)"foo");
	
	xsltStylesheetPtr transform = xsltParseStylesheetMemory( internal_stylesheet, strlen(internal_stylesheet) );
	if( !transform ) {
		fprintf( stderr, "ERROR: internal stylesheet could not be read.\n" );
		return -1;
	}
	
	swfmill_do_xslt( doc, transform, outfile );
}

int main( int argc, char *argv[] ) {
	char *command = NULL;
	
	swft_register();
	exsltRegisterAll();

	quiet = true;
	
// parse args
	int i=1;
	for( ; i<argc && command==NULL; i++ ) {
		if (strncmp(argv[i], "--version", 10) == 0) {
			printVersion();
			return 0;
		}
		
		if( argv[i][0] == '-' ) {
			int swallow=0;
			for( int j=1; j<strlen(argv[i]); j++ ) {
				switch( argv[i][j] ) {
					case 'q':
						quiet = true;
						break;
					case 'v':
						quiet = false;
						break;
					case 'V':
						verbose = true;
						break;
					case 'd':
						dump = true;
						break;
					case 'n':
						nonet = true;
						break;
					case 'h':
					case '?':
						usage();
						return 0;
						break;
					case 'e':
						++swallow;
						if (i+swallow < argc) {
							swf_encoding = argv[i+swallow];
						} else {
							usage();
							goto fail;
						}
						break;
					default:
						fprintf(stderr,"ERROR: unknown option -%c\n",argv[i][j]);
						usage();
						goto fail;
				}
			}
			i+=swallow;
		} else {
			if( !command ) command=argv[i];
		}
	}
	
	if( !command ) {
		usage();
		goto fail;
	}
	
	if( nonet ) xmlSetExternalEntityLoader( xmlNoNetExternalEntityLoader );
	
	if( !quiet ) fprintf( stderr, "%s %s\n", PACKAGE_NAME, PACKAGE_VERSION );

	if( !strcmp( command, "swf2xml" ) ) {
		return swfmill_swf2xml( argc-i, &argv[i] );
	} else if( !strcmp( command, "xml2swf" ) ) {
		return swfmill_xml2swf( argc-i, &argv[i] );
	} else if( !strcmp( command, "xslt" ) ) {
		return swfmill_xslt( argc-i, &argv[i] );
	} else if( !strcmp( command, "simple" ) ) {
		internal_stylesheet = xslt_simple;
		return swfmill_xml2swf( argc-i, &argv[i] );
	} else if( !strcmp( command, "library" ) ) {
		return swfmill_library( argc-i, &argv[i] );
	} else {
		fprintf(stderr,"ERROR: unknown command %s\n", command );
		usage();
		goto fail;
	}
		
fail:
	return -1;
}

