#include "SWFShapeMaker.h"
#include <stdarg.h>
#include "SWFShapeItem.h"
#include "SWFItem.h"
#include "SWF.h"

#define TMP_STRLEN 0xFF

namespace SWF {

// TODO assure bits is representable in 4 bits!
ShapeMaker::ShapeMaker( List<ShapeItem>* e, double fx, double fy, double ofsx, double ofsy ) {
	edges = e;
	factorx = fx;
	factory = fy;
	offsetx = ofsx;
	offsety = ofsy;
	diffx = diffy = 0.0;
	lastx = lasty = 0.0;
	smoothx = smoothy = 0.0;
	minx = miny = maxx = maxy = 0.0;
	have_first = false;
	
	fillStyle0 = lineStyle = fillStyle1 = -1;

	roundReset();
}

void ShapeMaker::doSetup( double _x, double _y, bool hasMoveTo, int _fillStyle0, int _fillStyle1, int _lineStyle ) {
	// append shapesetup (whithout styles, this is glyph only for now)
	ShapeSetup *setup = new ShapeSetup;


	if( _fillStyle0 != -1 ) {
		setup->setfillStyle0( _fillStyle0 );
		setup->sethasFillStyle0( 1 );
	}
	if( _fillStyle1 != -1 ) {
		setup->setfillStyle1( _fillStyle1 );
		setup->sethasFillStyle1( 1 );
	}
	if( _lineStyle != -1 ) {
		setup->setlineStyle( _lineStyle );
		setup->sethasLineStyle( 1 );
	}
	
	if( hasMoveTo ) {
		roundReset();
		int x = roundX(factorx * ( _x ) );
		int y = roundY(factory * ( _y ) );

		diffx = diffy = 0;

		setup->setxybits( SWFMaxBitsNeeded( true, 2, x, y ) );
	
		setup->sethasMoveTo( 1 );
		setup->setx( x );
		setup->sety( y );
		
		minmax( x, y );
	
		lastx = _x; lasty = _y;
		lastsetupx = _x; lastsetupy = _y;
	}
			
	edges->append( setup );
	
//	fprintf(stderr,"setup %i/%i\n", x, y );
}

void ShapeMaker::lineToR( double _x, double _y ) {
	int x = roundX(factorx * ( _x ) );
	int y = roundY(factory * ( _y ) );

	diffx += x; diffy += y;
	
	SWF::LineTo *segment = new SWF::LineTo;
	segment->setType(1);
	//segment->setbits( maxBitsNeeded( true, 2, x, y ) );
	segment->setx( x );
	segment->sety( y );
	edges->append( segment );
	minmax( x+(lastx*factorx), y+(lasty*factory) );

	lastx += _x; lasty += _y;
}

void ShapeMaker::curveToR( double _cx, double _cy, double ax, double ay ) {
	int cx = roundX(factorx * ( _cx ) );
	int cy = roundY(factory * ( _cy ) );
	int x = roundX(factorx * ( ax - _cx ) );
	int y = roundY(factory * ( ay - _cy) );

	smoothx = lastx + _cx;
	smoothy = lasty + _cy;

	diffx += cx + x; diffy += cy + y;
	
	
	CurveTo *segment = new CurveTo;
	segment->setType(2);
	segment->setbits( SWFMaxBitsNeeded( true, 4, x, y, cx, cy ) );
	segment->setx1( cx );
	segment->sety1( cy );
	segment->setx2( x );
	segment->sety2( y );
	edges->append( segment );
	minmax( x+cx+(lastx*factorx), y+cy+(lasty*factory) );

	lastx += ax; lasty += ay;

	smoothx = ax - _cx;
	smoothy = ay - _cy;
}

void ShapeMaker::curveTo( double cx, double cy, double ax, double ay ) {
	curveToR( cx - lastx, cy - lasty, ax - lastx, ay - lasty );
}

void ShapeMaker::smoothCurveToR( double ax, double ay ) {
	curveToR( smoothx, smoothy, ax, ay );

}

void ShapeMaker::smoothCurveTo( double ax, double ay ) {
	curveTo( lastx + smoothx, lasty + smoothy, ax, ay );
}

void ShapeMaker::cubicTo( double x1, double y1, double x2, double y2, double ax, double ay ) {
	Point a(lastx,lasty);
	Point b(x1,y1);
	Point c(x2,y2);
	Point d(ax,ay);

	Bezier cubic (a, b, c, d);

	double t0, t1;
	int cInflections = cubic.computeInflections (t0, t1);

	if (cInflections == 0)
	{
		cubicTo(cubic);
	}
	else if (cInflections == 1)
	{
		Bezier cubic0 = cubic.split (t0);
		cubicTo (cubic0);
		cubicTo (cubic);
	}
	else
	{
		Bezier cubic0 = cubic.split (t0);
		cubicTo (cubic0);

		Bezier cubic1 = cubic.split (1 - (1 - t1) / (1 - t0));
		cubicTo (cubic1);

		cubicTo (cubic);
	}

	lastx = ax; lasty = ay;
	smoothx = ax - x2;
	smoothy = ay - y2;
}

void ShapeMaker::cubicTo (const Bezier &cubic)
{
	const double sqrt3 = 1.7320508075688772935274463415059;
	const double precision = 600 * 18 / (60 * factorx * sqrt3);

	// distance between control points of quadratic approximations of either end
	double D01 = (cubic.p1 - (cubic.c1 - cubic.c0) * 3 - cubic.p0).magnitude () / 2;

	double tMax3 = precision / D01;
	if (tMax3 >= 1)
	{
		curveTo (cubic.quadraticCtrl(), cubic.p1);
		return;
	}

	Bezier end(cubic);

	if (tMax3 >= 0.5 * 0.5 * 0.5)
	{
		Bezier start (end.split (0.5));
		curveTo (start.quadraticCtrl (), start.p1);
	}
	else
	{
		double tMax = pow (tMax3, 1.0/3.0);
		Bezier start (end.split (tMax));
		Bezier middle (end.split (1 - tMax / (1 - tMax)));

		curveTo (start.quadraticCtrl (), start.p1);
		cubicTo (middle);
	}

	curveTo (end.quadraticCtrl (), end.p1);
}

void ShapeMaker::cubicToR( double x1, double y1, double x2, double y2, double ax, double ay ) {
	cubicTo(lastx + x1, lasty + y1,
	        lastx + x2, lasty + y2,
	        lastx + ax, lasty + ay);
}

void ShapeMaker::smoothCubicTo( double x2, double y2, double ax, double ay ) {
	cubicTo( lastx + smoothx, lasty + smoothy, x2, y2, ax, ay );

}

void ShapeMaker::smoothCubicToR( double x2, double y2, double ax, double ay ) {
	cubicToR( smoothx, smoothy, x2, y2, ax, ay );
}

void ShapeMaker::close(bool stroke) {
	// diffx/diffy captures rounding errors. they can accumulate a bit! FIXME
	
	if( diffx || diffy ) {
		if(!stroke) {
			doSetup( 0, 0, false, -1, -1, 0 );
		}
		
		// closing line
		LineTo *segment = new LineTo;
		segment->setType(1);
		//segment->setbits( maxBitsNeeded( true, 2, x, y ) );
		segment->setx( (int)-diffx );
		segment->sety( (int)-diffy );
		edges->append( segment );

		if(!stroke) {
			doSetup( 0, 0, false, -1, -1, lineStyle );
		}
		
		diffx = diffy = 0;
		if(stroke) {
			lastx = lastsetupx; lasty = lastsetupy;
		}
	}
}

void ShapeMaker::finish() {
	ListItem<ShapeItem> *last = edges->last ();
	if (last && last->data()->isEnd ()) {
		return;
	}

	// end shape
	ShapeSetup *setup = new ShapeSetup;
	edges->append( setup );
}

void ShapeMaker::setupR( double x, double y ) {
	x += lastx; y += lasty;
	doSetup( x, y, true, fillStyle0, fillStyle1, lineStyle );
}

void ShapeMaker::setup( double x, double y ) {
	x += offsetx; y += offsety;
	doSetup( x, y, true, fillStyle0, fillStyle1, lineStyle );
}

void ShapeMaker::lineTo( double x, double y ) {
	x += offsetx; y += offsety;
	lineToR( x - lastx, y - lasty );
}

void ShapeMaker::rect( double x, double y, double w, double h, double rx, double ry ) {
	if(rx > 0 || ry > 0) {
		setup(x + rx, y);
		lineTo(x + w - rx, y);
		arcTo(rx, ry, 0, false, true, x + w, y + ry);
		lineTo(x + w, y + h - ry);
		arcTo(rx, ry, 0, false, true, x + w - rx, y + h);
		lineTo(x + rx, y + h);
		arcTo(rx, ry, 0, false, true, x, y + h - ry);
		lineTo(x, y + ry);
		arcTo(rx, ry, 0, false, true, x + rx, y);
		close();
	} else {
		setup(x, y);
		lineToR(0, h);
		lineToR(w, 0);
		lineToR(0, -h);
		lineToR(-w, 0);
		close();
	}
}

#define ELLIPSE_SEGMENTS 8
#define ELLIPSE_ANGLE ( M_PI * 2 / ELLIPSE_SEGMENTS )

void ShapeMaker::ellipseSegment( double cx, double cy, double rx, double ry, double phi, double theta, double dTheta) {
	double a1 = theta + dTheta / 2;
	double a2 = theta + dTheta;
	double f = cos(dTheta / 2);

	Point c0(cos(a1) * rx / f, sin(a1) * ry / f);
	Point c1(cos(a2) * rx, sin(a2) * ry);
	c0.rotate(phi);
	c1.rotate(phi);

	curveTo(cx + c0.x, cy + c0.y, cx + c1.x, cy + c1.y);
}

void ShapeMaker::ellipse( double cx, double cy, double rx, double ry ) {
	setup(cx + rx, cy);
	for(int i = 0; i < ELLIPSE_SEGMENTS; i++) {
		ellipseSegment(cx, cy, rx, ry, 0, ELLIPSE_ANGLE * i, ELLIPSE_ANGLE);
	}
	close();
}

void ShapeMaker::arcTo( double rx, double ry, double rotation, bool largeArcFlag, bool sweepFlag, double x, double y ) {
	double a, f, lambda, theta, dTheta;

	a = rotation / 180 * M_PI;

	Point A(lastx, lasty);
	Point B(x, y);
			
	Point P = (A - B) / 2;
	P.rotate(-a);

	lambda = pow(P.x, 2) / pow(rx, 2) + pow(P.y, 2) / pow(ry, 2);
	if(lambda > 1) {
		rx *= sqrt(lambda);
		ry *= sqrt(lambda);
	}

	f = (pow(rx, 2)*pow(ry, 2)-pow(rx, 2)*pow(P.y, 2)-pow(ry, 2)*pow(P.x, 2))/(pow(rx, 2)*pow(P.y, 2)+pow(ry, 2)*pow(P.x, 2));
	if(f < 0) {
		f = 0;
	} else {
		f = sqrt(f);
	}
	if(largeArcFlag == sweepFlag) f *= -1;

	Point C_(rx / ry * P.y, -ry / rx * P.x);
	C_ = C_ * f;

	Point C = C_;
	C.rotate(a);
	C = C + (A + B) / 2; 
			
	theta = atan2((P.y-C_.y)/ry, (P.x-C_.x)/rx);
	dTheta = atan2((-P.y-C_.y)/ry, (-P.x-C_.x)/rx) - theta;

	if(sweepFlag && dTheta < 0)
		dTheta += 2 * M_PI;

	if(!sweepFlag && dTheta > 0)
		dTheta -= 2 * M_PI;
	
	double dThetaAbs = (dTheta < 0 ? -dTheta : dTheta);
	int segments = (int)ceil(dThetaAbs / ELLIPSE_ANGLE);

	for(int i = 0; i < segments; i++) {
		ellipseSegment(C.x, C.y, rx, ry, a, dTheta / segments * i + theta, dTheta / segments);
	}
}

void ShapeMaker::arcToR( double rx, double ry, double rotation, bool largeArcFlag, bool sweepFlag, double x, double y ) {
	arcTo(rx, ry, rotation, largeArcFlag, sweepFlag, lastx + x, lasty + y);
}

void ShapeMaker::boundsWriteXML( xmlNodePtr parent, double border ) {
	char tmp[TMP_STRLEN];
	xmlNodePtr node;

	if(border >= 0) {
		node = xmlNewChild(parent, NULL, (const xmlChar *)"bounds", NULL); 
	} else {
		node = xmlNewChild(parent, NULL, (const xmlChar *)"strokeBounds", NULL); 
		border = 0;
	}

	node = xmlNewChild(node, NULL, (const xmlChar *)"Rectangle", NULL);
	snprintf(tmp, TMP_STRLEN, "%f", minx - border * 20);
	xmlSetProp(node, (const xmlChar *)"left", (const xmlChar *)&tmp);
	snprintf(tmp, TMP_STRLEN,"%f", miny - border * 20);
	xmlSetProp(node, (const xmlChar *)"top", (const xmlChar *)&tmp);
	snprintf(tmp,TMP_STRLEN,"%f", maxx + border * 20);
	xmlSetProp(node, (const xmlChar *)"right", (const xmlChar *)&tmp);
	snprintf(tmp,TMP_STRLEN,"%f", maxy + border * 20);
	xmlSetProp(node, (const xmlChar *)"bottom", (const xmlChar *)&tmp);
}

}
