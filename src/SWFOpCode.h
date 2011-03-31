#ifndef SWF_OPCODE_H
#define SWF_OPCODE_H

#include <SWFIdItem.h>

namespace SWF {
	
class OpCode : public IdItem<OpCode> {
	// static stuff
	public:
		static OpCode* get( Reader *r, int end, Context *ctx );
};

}
#endif
