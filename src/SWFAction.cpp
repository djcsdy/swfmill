#include <SWFAction.h>
#include "gSWF.h"
#include <cstring>

namespace SWF {

	Action *Action::get(Reader *r, int end, Context *ctx) {
		uint16_t h = r->getByte();
		int type = h;
		int len = 0;
		if (type >= 0x80) {
			len = r->getWord();
		}

		if (type == 0) {
			return new EndAction; // terminator
		}

		Action *ret = getByType(type);

		if (!ret) {
			ret = new UnknownAction;
		}

		ret->setType(type);
		ret->setLength(len);
		ret->parse(r, r->getPosition()+len, ctx);

		return ret;
	}

	void Action::writeHeader(Writer *w, Context *ctx, size_t len) {
		w->putByte(type);
		if (type >= 0x80) {
			w->putWord(len-=3);
		}
	}

}
