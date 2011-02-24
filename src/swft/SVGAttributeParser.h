#ifndef SVG_ATTRIBUTE_PARSER_H
#define SVG_ATTRIBUTE_PARSER_H

#include <string>
#include <map>
#include <iostream>
#include <libxml/tree.h>
#include "Parser.h"



namespace SWF {

class AttributeParser : public Parser {
	public:
		AttributeParser() :
			Parser(':', ';') {
		}

		void parseNode(xmlNodePtr node);
		const std::map<std::string, std::vector<std::string> >& getAttributes();

		double getDouble(const char* attribute, double defaultValue = 0, double value100 = 1);
		const char *getString(const char* attribute);
		const char *operator[](const char* attribute);

	protected:
		void doParse(const char* str);


	private:
		std::map<std::string, std::vector<std::string> > attributes;
		void handleData(const std::string& attrib, const std::vector<std::string>& value);
};

}

#endif
