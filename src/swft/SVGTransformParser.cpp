#include "SVGTransformParser.h"

using namespace std;

namespace SWF {

void TransformParser::printWarning(const string& transformType) {
	cerr << "WARNING: wrong number of parameters in " << transformType << " transformation" << endl;
}

void TransformParser::handleData(const string& transformType, const vector<string>& params) {
	size_t cnt = params.size();
	Matrix m;

	double p[cnt];	
	for(int i = 0; i < cnt; i++) {
		p[i] = atof(params[i].c_str());
	}

	if(transformType == "matrix") {
		if(cnt == 6) {
			Matrix t = Matrix(p[0], p[2], p[4] * 20,
 			                  p[1], p[3], p[5] * 20,
			                     0,    0,         1);
			transformMatrix *= t;
		} else {
			printWarning(transformType);
		}
	} else if(transformType == "translate") {
		if(cnt == 1) {
			transformMatrix.translate(p[0] * 20, 0);
		} else if (cnt == 2) {
			transformMatrix.translate(p[0] * 20, p[1] * 20);
		} else {
			printWarning(transformType);
		}
	} else if(transformType == "scale") {
		if(cnt == 1) {
			transformMatrix.scale(p[0]);
		} else if (cnt == 2) {
			transformMatrix.scale(p[0], p[1]);
		} else {
			printWarning(transformType);
		}
	} else if(transformType == "rotate") {
		if(cnt == 1) {
			transformMatrix.rotate(p[0] * M_PI / 180.0);
		} else if(cnt == 3) {
			transformMatrix.translate(p[1] * 20, p[2] * 20);
			transformMatrix.rotate(p[0] * M_PI / 180.0);
			transformMatrix.translate(-p[1] * 20, -p[2] * 20);
		} else {
			printWarning(transformType);
		}
	} else if(transformType == "skewX") {
		if(cnt == 1) {
			transformMatrix.skewX(p[0] * M_PI / 180.0);
		} else {
			printWarning(transformType);
		}
	} else if(transformType == "skewY") {
		if(cnt == 1) {
			transformMatrix.skewY(p[0] * M_PI / 180.0);
		} else {
			printWarning(transformType);
		}
	} else {
		cerr << "WARNING: unknown SVG transformation (" << transformType << ")" << endl;
	}
}

}
