#include "swft.h"
#include "SVGPathParser.h"
#include "SVGTransformParser.h"
#include "SVGAttributeParser.h"
#include "SVGPointsParser.h"
#include "SVGStyle.h"
#include <libxml/xpathInternals.h>
#include <libxslt/xsltutils.h>
#include <libxslt/extensions.h>

using namespace SWF;

/*
	Create a complete DefineShape3 element.
	syntax: swft:path( <node> , <shapeid> , <movie-version> )
*/
void swft_path(xmlXPathParserContextPtr ctx, int nargs) {
	xmlChar *styleString, *idString, *pathString;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlNodeSetPtr nodeSet;
	double movieVersion;
	
	if(nargs != 3) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	movieVersion = xmlXPathPopNumber(ctx);
	idString = xmlXPathPopString(ctx);
	nodeSet = xmlXPathPopNodeSet(ctx);
	
	if(nodeSet == NULL || nodeSet->nodeNr != 1) {
		xmlXPathSetTypeError(ctx);
		return;
	}
	
	node = nodeSet->nodeTab[0];
	
	swft_ctx *c = (swft_ctx*)xsltGetExtData(xsltXPathGetTransformContext(ctx), SWFT_NAMESPACE);
	SVGStyle style;
	if(c->styles.size() > 0) {
		style = c->styles.top();
	}
	
	style.parseNode(node, c->gradients);
		
	if(!style.hasStyle()) {
		style.setFillColor("#000");
	}

	Shape shape;	
	int xOffset = 0, yOffset = 0;
	ShapeMaker shaper(shape.getedges(), 20, 20, xOffset, yOffset);
	shaper.setStyle(style.hasFill() ? 1 : -1, -1, style.hasStroke() ? 1 : -1);
	
	AttributeParser attribs;
	attribs.parseNode(node);

	if(!xmlStrcmp(node->name, (const xmlChar *)"path")) {
		const char* pathString = attribs["d"];
		if(pathString) {
			PathParser parser(&shaper);
			parser.parse(pathString);
		}
	} else if(!xmlStrcmp(node->name, (const xmlChar *)"rect")) {
		double w = attribs.getDouble("width");
		double h = attribs.getDouble("height");
		if(w > 0 && h > 0) {
			double rx = attribs.getDouble("rx");
			double ry = attribs.getDouble("ry");

			if(attribs["rx"] == NULL && attribs["ry"] == NULL) {
				rx = ry = 0;
			} else if(attribs["rx"] == NULL && attribs["ry"] != NULL) {
				rx = ry;
			} else if(attribs["rx"] != NULL && attribs["ry"] == NULL) {
				ry = rx;
			}

			if(rx > w / 2) rx = w / 2;
			if(ry > h / 2) ry = h / 2;

			shaper.rect( attribs.getDouble("x"),  attribs.getDouble("y"), w, h, rx, ry);
		}
	} else if(!xmlStrcmp(node->name, (const xmlChar *)"circle")) {
		double r = attribs.getDouble("r");
		if(r > 0) 
			shaper.ellipse(attribs.getDouble("cx"), attribs.getDouble("cy"), r, r);
	} else if(!xmlStrcmp(node->name, (const xmlChar *)"ellipse")) {
		double rx = attribs.getDouble("rx");
		double ry = attribs.getDouble("ry");
		if(rx > 0 && ry > 0)
			shaper.ellipse(attribs.getDouble("cx"), attribs.getDouble("cy"), rx, ry);
	} else if(!xmlStrcmp(node->name, (const xmlChar *)"line")) {
		shaper.setup(attribs.getDouble("x1"), attribs.getDouble("y1"));
		shaper.lineTo(attribs.getDouble("x2"), attribs.getDouble("y2"));
	} else if(!xmlStrcmp(node->name, (const xmlChar *)"polyline") ||
			   !xmlStrcmp(node->name, (const xmlChar *)"polygon")) {
		PointsParser parser;
		parser.parse(attribs["points"]);
		
		if(parser.getPointCount() >= 2) {
			Point point, firstPoint;
			
			firstPoint = parser.getPoint();
			shaper.setup(firstPoint.x, firstPoint.y);
			
			int pointCount = parser.getPointCount();
			for(int i = 0; i < pointCount; i++) {
				point = parser.getPoint();
				shaper.lineTo(point.x, point.y);			
			}
			
			if(!xmlStrcmp(node->name, (const xmlChar *)"polyline")) {
				shaper.close(false);
			} else {
				shaper.close(true);
			}
		}
	}
	
	xmlNodePtr shapeNode, styleNode;

	// make the shape xml
	doc = xmlNewDoc( (const xmlChar *)"1.0");

	if(movieVersion > 7) {
		node = shapeNode = doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"DefineShape5", NULL );
	} else {
		node = shapeNode = doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"DefineShape3", NULL );
	}

	xmlSetProp( node, (const xmlChar*)"objectID", idString );

	// bounds
	shaper.boundsWriteXML(shapeNode, (style.hasStroke() ? style.getStrokeWidth() / 2 : 0));
	
	// stroke bounds
	if(movieVersion > 7)
		shaper.boundsWriteXML(shapeNode);

	// styles
	node = xmlNewChild(shapeNode, NULL, (const xmlChar *)"styles", NULL); 
	style.setBounds(shaper.getBounds());
	style.writeXML(node, movieVersion);

	// the shape itself
	shaper.finish();
	node = xmlNewChild(shapeNode, NULL, (const xmlChar *)"shapes", NULL); 

	Context swfctx;
	shape.writeXML( node, &swfctx );
	
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
	return;
}

void swft_transform( xmlXPathParserContextPtr ctx, int nargs ) {
	if(nargs != 1) {
		xmlXPathSetArityError(ctx);
		return;
	}
	
	xmlChar *transform = xmlXPathPopString(ctx);
	if(xmlXPathCheckError(ctx) || (transform == NULL)) {
		return;
	}

	TransformParser transformParser;
	transformParser.parse((char *)transform);

	xmlDocPtr doc;
	doc = xmlNewDoc( (const xmlChar *)"1.0");
	doc->xmlRootNode = xmlNewDocNode( doc, NULL, (const xmlChar *)"Transform", NULL );
	transformParser.getMatrix().setXMLProps(doc->xmlRootNode);
	valuePush( ctx, xmlXPathNewNodeSet( (xmlNodePtr)doc ) );
}
