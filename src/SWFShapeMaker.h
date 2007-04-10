#ifndef SWF_SHAPEMAKER_H
#define SWF_SHAPEMAKER_H

#include <stdio.h>

namespace SWF {

class ShapeItem;
template <class T> class List;

class Point {
	public:
		double x, y;
		
		Point( double _x=0, double _y=0 ) {
			x = _x; y = _y;
		}
		Point( const Point& p ) {
			x=p.x; y=p.y;
		}
		const Point operator -( const Point& p ) const {
			Point p2(x-p.x, y-p.y);
			return p2;
		}
		Point operator =( const Point& p ) {
			x=p.x; y=p.y;
			return *this;
		}
};

class Bezier {
	public:
		Point p0, p1, p2, p3;
		Bezier() { ; }
		Bezier( const Point& _p0, const Point& _p1, const Point& _p2, const Point& _p3 )
			: p0(_p0), p1(_p1), p2(_p2), p3(_p3) { ; }
		Bezier &set( const Point& _p0, const Point& _p1, const Point& _p2, const Point& _p3 ) {
			p0 = _p0; p1 = _p1; p2 = _p2; p3 = _p3;
			return *this;
		}
};

class ShapeMaker {
	public:
		ShapeMaker( List<ShapeItem>* edges, double fx = 1, double fy = 1, double ofsx = 0, double ofsy = 0 );
	
		void setStyle( int _fillStyle0=-1, int _fillStyle1=-1, int _lineStyle=-1 ) {
			fillStyle0 = _fillStyle0;
			fillStyle1 = _fillStyle1;
			lineStyle = _lineStyle;
		}
	
		void do_setup( double x=0, double y=0 );
		void setup( double x=0, double y=0 );
		void lineTo( double x, double y );
		void curveTo( double cx, double cy, double ax, double ay );
		void cubicTo( double x1, double y1, double x2, double y2, double ax, double ay );
		void close();
		void finish();
	
		void setupR( double x=0, double y=0 );
		void lineToR( double x, double y );
		void curveToR( double cx, double cy, double ax, double ay );
	
		double getLastX() { return lastx; }
		double getLastY() { return lasty; }

		double getMinX() { return minx; }
		double getMinY() { return miny; }
		double getMaxX() { return maxx; }
		double getMaxY() { return maxy; }
		
	protected:
		void cubicToRec( const Point& a, const Point& b, const Point& c, const Point& d, double k, int iteration=0 );
	
		void minmax( double x, double y ) {
			if( !have_first ) {
				have_first = true;
				minx = x; maxx=x; miny=y; maxy=y;
			} else {
				if( x < minx ) minx=x;
				if( y < miny ) miny=y;
				if( x > maxx ) maxx=x;
				if( y > maxy ) maxy=y;
			}
		}
	
		List<ShapeItem>* edges;
		double factorx, factory;
		double offsetx, offsety;
		double lastx, lasty;
		double diffx, diffy;
		double minx, miny, maxx, maxy;
		bool have_first;
	
		int fillStyle0, fillStyle1, lineStyle;
		
		// rounding error accumulation compensation
		double roundx, roundy;
		int roundX( double x ) { return round( x, &roundx ); }
		int roundY( double y ) { return round( y, &roundy ); }
		int round( double v, double *acc ) {
				int r = (int)v;
				*acc += v-(double)r;
				while( *acc >= .5 ) {
					*acc -= 1.0;
					r++;
				}
				while( *acc <= -.5 ) {
					*acc += 1.0;
					r--;
				}
				return r;
			}
		void roundReset() {
			roundx = roundy = 0;
			}
};

	
}
#endif
