
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

	void interpolate();
	void setStatus(const bool &newStatus);
	void setPoint(const ofPoint &newPoint);

	bool getStatus();
	ofPoint getPoint();
	ofPoint getInterpolatedPoint();

protected:
	bool status;
	ofPoint point;
	ofPoint interpolatedPoint;
};



