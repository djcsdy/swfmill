#ifndef SVG_GRADIENT_H
#define SVG_GRADIENT_H

#include <iostream>
#include <map>
#include <libxml/tree.h>
#include "Geom.h"
#include "SVGColor.h"
#include "SVGAttributeParser.h"
#include "SWFShapeMaker.h"



namespace SWF {

class SVGGradientStop;

class SVGGradient {
	public:
		SVGGradient();
		
		virtual void parse(xmlNodePtr node);
		virtual void writeXML(xmlNodePtr node, Rect &bounds, bool hadModes, double opacity) = 0;
		
		enum SpreadMethod {
			PAD,
			REFLECT,
			REPEAT
		};

	protected:
		virtual void parseGradient() = 0;
		void parseStops(xmlNodePtr parent);
		void parseStop(xmlNodePtr node);
		void parseTransform();
		void parseSpreadMethod();
		void writeCommonXML(xmlNodePtr node, Matrix& m, bool hasModes, double opacity);

		std::map<double, SVGGradientStop> stops;
		Matrix transform;
		AttributeParser attribs;
		SpreadMethod spreadMethod;
		
		bool userSpace;
};

class SVGLinearGradient : public SVGGradient {
	public:
		void writeXML(xmlNodePtr node, Rect &bounds, bool hasModes, double opacity);
	
	protected:
		void parseGradient();
		
		double x1, x2, y1, y2;
};

class SVGRadialGradient : public SVGGradient {
	public:
		void writeXML(xmlNodePtr node, Rect &bounds, bool hasModes, double opacity);
		
	protected:
		void parseGradient();
				
		double cx, cy, fx, fy, r;
		bool hasFocalPoint;
};

class SVGGradientStop {
	public:
		void setColor(const char *c) { color.parse(c); }
		void setAlpha(unsigned char alpha) { color.setAlpha(alpha); }
		void setAlpha(double alpha) { color.setAlpha(alpha); }
		void writeXML(xmlNodePtr node, double offset, double opacity);
		
	private:
		SVGColor color;
};

}

#endif
