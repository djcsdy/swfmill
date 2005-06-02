#ifndef SWF_SWFT_H
#define SWF_SWFT_H

#ifdef _cplusplus
extern "C" {
#endif
	
void swft_register();
	
// utility functoins for import (in swft_import.cpp)
void swft_addFileName( xmlNodePtr node, const char *filename );
void swft_addData( xmlNodePtr node, char *data, int length );

#ifdef _cplusplus
}
#endif

#endif
