#ifndef SVG_COLOR_H
#define SVG_COLOR_H

#include <string>
#include <map>
#include <iostream>
#include <libxml/tree.h>



namespace SWF {

class SVGColor {
	public:
		SVGColor();
		SVGColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255);

		void initColors();

		void setColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255);
		void setAlpha(unsigned char _a);
		void setAlpha(double _a);
		bool parse(std::string &color);
		bool parse(const char *color);

		void writeXML(xmlNodePtr parent, double opacity);

	private:
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
};

}

#endif
