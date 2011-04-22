#ifndef GEOM_H
#define GEOM_H

#include <libxml/tree.h>
#include <cmath>

using namespace std;

namespace SWF {

class Matrix {
	public:
		Matrix();
		Matrix(double v11, double v12, double v13,
		       double v21, double v22, double v23,
		       double v31, double v32, double v33);

		void identity();
		void translate(double tx, double ty);
		void scale(double sx, double sy);
		void scale(double s);
		void rotate(double a);
		void skewX(double a);
		void skewY(double a);

		double getValue(int row, int col);
		
		void setXMLProps(xmlNodePtr node);

		Matrix& operator*=(const Matrix &m);
		Matrix operator*(const Matrix &m);

	private:
		double values[3][3];
};

class Rect {
	public:
		Rect(double _left = 0, double _top = 0, double _right = 0, double _bottom = 0) { 
			left = _left;
			top = _top;
			right = _right;
			bottom = _bottom;
		}

		double left;
		double right;
		double top;
		double bottom;
};

class Point {
	public:
		double x, y;
		
		Point( double _x=0, double _y=0 ) {
			x = _x; y = _y;
		}

		Point( const Point& p ) {
			x=p.x; y=p.y;
		}

		// rotate point around origin
		void rotate( const double phi ) { 
			double _x = cos(phi) * x + -sin(phi) * y;
			double _y = sin(phi) * x + cos(phi) * y;

			x = _x; y = _y;
		}

		const Point operator -( const Point& p ) const {
			Point p2(x-p.x, y-p.y);
			return p2;
		}

		const Point operator +( const Point& p ) const {
			Point p2(x+p.x, y+p.y);
			return p2;
		}

		const Point operator /( const double s ) const {
			Point p2(x / s, y / s);
			return p2;
		}

		const Point operator *( const double s ) const {
			Point p2(x * s, y * s);
			return p2;
		}

		Point operator =( const Point& p ) {
			x=p.x; y=p.y;
			return *this;
		}

		double magnitude () const {
			return sqrt (x * x + y * y);
		}

		double distanceTo (const Point& p) const {
			return (*this - p).magnitude ();
		}
};

}

#endif
