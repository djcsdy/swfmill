#include "xslt.h"
#include <cstring>

namespace SWF {
	
#define TMP_STRLEN 0xff

char *get_name_from_filename( const char *filename ) {
	char *name, *b;
	const char *basename;
	int l;
	
	// figure basename (filename without path)
	b = strrchr( filename, '/' );
	basename = b ? b+1 : filename;
	
	l = strlen(basename);
	name = new char[l];
	strncpy( name, basename, l );
	
	// separate extension (set position of last . as end of string)
	b = strrchr( name, '.' );
	if( b ) b[0]=0;
	
	return name;
}

// jpeg

int getJpegWord( FILE *fp ) {
	int r = fgetc(fp);
	r <<= 8;
	r += fgetc(fp);
	return r;
}

bool create_jpeg( const char *filename, xmlNodePtr parent ) {
	char *name = get_name_from_filename( filename );

	// parse thru the jpeg to find width and height
	FILE *fp = fopen( filename, "rb" );
	char tmp[TMP_STRLEN];
	if( !fp ) return false;
		
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
	
	xmlNodePtr node = xmlNewChild( parent, NULL, (const xmlChar *)"jpeg", NULL );

	snprintf(tmp,TMP_STRLEN,"%i", width);
	xmlSetProp( node, (const xmlChar *)"width", (const xmlChar *)&tmp );

	snprintf(tmp,TMP_STRLEN,"%i", height);
	xmlSetProp( node, (const xmlChar *)"height", (const xmlChar *)&tmp );

	xmlSetProp( node, (const xmlChar *)"file", (const xmlChar *)filename );
	xmlSetProp( node, (const xmlChar *)"name", (const xmlChar *)name );
	if( name ) delete name;
		
	return true;
}

bool create_swf( const char *filename, xmlNodePtr parent ) {
	char *name = get_name_from_filename( filename );
	xmlNodePtr node = xmlNewChild( parent, NULL, (const xmlChar *)"swf", NULL );

	xmlSetProp( node, (const xmlChar *)"file", (const xmlChar *)filename );
	xmlSetProp( node, (const xmlChar *)"name", (const xmlChar *)name );
	
	if( name ) delete name;
		
	return true;
}

bool create_font( const char *filename, xmlNodePtr parent ) {
	char *name = get_name_from_filename( filename );
	xmlNodePtr node = xmlNewChild( parent, NULL, (const xmlChar *)"font", NULL );

	xmlSetProp( node, (const xmlChar *)"file", (const xmlChar *)filename );
	xmlSetProp( node, (const xmlChar *)"name", (const xmlChar *)name );
	
	if( name ) delete name;
		
	return true;
}

typedef bool createNodeFunction( const char *filename, xmlNodePtr parent );
struct NodeTypes {
	const char *extension;
	createNodeFunction *create;
};
NodeTypes nodeTypes[] = {
		{ ".jpg", create_jpeg },
		{ ".jpeg", create_jpeg },
		{ ".swf", create_swf },
		{ ".ttf", create_font },
		{ NULL, NULL }
	};


}

using namespace SWF;

xmlDocPtr xslt_pack( int argc, char *argv[], int width, int height, float framerate ) {
	int nObjects = 0;
	xmlDocPtr doc;
	xmlNodePtr root;
	doc = xmlNewDoc((const xmlChar*)"1.0");
	root = doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"files", NULL );

	// add header
	char tmp[TMP_STRLEN];
	
	snprintf(tmp,TMP_STRLEN,"%i", width);
	xmlSetProp( root, (const xmlChar *)"right", (const xmlChar *)&tmp );

	snprintf(tmp,TMP_STRLEN,"%i", height);
	xmlSetProp( root, (const xmlChar *)"bottom", (const xmlChar *)&tmp );

	snprintf(tmp,TMP_STRLEN,"%f", framerate);
	xmlSetProp( root, (const xmlChar *)"framerate", (const xmlChar *)&tmp );

	
	// add nodes for every file
	const char *filename;
	for( int i=0; i<argc; i++ ) {
		filename = argv[i];
		char *ext = strrchr( filename, '.' );
		if( !ext ) {
			fprintf(stderr, "Unsure what to do with '%s' (no extension), ignoring.\n", filename );
		} else {
			int h=0;
			while( h>=0 && nodeTypes[h].extension != NULL ) {
				if( !strcasecmp( nodeTypes[h].extension, ext ) ) {
					if( nodeTypes[h].create( filename, root ) ) {
						h=-1;
						nObjects++;
					}
				}
				if( h>=0 ) h++;
			}
			if( h>=0 ) {
				fprintf(stderr, "No import function for '%s', ignoring.\n", filename );
			}
		}
	}
	
	if( !nObjects ) {
		xmlFreeDoc( doc );
		return NULL;
	}
	
	return doc;
}
