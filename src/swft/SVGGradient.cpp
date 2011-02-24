#include "SVGGradient.h"
#include "SVGTransformParser.h"
#include <cstring>

#define TMP_STRLEN 0xFF

using namespace std;

namespace SWF {

/* SVGGradient */

SVGGradient::SVGGradient() {
	spreadMethod = PAD;
}

void SVGGradient::parse(xmlNodePtr node) {
	attribs.parseNode(node);

	const char *tmp = attribs["gradientUnits"];
	if(tmp) {
		if(!strcmp(tmp, "userSpaceOnUse")) {
			userSpace = true;
		} else {
			userSpace = false;
		}
	} else {
		userSpace = false;
	}

	parseGradient();
	parseSpreadMethod();
	parseTransform();
	parseStops(node);
}

void SVGGradient::parseStops(xmlNodePtr parent) {
	for(xmlNodePtr node = parent->children; node; node = node->next) {
		if(!xmlStrcmp(node->name, (const xmlChar *)"stop"))
			parseStop(node);
	}
}

void SVGGradient::parseStop(xmlNodePtr node) {
	SVGGradientStop stop;
	AttributeParser stopAttribs;
	
	stopAttribs.parseNode(node);

	double offset = stopAttribs.getDouble("offset");
	
	const char *tmp = stopAttribs["stop-color"];
	if(tmp) {
		stop.setColor(tmp);
	}

	stop.setAlpha(stopAttribs.getDouble("stop-opacity", 1));

	stops[offset] = stop;
}

void SVGGradient::parseTransform() {
	const char *tmp = attribs["gradientTransform"];
	if(tmp) {
		TransformParser parser;
		parser.parse(tmp);
		transform = parser.getMatrix();
	}
	
}

void SVGGradient::parseSpreadMethod() {
	const char *tmp = attribs["spreadMethod"];
	if(tmp) {
		if(!strcmp(tmp, "pad")) {
			spreadMethod = PAD;
		} else if(!strcmp(tmp, "reflect")) {
			spreadMethod = REFLECT;
		} else if(!strcmp(tmp, "repeat")) {
			spreadMethod = REPEAT;
		} else {
			spreadMethod = PAD;
		}
	}
}

void SVGGradient::writeCommonXML(xmlNodePtr parentNode, Matrix& m, bool hasModes, double opacity) {
	char tmp[TMP_STRLEN];
	xmlNodePtr node;
	
	if(hasModes) {
		xmlSetProp(parentNode, (const xmlChar *)"interpolationMode", (const xmlChar *)"0");
		
		snprintf(tmp, TMP_STRLEN, "%i", spreadMethod);
		xmlSetProp(parentNode, (const xmlChar *)"spreadMode", (const xmlChar *)&tmp);
	} else {
		xmlSetProp(parentNode, (const xmlChar *)"reserved", (const xmlChar *)"0");
	}
	
	node = xmlNewChild(parentNode, NULL, (const xmlChar *)"matrix", NULL);
	node = xmlNewChild(node, NULL, (const xmlChar *)"Transform", NULL);
	m.setXMLProps(node);

	node = xmlNewChild(parentNode, NULL, (const xmlChar *)"gradientColors", NULL);

	for(map<double, SVGGradientStop>::iterator i = stops.begin(); i != stops.end(); i++) {
		(*i).second.writeXML(node, (*i).first, opacity);
	}
}

/* SVGLinearGradient */

void SVGLinearGradient::parseGradient() {
	x1 = attribs.getDouble("x1", 0);
	y1 = attribs.getDouble("y1", 0);
	x2 = attribs.getDouble("x2", 1);
	y2 = attribs.getDouble("y2", 0);
}

void SVGLinearGradient::writeXML(xmlNodePtr node, Rect& bounds, bool hadModes, double opacity) {
	double w = bounds.right - bounds.left;
	double h = bounds.bottom - bounds.top;

	Matrix m;

	if(userSpace) {
		double lx = x2 - x1;
		double ly = y2 - y1;

		double dx = x2 - x1;
		double dy = y2 - y1;
		double d = sqrt(dx * dx + dy * dy);
		double a = atan2(dy, dx);

		m *= transform;
		m.translate((x1 + x2) / 2 * 20, (y1 + y2) / 2 * 20);
		m.rotate(a);
		m.scale(d * 20 / 32768);
	} else {
		double sx, sy;
		
		double _x1 = bounds.left + x1 * w;
		double _y1 = bounds.top + y1 * h;
		double _x2 = bounds.left + x2 * w;
		double _y2 = bounds.top + y2 * h;

		if(x1 != x2) {
			sx = (_x2 - _x1) / 32768.0;
		} else {
			sx = 1;
		}

		if(y1 != y2) {
			sy = (_y2 - _y1) / 32768.0;
		} else {
			sy = 1;
		}

		double dx = x2 - x1;
		double dy = y2 - y1;
		double d = sqrt(dx * dx + dy * dy);
		double a = atan2(dy, dx);

		m.translate((_x1 + _x2) / 2, (_y1 + _y2) / 2);
		m.scale(sx, sy);
		m.rotate(a);
		m.scale(d);
	}

	xmlNodePtr topNode = xmlNewChild(node, NULL, (const xmlChar *)"LinearGradient", NULL);
	writeCommonXML(topNode, m, hadModes, opacity);
}

/* SVGRadialGradient */

void SVGRadialGradient::parseGradient() {
	cx = attribs.getDouble("cx", .5);
	cy = attribs.getDouble("cy", .5);
	r = attribs.getDouble("r", .5);
	
	if(attribs["fx"] || attribs["fy"]) {
		hasFocalPoint = true;
		fx = attribs.getDouble("fx", cx);
		fy = attribs.getDouble("fy", cy);
		
		if(fx == cx && fy == cy) {
			hasFocalPoint = false;
		}
	} else {
		hasFocalPoint = false;
	}
}

void SVGRadialGradient::writeXML(xmlNodePtr node, Rect& bounds, bool hasModes, double opacity) {
	Matrix m;
	
	double w = bounds.right - bounds.left;
	double h = bounds.bottom - bounds.top;
	double shift = 0;
	
	if(userSpace) {
		m *= transform;
		m.translate(cx * 20, cy * 20);
		
		if(hasFocalPoint) {
			double dx = fx - cx;
			double dy = fy - cy;
			m.rotate(atan2(dy, dx));
			shift = sqrt(pow(dx, 2) + pow(dy, 2)) / r;
		}
				
		m.scale(r * 20 / 16348.0, r * 20 / 16384.0);
	} else {
		double _cx = bounds.left + cx * w;
		double _cy = bounds.top + cy * h;
		
		m.translate(_cx, _cy);
		
		m.scale(r * w / 16348.0, r * h / 16384.0);
		
		if(hasFocalPoint) {
			double _fx = bounds.left + fx * w;
			double _fy = bounds.top + fy * h;
			
			double dx = _fx - _cx;
			double dy = _fy - _cy;
			
			m.rotate(atan2(dy, dx));
			shift = sqrt(pow(dx, 2) + pow(dy, 2)) / (r * sqrt(pow(w, 2) + pow(h, 2)) / sqrt(2));
		}		
	}
	
	xmlNodePtr topNode;
	if(hasFocalPoint) {
		topNode = xmlNewChild(node, NULL, (const xmlChar *)"ShiftedRadialGradient", NULL);
		
		char tmp[TMP_STRLEN];	
		snprintf(tmp, TMP_STRLEN, "%f", shift);
		xmlSetProp(topNode, (const xmlChar *)"shift", (const xmlChar *)tmp);
	} else {
		topNode = xmlNewChild(node, NULL, (const xmlChar *)"RadialGradient", NULL);
	}
	writeCommonXML(topNode, m, hasModes, opacity);
}

/* SVGGradientStop */

void SVGGradientStop::writeXML(xmlNodePtr node, double offset, double opacity) {
	char tmp[TMP_STRLEN];

	node = xmlNewChild(node, NULL, (const xmlChar *)"GradientItem", NULL);
	snprintf(tmp, TMP_STRLEN, "%i", (int)(offset * 255));
	xmlSetProp(node, (const xmlChar *)"position", (const xmlChar *)&tmp);
	node = xmlNewChild(node, NULL, (const xmlChar *)"color", NULL);
	color.writeXML(node, opacity);
}

}
