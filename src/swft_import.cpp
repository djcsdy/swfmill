#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxml/xpathInternals.h>
#include "base64.h"
#include <string.h>

void swft_addFileName( xmlNodePtr node, const char *filename ) {
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
		
	xmlSetProp( node, (const xmlChar *)"name", (const xmlChar *)name );
	
	delete name;
}

void swft_addData( xmlNodePtr node, char *data, int sz ) {
	char *tmpstr = new char[ (sz * 3) ];
	
	int l = base64_encode( tmpstr, data, sz );
	if( l > 0 ) {
		tmpstr[l] = 0;
		xmlNewTextChild( node, NULL, (const xmlChar *)"data", (const xmlChar *)tmpstr );
	}
	delete tmpstr;
}
