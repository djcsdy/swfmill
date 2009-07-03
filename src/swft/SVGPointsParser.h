#ifndef SVG_POINTSPARSER_H
#define SVG_POINTSPARSER_H

#include <string>
#include <deque>
#include "Geom.h"



namespace SWF {

class PointsParser {
	public:
		void parse(const char *path);
		Point getPoint();
		int getPointCount();
	
	private:
		deque<double> coords;

};

}

#endif
