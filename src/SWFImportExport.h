#ifndef SWF_IMPORTEXPORT_H
#define SWF_IMPORTEXPORT_H

#include <libxml/tree.h>

namespace SWF {

class DefineBitsJPEG2;	
void exportDefineBitsJPEG2( DefineBitsJPEG2 *tag );
void importDefineBitsJPEG2( DefineBitsJPEG2 *tag, const char *filename, xmlNodePtr node );

class DefineBitsLossless2;	
void exportDefineBitsLossless2( DefineBitsLossless2 *tag );
void importDefineBitsLossless2( DefineBitsLossless2 *tag, const char *filename, xmlNodePtr node );

class DefineFont2;	
void exportDefineFont2( DefineFont2 *tag );
void importDefineFont2( DefineFont2*tag, const char *filename, xmlNodePtr node );

}

#endif
