#include "SVGStyle.h"
#include "SVGAttributeParser.h"
#include <cstring>

#define TMP_STRLEN 0xFF

using namespace std;

namespace SWF {

SVGStyle::SVGStyle() {
	_hasFill = false;
	_hasFillGradient = false;
	_hasStroke = false;
	_hasStrokeGradient = false;
	_hasStyle = false;

	_hasLineCap = false;
	lineCap = CAP_BUTT;

	_hasLineJoin = false;
	lineJoin = JOIN_MITER;
	miterLimit = 4;

	strokeWidth = 1;
}

void SVGStyle::parseNode(xmlNodePtr node, map<string, SVGGradient*> &gradients) {
	AttributeParser parser;
	parser.parseNode(node);

	map<string, string> &attributes = parser.getAttributes();
	for(map<string, string>::iterator iter = attributes.begin(); iter != attributes.end(); iter++) {
		const string &attribute = (*iter).first;
		const string &valueStr = (*iter).second;
		const char *value = valueStr.c_str();
		
		if(attribute == "stroke") {
			SVGGradient *gradient = getGradient(valueStr, gradients);
			if(gradient) {
					strokeGradient = gradient;
					_hasStrokeGradient = true;
					_hasStroke = true;
			} else {
				setStrokeColor(value);
			}
		} else if(attribute == "stroke-width") {
			setStrokeWidth(atof(value));
		} else if(attribute == "stroke-opacity") {
			setStrokeAlpha(atof(value));
		} else if(attribute == "stroke-linecap") {
			setLineCap(value);
		} else if(attribute == "stroke-linejoin") {
			setLineJoin(value);
		} else if(attribute == "stroke-miterlimit") {
			setMiterLimit(atof(value));
		} else if(attribute == "fill") {
			SVGGradient *gradient = getGradient(valueStr, gradients);
			if(gradient) {
					fillGradient = gradient;
					_hasFillGradient = true;
					_hasFill = true;
			} else {
				setFillColor(value);
			}
		} else if(attribute == "fill-opacity") {
			setFillAlpha(atof(value));
		}
	}
}

SVGGradient *SVGStyle::getGradient(const string &str, map<string, SVGGradient*> &gradients) {
	if(str.substr(0, 4) == "url(" && str.substr(str.length() - 1) == ")") {
		map<string, SVGGradient*>::iterator i;
		i = gradients.find(str.substr(5, str.length() - 6));
		if(i != gradients.end()) {
			return (*i).second;
		} else {
			return NULL;
		}
	} else {
		return NULL;
	}
}

void SVGStyle::setLineCap(const char *cap) {
	if(!strcmp(cap, "butt")) {
		lineCap = CAP_BUTT;
		_hasLineCap = true;
		_hasStyle = true;
	} else if(!strcmp(cap, "round")) {
		lineCap = CAP_ROUND;
		_hasLineCap = true;
		_hasStyle = true;
	} else if(!strcmp(cap, "square")) {
		lineCap = CAP_SQUARE;
		_hasLineCap = true;
		_hasStyle = true;
	}
}

void SVGStyle::setLineJoin(const char *join) {
	if(!strcmp(join, "miter")) {
		lineJoin = JOIN_MITER;
		_hasLineJoin = true;
		_hasStyle = true;
	} else if(!strcmp(join, "round")) {
		lineJoin = JOIN_ROUND;
		_hasLineJoin = true;
		_hasStyle = true;
	} else if(!strcmp(join, "bevel")) {
		lineJoin = JOIN_BEVEL;
		_hasLineJoin = true;
		_hasStyle = true;
	}
}

void SVGStyle::writeXML(xmlNodePtr parent, double movieVersion) {
	xmlNodePtr node, styleNode;
	char tmp[TMP_STRLEN];

	styleNode = xmlNewChild(parent, NULL, (const xmlChar *)"StyleList", NULL);

	if(_hasFill) {
		node = xmlNewChild(styleNode, NULL, (const xmlChar *)"fillStyles", NULL);
		if(_hasFillGradient) {
			fillGradient->writeXML(node, bounds, (movieVersion >= 8));
		} else {
			node = xmlNewChild(node, NULL, (const xmlChar *)"Solid", NULL);
			node = xmlNewChild(node, NULL, (const xmlChar *)"color", NULL);
			fill.writeXML(node);
		}
	}

	if(_hasStroke) {
		node = xmlNewChild(styleNode, NULL, (const xmlChar *)"lineStyles", NULL);

		if(movieVersion >= 8) {
			node = xmlNewChild(node, NULL, (const xmlChar *)"LineStyle", NULL);
			snprintf(tmp,TMP_STRLEN,"%f", strokeWidth * 20);
			xmlSetProp(node, (const xmlChar *)"width", (const xmlChar *)&tmp);
			snprintf(tmp, TMP_STRLEN, "%i", lineCap);
			xmlSetProp(node, (const xmlChar *)"startCapStyle", (const xmlChar *)&tmp);
			snprintf(tmp, TMP_STRLEN, "%i", lineJoin);
			xmlSetProp(node, (const xmlChar *)"jointStyle", (const xmlChar *)&tmp);
			xmlSetProp(node, (const xmlChar *)"hasFill", (const xmlChar *)(_hasStrokeGradient ? "1" : "0"));
			xmlSetProp(node, (const xmlChar *)"noHScale", (const xmlChar *)"0");
			xmlSetProp(node, (const xmlChar *)"noVScale", (const xmlChar *)"0");
			xmlSetProp(node, (const xmlChar *)"pixelHinting", (const xmlChar *)"0");
			xmlSetProp(node, (const xmlChar *)"reserved", (const xmlChar *)"0");
			xmlSetProp(node, (const xmlChar *)"noClose", (const xmlChar *)"0");
			snprintf(tmp, TMP_STRLEN, "%i", lineCap);
			xmlSetProp(node, (const xmlChar *)"endCapStyle", (const xmlChar *)&tmp);

			if(lineJoin == JOIN_MITER) {
				snprintf(tmp, TMP_STRLEN, "%f", miterLimit);
				xmlSetProp(node, (const xmlChar *)"miterLimitFactor", (const xmlChar *)&tmp);
			}
			
			if(_hasStrokeGradient) {
				node = xmlNewChild(node, NULL, (const xmlChar *)"fillStyles", NULL);
				strokeGradient->writeXML(node, bounds, (movieVersion >= 8));
			} else {
				node = xmlNewChild(node, NULL, (const xmlChar *)"fillColor", NULL);
				stroke.writeXML(node);
			}
		} else {
			node = xmlNewChild(node, NULL, (const xmlChar *)"LineStyle", NULL);
			snprintf(tmp,TMP_STRLEN,"%f", strokeWidth * 20);
			xmlSetProp(node, (const xmlChar *)"width", (const xmlChar *)&tmp);
			node = xmlNewChild(node, NULL, (const xmlChar *)"color", NULL);
			stroke.writeXML(node);

			if(_hasLineCap || _hasLineJoin) {
				cerr << "WARNING: some svg features aren't supported before swf version 8" << endl;
			}
		}
	}
}

}
