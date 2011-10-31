#include "SVGAttributeParser.h"
#include <cstring>

#define DPI 90.0

using namespace std;

namespace SWF {

void AttributeParser::parseNode(xmlNodePtr node) {
	for(xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
		xmlChar *tmp = xmlGetProp(node, attr->name);
		if(tmp) {
			vector<string> values;
			values.push_back ((const char *) tmp);
			attributes[(const char *)attr->name] = values;
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
	map<string, vector<string> >::iterator iter = attributes.find(attribute);
	if (iter == attributes.end ()) {
		return NULL;
	}

	return (*iter).second [0].c_str ();
}

const char *AttributeParser::operator[](const char* attribute) {
	return getString(attribute);
}

const map<string, vector<string> > &AttributeParser::getAttributes() {
	return attributes;
}

void AttributeParser::handleData(const string& attrib, const vector<string>& value) {
	// Unused.
}



void AttributeParser::doParse(const char* str) {


	int ich = 0;
	int cch = strlen (str);
	char ch;

	while (ich < cch) {
		string attrib;

		while (ich < cch && (ch = str[ich++]) != ':') {
			attrib += ch;
		}

		trimString (attrib);

		vector<string> params;

		bool fNextAttribute = false;

		while (ich < cch && !fNextAttribute)
		{
			// skip whitespace
			while (ich < cch && isWhitespace (str [ich])) {
				ich ++;
			}

			string value;

			int cParens = 0;

			while (ich < cch) {
				char ch = str [ich++];

				if (cParens <= 0) {
					if (ch == ';') {
						fNextAttribute = true;
						break;
					}

					if (isWhitespace (ch)) {
						break;
					}
				}

				value += ch;

				if (ch == '(') {
					cParens ++;
				} else if (ch == ')') {
					cParens --;
					if (cParens < 0) {
						cerr << "WARNING: unexpected ')' in '" << str << "'" << endl;
					}
				}

			}

			if (cParens != 0) {
				cerr << "WARNING: unbalanced '(' in '" << str << "'" << endl;
			}

			trimString(value);

			if (value.length () > 0) {
				params.push_back (value);
			}
		}

		if (attrib.length () > 0 && params.size () > 0) {
			attributes[attrib] = params;
		}
	}
}

}
