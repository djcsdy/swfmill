#ifndef SVG_PATHPARSER_H
#define SVG_PATHPARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <libxml/tree.h>
#include <cmath>
#include <cstdlib>
#include "SWFShapeMaker.h"
#include "SWF.h"



namespace SWF {

class PathParser {
	public:
		PathParser(ShapeMaker *shapeMaker);
		void parse(const char *path);
	
	private:
		std::vector<double> params;
		ShapeMaker *shaper;

		enum CharType {
			COMMAND_ABSOLUTE,
			COMMAND_RELATIVE,
			WHITESPACE,
			OTHER
		};

		void executeCommand(char command, bool repeated);

		CharType getCharType(char c) {
			switch(c) {
				case 'M':
				case 'Z':
				case 'L':
				case 'H':
				case 'V':
				case 'C':
				case 'S':
				case 'Q':
				case 'T':
				case 'A':
					return COMMAND_ABSOLUTE;
					break;

				case 'm':
				case 'z':
				case 'l':
				case 'h':
				case 'v':
				case 'c':
				case 's':
				case 'q':
				case 't':
				case 'a':
					return COMMAND_RELATIVE;
					break;

				case '\t':
				case '\n':
				case '\r':
				case ',':
				case ' ':
				case 0:
					return WHITESPACE;
					break;
					
				default:
					return OTHER;
					break;

			}
		}

		size_t getParameterCount(char command) {
			switch(command) {
				case 'z':
				case 'Z':
					return 0;
					break;

				case 'h':
				case 'H':
				case 'v':
				case 'V':
					return 1;
					break;

				case 'm':
				case 'M':
				case 'l':
				case 'L':
				case 't':
				case 'T':
					return 2;
					break;

				case 's':
				case 'S':
				case 'q':
				case 'Q':
					return 4;
					break;

				case 'c':
				case 'C':
					return 6;
					break;

				case 'a':
				case 'A':
					return 7;
					break;
			}
			return 0;
		}

};

}

#endif
