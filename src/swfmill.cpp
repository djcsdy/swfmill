#include <libexslt/exslt.h>
#include "SWFFile.h"
#include "swft.h"
#include "xslt.h"
#include <stdlib.h>
#include <string.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

using namespace SWF;

bool quiet = false;
bool verbose = false;
bool dump = false;
const char *internal_stylesheet = NULL;

void usage() {
	fprintf( stderr,
		"\n"PACKAGE_NAME" "PACKAGE_VERSION"\n"
		"     XML-based SWF (Shockwave Flash) processing tool\n"
		"\n"
		"usage: swfmill [<options>] <command>\n"
		"\n"
		"<command> is one of:\n"
		"    swf2xml <in> [<out>]\n"
		"           convert from SWF to XML.\n"
		"           <in> is a single SWF file, or 'stdin'\n"
		"           <out> is a single XML file, or (by default) 'stdout'\n"
		"\n"
		"    xml2swf <in> [<out>]\n"
		"           convert from XML to SWF.\n"
		"           <in> is a single XML file, or 'stdin'\n"
		"           <out> is a single SWF file, or (by default) 'stdout'\n"
		"\n"
		"    simple <in> [<out>]\n"
		"           convert from a movie definition file to SWF.\n"
		"           <in> is a single XML file, or 'stdin'\n"
		"           <out> is a single SWF file, or (by default) 'stdout'\n"
		"           (for details, see README)\n"
		"\n"
		"    xslt <xsl> <in> [<out>"
		//" [<param>*]"
		"]\n"
		"           transform <in> to <out> as described by <xsl>.\n"
		"           <xsl> is the XSLT stylesheet,\n"
		"                 and can use the swft: extension.\n"
		"           <in>  must be some XML (depends on <xsl>)\n"
		"           <out> is either SWF (when it ends in .swf)\n"
		"                 or XML, by default on 'stdout'\n"
//		"           <param>* is a whitespace-separated list of name=value\n"
//		"                 assignments for xsl parameters\n"
		"\n"
		"<option>s are:\n"
		"    -h print this help and quit\n"
		"    -q be quiet, only print errors and warnings\n"
		"    -v verbose output (for debugging)\n"
		"    -d dump SWF data when loaded (for debugging)\n"
		"\n"
		"E-mail bug reports to "PACKAGE_BUGREPORT"\n\n"
	);
}

// helper func
xsltStylesheetPtr xsltParseStylesheetMemory( const char *buffer, int size ) {
	xmlDocPtr doc = xmlParseMemory( buffer, size );
	if( !doc ) return NULL;
	xsltStylesheetPtr ret = xsltParseStylesheetDoc( doc );
//	xmlFreeDoc( doc );
	return ret;
}

int swfmill_swf2xml( int argc, char *argv[] ) {
	const char *infile = NULL, *outfile = "stdout";
	bool success = false;
	FILE *in_fp=0, *out_fp=0;
	bool std_in, std_out;

	File input;
	int size, xmlsize;
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
	
// setup context
	ctx.debugTrace = verbose;
	ctx.quiet = quiet;

		// treat input as SWF, produce XML
		if( (size = input.load( in_fp, &ctx )) != 0 ) {
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

			char *params[3];
			params[0] = NULL;
			
			if( quiet ) {
				params[0] = "quiet";
				params[1] = "true";
				params[2] = NULL;
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
		input.setXML( doc->xmlRootNode, NULL );
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


int swfmill_xslt( int argc, char *argv[] ) {
	const char *xslfile, *infile, *outfile = "stdout";
	xsltStylesheetPtr transform = NULL;
	xmlDocPtr doc = NULL, doc2 = NULL;
	FILE *out_fp;
	const char *ext;
	Context ctx;
	int size;

// setup context
	ctx.debugTrace = verbose;
	ctx.quiet = quiet;
	
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
	
	if( !quiet ) fprintf( stderr, "Applying XSLT %s to %s...\n", xslfile, infile );
	doc2 = xsltApplyStylesheet( transform, doc, NULL );
	
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


int main( int argc, char *argv[] ) {
	char *command = NULL;
	
	swft_register();
	exsltRegisterAll();

// parse args
	int i=1;
	for( ; i<argc && command==NULL; i++ ) {
		if( argv[i][0] == '-' ) {
			int swallow=0;
			for( int j=1; j<strlen(argv[i]); j++ ) {
				switch( argv[i][j] ) {
					case 'q':
						quiet = true;
						break;
					case 'v':
						verbose = true;
						break;
					case 'd':
						dump = true;
						break;
					case '?':
						usage();
						goto fail;
						break;
					default:
						fprintf(stderr,"ERROR: unknown option %c\n",argv[i][j]);
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
	} else {
		fprintf(stderr,"ERROR: unknown command %s\n", command );
		usage();
		goto fail;
	}
		
fail:
	return -1;
}

