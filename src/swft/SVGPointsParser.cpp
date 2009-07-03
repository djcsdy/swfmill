#include "SVGPointsParser.h"

#include <iostream>
#include <cstring>

using namespace std;

namespace SWF {

void PointsParser::parse(const char *points) {
	string tmp;

	coords.clear();
	
	for(int i = 0; i < strlen(points); i++) {
		char currentChar = points[i];
		
		if(currentChar == ',' || currentChar == ' ') {
			if(tmp.length() > 0) {
				coords.push_back(atof(tmp.c_str()));
				tmp.clear();
			}			
		} else {
			tmp += currentChar;
		}
	}
	
	if(tmp.length() > 0) {
		coords.push_back(atof(tmp.c_str()));
	}
	
	if(coords.size() % 2) {
		cerr << "WARNING: odd number of coordinates in points attribute" << endl;
	}
}

int PointsParser::getPointCount() {
	return coords.size() / 2;
}

Point PointsParser::getPoint() {
	double x = coords.front();
	coords.pop_front();
		
	double y = coords.front();
	coords.pop_front();
	
	return Point(x, y);
}
	
}
