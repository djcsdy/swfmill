#include "SWFImportExport.h"
#include "SWF.h"
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace SWF {

#define TMP_STRLEN 0xff
const char jpeg_header[] = { 0xff, 0xd9, 0xff, 0xd8, NULL };
		
void exportDefineBitsJPEG2( DefineBitsJPEG2 *tag ) {
	int id = tag->getobjectID();
	FILE *fp = 0;
	
	Rest *rest = tag->getdata();
	unsigned char *data;
	int size;
	rest->getdata( &data, &size );
	if( size ) {

/*		for( int i=0; i<4; i++ ) {
			fprintf(stderr,"%02X\n", data[i] );
		}
	*/	
		char tmp[TMP_STRLEN];
		snprintf( tmp, TMP_STRLEN, "%i.jpg", id );
		fp = fopen( tmp, "wb" );
		if( fp ) {
			int ofs=strlen(jpeg_header); // FIXME: this is not really good, it can well fail. implement the table/data separation as per specs?
			if( fwrite( &data[ofs], 1, size-ofs, fp )!=size-ofs ) {
				goto fail;
			}
				
//			fprintf(stderr,"JPEG data size: %i, id %i, saved to %s\n", size, id, tmp );
			fclose( fp );
		} else {
			fprintf(stderr,"WARNING: Could not export JPEG to %s\n", tmp );
		}
	}
	return;
	
fail:
	fprintf(stderr,"Could not export JPEG #%i\n", id );
	if( fp ) fclose(fp);
	return;
}

void importDefineBitsJPEG2( DefineBitsJPEG2 *tag, const char *filename, xmlNodePtr node ) {
	unsigned char *data;
	int size, ofs;
	struct stat filestat;
		
	Rest *rest = tag->getdata();
	
	FILE *fp = fopen( filename, "rb" );
	if( !fp ) goto fail;
	
	if( stat( filename, &filestat ) ) goto fail;
	
	size = filestat.st_size;
//	fprintf(stderr, "importing %s: %i bytes\n", filename, size );
	
	ofs = strlen(jpeg_header);
	
	data = new unsigned char[size+ofs];
	if( !data ) goto fail;
	memcpy( data, jpeg_header, ofs );
	if( fread( &data[ofs], 1, size, fp ) != size ) {
		fprintf(stderr,"WARNING: could not read enough (%i) bytes for jpeg %s\n", size, filename );
		goto fail;
	}
		
	rest->setdata( data, size+ofs );
	
	return;
	
fail:
	if( fp ) fclose(fp);
	if( data ) delete data;
	fprintf( stderr, "WARNING: could not import %s\n", filename );
	return;
}

}
