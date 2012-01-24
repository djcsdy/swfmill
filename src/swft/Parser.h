#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>

namespace SWF {

class Parser {
	public:
		Parser(const char begin, const char end, const char parameter = 0);
		virtual ~Parser();

	protected:
		virtual void handleData(const std::string& outside, const std::vector<std::string>& inside) = 0;
		virtual void doParse(const char* str);
		void trimString(std::string& s);
		bool isWhitespace(const char c);

	private:
		void handleDelimiter(std::string& tmp);
		void swapDelimiters();

		char expectedDelimiter;
		char otherDelimiter;
		char parameterDelimiter;
		bool insideParams;
		std::vector<std::string> params;
		std::string outside;
};

}

#endif
