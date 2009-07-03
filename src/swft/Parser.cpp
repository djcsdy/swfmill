#include "Parser.h"
#include <cstring>

namespace SWF {

Parser::Parser(const char begin, const char end, const char parameter) {
	expectedDelimiter = begin;
	otherDelimiter = end;
	parameterDelimiter = parameter;
}

Parser::~Parser() {
}

void Parser::swapDelimiters() {
	char tmp = otherDelimiter;
	otherDelimiter = expectedDelimiter;
	expectedDelimiter = tmp;
}

bool Parser::isWhitespace(const char c) {
	return (c == 0 || c == '\n' || c == '\r' || c == '\t' || c == ' '); 	
}

void Parser::doParse(const char* str) {
	string tmp;
		
	insideParams = false;
	params.clear();

	for (size_t i = 0; i < strlen(str); i++) {
		if(str[i] == expectedDelimiter) {
			handleDelimiter(tmp);	
		} else if(str[i] == otherDelimiter) {
			cerr << "WARNING: Unexpected delimiter while parsing" << endl;
		} else {
			if(tmp.length() > 0 && (str[i] == parameterDelimiter || isWhitespace(str[i]))) {
				trimString(tmp);
				params.push_back(tmp);
				tmp.clear();
			} else {
				tmp += str[i];
			}
		}
	}
		
	if (tmp.length() > 0) {
		//cerr << "WARNING: Reached end of line while parsing" << endl;
		handleDelimiter(tmp);
	}
}

void Parser::handleDelimiter(string& tmp) {
	trimString(tmp);
		
	if(insideParams) {
		if(tmp.length() > 0) {
			params.push_back(tmp);
		}
		handleData(outside, params);
		params.clear();
	} else {
		outside = tmp;
	}
	
	insideParams = !insideParams;
	tmp.clear();
	swapDelimiters();
}

void Parser::trimString(string& s) {
	s.erase(0, s.find_first_not_of(" \t\r\n"));
	s.erase(s.find_last_not_of(" \t\r\n") + 1);
}

}
