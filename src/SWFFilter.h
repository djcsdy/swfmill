#ifndef SWF_FILTER_H
#define SWF_FILTER_H

#include <SWFIdItem.h>

namespace SWF {
	
class Filter : public IdItem<Filter> {
	// static stuff
	public:
		static Filter* get( Reader *r, int end, Context *ctx );
};

}
#endif
