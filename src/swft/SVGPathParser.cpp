#include "SVGPathParser.h"

using namespace std;

namespace SWF {

PathParser::PathParser(ShapeMaker *shapeMaker) {
	shaper = shapeMaker;
}

void PathParser::parse(const char *path) {
	char currentChar, currentCmd = 0;
	CharType charType;
	bool repeated = false;

	//use <= strlen() to make sure the last parameter is parsed correctly 
	for(size_t i = 0; i <= strlen(path); i++) {
		currentChar = path[i];
		charType = getCharType(currentChar);

		if(charType != OTHER) {
			if(params.size() != 0 && params.size() == getParameterCount(currentCmd)) {
				executeCommand(currentCmd, repeated);
				params.clear();

				repeated = true;
			}

			if(charType == COMMAND_ABSOLUTE || charType == COMMAND_RELATIVE) {
				if(params.size() > 0) {
					cerr << "WARNING: too much parameters in SVG path" << endl;
					params.clear();
				}
				
				currentCmd = currentChar;

				if(getParameterCount(currentCmd) == 0) {
					executeCommand(currentCmd, repeated);
				}

				repeated = false;
			}
		} else {
			char *str = (char*)path + i;
			char *tailPtr;
			
			params.push_back(strtod(str, &tailPtr));
			i += tailPtr - str - 1;
		}
		
	}
	if(params.size() > 0) {
		cerr << "WARNING: too much parameters in SVG path" << endl;
		params.clear();
	}
	shaper->close(false);
	shaper->finish();
}

void PathParser::executeCommand(char command, bool repeated) {
	switch(command) {
		case 'Q':
			shaper->curveTo(params[0], params[1], params[2], params[3]);
			break;

		case 'q':
			shaper->curveToR(params[0], params[1], params[2], params[3]);
			break;

		case 'T':
			shaper->smoothCurveTo(params[0], params[1]);
			break;

		case 't':
			shaper->smoothCurveToR(params[0], params[1]);
			break;

		case 'C':
			shaper->cubicTo(params[0], params[1], params[2], params[3], params[4], params[5]);
			break;

		case 'c':
			shaper->cubicToR(params[0], params[1], params[2], params[3], params[4], params[5]);
			break;

		case 'S':
			shaper->smoothCubicTo(params[0], params[1], params[2], params[3]);
			break;

		case 's':
			shaper->smoothCubicToR(params[0], params[1], params[2], params[3]);
			break;

		case 'L':
			shaper->lineTo(params[0], params[1]);
			break;

		case 'l':
			shaper->lineToR(params[0], params[1]);
			break;

		case 'M':
			if(repeated) {
				shaper->lineTo(params[0], params[1]);
			} else {
				shaper->close(false);
				shaper->setup(params[0], params[1]);
			}
			break;

		case 'm':
			if(repeated) {
				shaper->lineToR(params[0], params[1]);
			} else {
				shaper->close(false);
				shaper->setupR(params[0], params[1]);
			}
			break;

		case 'H':
			shaper->lineTo(params[0], shaper->getLastY());
			break;

		case 'h':
			shaper->lineToR(params[0], 0);
			break;
			
		case 'V':
			shaper->lineTo(shaper->getLastX(), params[0]);
			break;

		case 'v':
			shaper->lineToR(0, params[0]);
			break;

		case 'A':
			shaper->arcTo(params[0], params[1], params[2], (params[3] == 1), (params[4] == 1), params[5], params[6]);
			break;

		case 'a':
			shaper->arcToR(params[0], params[1], params[2], (params[3] == 1), (params[4] == 1), params[5], params[6]);
			break;

		case 'Z':
		case 'z':
			shaper->close();
			break;
		
		default:
			cerr << "WARNING: SVG path command not implemented (" << command << ")" << endl;
			break;
	}
}

}
