#ifndef SVG_TRANSFORMPARSER_H
#define SVG_TRANSFORMPARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <libxml/tree.h>
#include "Geom.h"
#include "Parser.h"

using namespace std;

namespace SWF {

class TransformParser : public Parser {
	public:
		TransformParser() :
			Parser('(', ')', ',') {
		}
		Matrix getMatrix() { return transformMatrix; }
		void parse(const char* str) { doParse(str); }

	private:
		Matrix transformMatrix;

		void handleData(const string& transformType, const vector<string>& params);
		void printWarning(const string& transformType);
};

}

#endif
