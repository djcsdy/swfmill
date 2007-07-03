#include "Geom.h"

#define TMP_STRLEN 0xff

namespace SWF {

Matrix::Matrix() {
	identity();	
}

Matrix::Matrix(double v11, double v12, double v13,
	       double v21, double v22, double v23,
	       double v31, double v32, double v33) {
	
	values[0][0] = v11; values[0][1] = v12; values[0][2] = v13;
	values[1][0] = v21; values[1][1] = v22; values[1][2] = v23;
	values[2][0] = v31; values[2][1] = v32; values[2][2] = v33;
}

void Matrix::identity() {
	values[0][0] = 1; values[0][1] = 0; values[0][2] = 0;
	values[1][0] = 0; values[1][1] = 1; values[1][2] = 0;
	values[2][0] = 0; values[2][1] = 0; values[2][2] = 1;
}

void Matrix::translate(double tx, double ty) {
	Matrix t(1, 0, tx,
	         0, 1, ty,
	         0, 0,  1);
	(*this) *= t;
}

void Matrix::scale(double sx, double sy) {
	Matrix t(sx,  0, 0,
	          0, sy, 0,
	          0,  0, 1);
	(*this) *= t;
}

void Matrix::scale(double s) {
	scale(s, s);
}

void Matrix::rotate(double a) {
	Matrix t(cos(a), -sin(a), 0,
	         sin(a),  cos(a), 0,
	              0,       0, 1);
	(*this) *= t;
}

void Matrix::skewX(double a) {
	Matrix t(1, tan(a), 0,
	         0,      1, 0,
	         0,      0, 1);
	(*this) *= t;
}

void Matrix::skewY(double a) {
	Matrix t(     1, 0, 0,
	         tan(a), 1, 0,
	              0, 0, 1);
	(*this) *= t;
}

Matrix& Matrix::operator*=(const Matrix &m) {
	*this = *this * m;
	return *this;
}

Matrix Matrix::operator*(const Matrix &m) {
	return Matrix(values[0][0] * m.values[0][0] + values[0][1] * m.values[1][0] + values[0][2] * m.values[2][0],
	              values[0][0] * m.values[0][1] + values[0][1] * m.values[1][1] + values[0][2] * m.values[2][1],
	              values[0][0] * m.values[0][2] + values[0][1] * m.values[1][2] + values[0][2] * m.values[2][2],
	              values[1][0] * m.values[0][0] + values[1][1] * m.values[1][0] + values[1][2] * m.values[2][0],
	              values[1][0] * m.values[0][1] + values[1][1] * m.values[1][1] + values[1][2] * m.values[2][1],
	              values[1][0] * m.values[0][2] + values[1][1] * m.values[1][2] + values[1][2] * m.values[2][2],
	              values[2][0] * m.values[0][0] + values[2][1] * m.values[1][0] + values[2][2] * m.values[2][0],
	              values[2][0] * m.values[0][1] + values[2][1] * m.values[1][1] + values[2][2] * m.values[2][1],
	              values[2][0] * m.values[0][2] + values[2][1] * m.values[1][2] + values[2][2] * m.values[2][2]);
}

double Matrix::getValue(int row, int col) {
	if(row >= 0 && row <= 2 && col >=0 && col <= 2) {
		return values[row][col];
	} else {
		return 0;
	}
}

void Matrix::setXMLProps(xmlNodePtr node) {
	char tmp[TMP_STRLEN];
	
	snprintf(tmp,TMP_STRLEN,"%f", values[1][0]);
	xmlSetProp(node, (const xmlChar *)"skewX", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%f", values[0][1]);
	xmlSetProp(node, (const xmlChar *)"skewY", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%f", values[0][0]);
	xmlSetProp(node, (const xmlChar *)"scaleX", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%f", values[1][1]);
	xmlSetProp(node, (const xmlChar *)"scaleY", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%f", values[0][2]);
	xmlSetProp(node, (const xmlChar *)"transX", (const xmlChar *)&tmp );
	snprintf(tmp,TMP_STRLEN,"%f", values[1][2]);
	xmlSetProp(node, (const xmlChar *)"transY", (const xmlChar *)&tmp );
}

}
