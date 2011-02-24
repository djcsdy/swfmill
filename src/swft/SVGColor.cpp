#include "SVGColor.h"
#include "SVGColors.h"

#define TMP_STRLEN 0xFF

using namespace std;

namespace SWF {

SVGColor::SVGColor() {
	r = 0;
	g = 0;
	b = 0;
	a = 0;
}

SVGColor::SVGColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) {
	setColor(_r, _g, _b, _a);
}

void SVGColor::setColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

void SVGColor::setAlpha(unsigned char _a) {
	a = _a;
}

void SVGColor::setAlpha(double _a) {
	a = (unsigned char)(_a * 255.0);
}

bool SVGColor::parse(string &color) {
	if(color == "none" || color == "") {
		return false;
	} else {
		if(color[0] == '#') {
			if(color.length() == 4) {
				int c[3];
				
				for(int i = 0; i < 3; i++)
					sscanf(color.substr(1 + i, 1).c_str(), "%x", &c[i]);
				
				r = (c[0] << 4) + c[0];
				g = (c[1] << 4) + c[1];
				b = (c[2] << 4) + c[2];
				a = 255;
			} else if (color.length() == 7) {
				int c[3];
				
				for(int i = 0; i < 3; i++)
					sscanf(color.substr(1 + i*2, 2).c_str(), "%x", &c[i]);
				
				r = c[0];
				g = c[1];
				b = c[2];
				a = 255;
			} else {
				cerr << "WARNING: can't parse color" << endl;
				return false;
			}
		} else if(color.substr(0, 3) == "rgb") {
			double pR, pG, pB;
			if(sscanf(color.c_str(), "rgb(%lf%%,%lf%%,%lf%%)", &pR, &pG, &pB) == 3) {
				r = (unsigned char)(pR / 100.0 * 255.0);
				g = (unsigned char)(pG / 100.0 * 255.0);
				b = (unsigned char)(pB / 100.0 * 255.0);
				a = 255;
			} else if(sscanf(color.c_str(), "rgb(%hhu,%hhu,%hhu)", &r, &g, &b) == 3) {
				a = 255;
				return true;
			} else {
				cerr << "WARNING: can't parse rgb color '" << color << "' " << endl;	
				return false;
			}
		} else {
			for(int i = 0; i < SVG_COLOR_COUNT; i++) {
				if(svgColors[i].name == color) {
					r = svgColors[i].r;
					g = svgColors[i].g;
					b = svgColors[i].b;
					a = 255;
					return true;
				}
			}
			cerr << "WARNING: color name not found (" << color << ")" << endl;	
			return false;
		}
	}
	return true;
}

bool SVGColor::parse(const char *color) {
	string tmp(color);
	return parse(tmp);
}

void SVGColor::writeXML(xmlNodePtr parent, double opacity) {
	xmlNodePtr node;
	char tmp[TMP_STRLEN];

	node = xmlNewChild(parent, NULL, (const xmlChar *)"Color", NULL);
	snprintf(tmp, TMP_STRLEN, "%i", r);
	xmlSetProp(node, (const xmlChar *)"red", (const xmlChar *)&tmp);
	snprintf(tmp, TMP_STRLEN, "%i", g);
	xmlSetProp(node, (const xmlChar *)"green", (const xmlChar *)&tmp);
	snprintf(tmp, TMP_STRLEN, "%i", b);
	xmlSetProp(node, (const xmlChar *)"blue", (const xmlChar *)&tmp);
	snprintf(tmp, TMP_STRLEN, "%i", (int) (a * opacity));
	xmlSetProp(node, (const xmlChar *)"alpha", (const xmlChar *)&tmp);
}

}
