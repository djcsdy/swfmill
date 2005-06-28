#ifndef SWF_SHAPEMAKER_H
#define SWF_SHAPEMAKER_H

//#include "SWF.h"

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
	
		void setup( int x=0, int y=0, int fillStyle0=-1, int fillStyle1=-1, int lineStyle=-1 );
		void lineTo( int x, int y );
		void curveTo( int cx, int cy, int ax, int ay );
		void cubicTo( int x1, int y1, int x2, int y2, int ax, int ay );
		void close();
		void finish();
	
		void setupR( int x=0, int y=0, int fillStyle0=-1, int fillStyle1=-1, int lineStyle=-1 );
		void lineToR( int x, int y );
		void curveToR( int cx, int cy, int ax, int ay );
	
		int getLastX() { return lastx; }
		int getLastY() { return lasty; }
	
	protected:
		void cubicToRec( const Point& a, const Point& b, const Point& c, const Point& d, double k, int iteration=0 );
	
		List<ShapeItem>* edges;
		double factorx, factory;
		double offsetx, offsety;
		int lastx, lasty;
		int diffx, diffy;
	
		// rounding error accumulation compensation
		double roundx, roundy;
		int roundX( double x ) { return round( x, &roundx ); }
		int roundY( double y ) { return round( y, &roundy ); }
		int round( double v, double *acc ) {
				int r = (int)v;
				*acc += v-(double)r;
				while( *acc >= 1.0 ) {
					*acc -= 1.0;
					r++;
				}
				while( *acc <= 1.0 ) {
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
