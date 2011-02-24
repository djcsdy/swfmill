#ifndef SVG_STYLE_H
#define SVG_STYLE_H

#include <iostream>
#include <map>
#include <libxml/tree.h>
#include "SVGColor.h"
#include "SVGGradient.h"
#include "SWFShapeMaker.h"



namespace SWF {

enum SVGLineCap {
	CAP_ROUND,
	CAP_BUTT,
	CAP_SQUARE
};

enum SVGLineJoin {
	JOIN_ROUND,
	JOIN_BEVEL,
	JOIN_MITER
};

class SVGStyle {
	public:
		SVGStyle();
		
		bool setStrokeColor(const char *color)
		{
			_hasStroke = stroke.parse(color);
			_hasStyle = true;
			return _hasStroke;
		}
		void setStrokeWidth(const double width)
		{
			strokeWidth = width;
			_hasStyle = true;
		}
		void setStrokeAlpha(const double alpha) { stroke.setAlpha(alpha); _hasStyle = true; }		
		bool setFillColor(const char *color)
		{
			_hasFill = fill.parse(color);
			_hasStyle = true;
			return _hasFill;
		}
		void setFillAlpha(const double alpha)
		{
			fill.setAlpha(alpha);
			_hasStyle = true;
		}
		void setOpacity(const double opacity)
		{
			_opacity = opacity;
			_hasStyle = true;
		}
		bool setLineCap(const char *cap);
		bool setLineJoin(const char *join);
		void setMiterLimit(const double miter) { miterLimit = miter; _hasStyle = true; }

		double getStrokeWidth() { return strokeWidth; }

		void setBounds(Rect b) { bounds = b; }

		bool hasFill() { return _hasFill; }
		bool hasStroke() { return _hasStroke; }
		bool hasStyle() { return _hasStyle; }

		void parseNode(xmlNodePtr node, std::map<std::string, SVGGradient*> &gradients);
		void writeXML(xmlNodePtr parent, double movieVersion);

	private:
		SVGGradient *getGradient(const std::string &str, std::map<std::string, SVGGradient*> &gradients);
	
		bool _hasFill;
		bool _hasFillGradient;
		SVGColor fill;
		SVGGradient *fillGradient;
		
		bool _hasStroke;
		bool _hasStrokeGradient;
		SVGColor stroke;
		SVGGradient *strokeGradient;

		bool _hasLineCap;
		SVGLineCap lineCap;

		bool _hasLineJoin;
		SVGLineJoin lineJoin;
		double miterLimit;

		bool _hasStyle;

		double strokeWidth;
		double _opacity;

		Rect bounds;
};

}

#endif
