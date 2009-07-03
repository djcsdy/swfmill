#include "SVGAttributeParser.h"

#define DPI 90.0

using namespace std;

namespace SWF {

void AttributeParser::parseNode(xmlNodePtr node) {
	for(xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
		xmlChar *tmp = xmlGetProp(node, attr->name);
		if(tmp) {
			attributes[(char *)attr->name] = (char *)tmp;
			xmlFree(tmp);
		}
	}	

	xmlChar *tmp = xmlGetProp(node, (const xmlChar *)"style");
	if(tmp) {
		doParse((char *)tmp);
		xmlFree(tmp);
	}
}


double AttributeParser::getDouble(const char* attribute, double defaultValue, double value100) {
	const char *tmp = getString(attribute);
	if(tmp) {
		string tmpStr = tmp;

		char *tailPtr;
		double value = strtod(tmpStr.c_str(), &tailPtr);
				
		string unit = tailPtr;
		trimString(unit);

		if(unit == "in") {
			value *= DPI;
		} else if(unit == "cm") {
			value *= DPI / 2.54;
		} else if(unit == "mm") {
			value *= DPI / 25.4;
		} else if(unit == "pt") {
			value *= DPI / 72.0;
		} else if(unit == "pc") {
			value *= DPI / 72.0 * 12.0;
		} else if(unit == "%") {
			value *= value100 / 100;
		}
	
		return value;
	} else {
		return defaultValue;
	}
}

const char *AttributeParser::getString(const char* attribute) {
	map<string, string>::iterator iter = attributes.find(attribute);
	if(iter != attributes.end()) {
		return (*iter).second.c_str();
	} else {
		return NULL;
	}
}

const char *AttributeParser::operator[](const char* attribute) {
	return getString(attribute);
}

map<string, string> &AttributeParser::getAttributes() {
	return attributes;
}

void AttributeParser::handleData(const string& attrib, const vector<string>& value) {
	attributes[attrib] = value[0];
}

}
