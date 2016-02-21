
#pragma once
#include "const.h"
#include <ofMain.h>

class Touch {
public:
	Touch():path(ofPath()), distance(0), angle(0), interpolation(false), status(false), point(ofPoint(0,0)),interpolatedPoint(ofPoint(0,0)){
		path.circle(point, kCircleSize);
	}
	ofPath path;
	float distance;
	float angle;
	bool interpolation;
	bool status;

	void interpolate();

	void setPoint(const ofPoint &newPoint);
	ofPoint getPoint();
	ofPoint getInterpolatedPoint();

protected:
	ofPoint point;
	ofPoint interpolatedPoint;
};



