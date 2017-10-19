#include <SWFOpCode.h>
#include "gSWF.h"

namespace SWF {

	OpCode *OpCode::get(Reader *r, int end, Context *ctx) {
		int type = r->getByte();
		int len = end - r->getPosition();

		OpCode* ret = getByType(type);

		if (!ret) {
			ret = new UnknownOpCode();
		}

		ret->setType(type);
		ret->setLength(len);
		ret->parse(r, end, ctx);

		return ret;
	}

}
