
#pragma once
#include "const.h"
#include <ofMain.h>

enum class TouchStatus{
	OFF, INTERPOLATED, MATCHED, RELEASE
};


class Touch {
public:

	Touch():path(ofPath()), distance(0), angle(0), status(TouchStatus::OFF), point(ofPoint(0,0)),interpolatedPoint(ofPoint(0,0)){
		path.circle(point, kCircleSize);
	}
	ofPath path;
	float distance;
	float angle;

	void interpolate(const ofPoint &goal);
	void setStatus(const TouchStatus &newStatus);
	void setPoint(const ofPoint &newPoint);
	void setInterpolatedPoint(const ofPoint &newPoint);
	TouchStatus getStatus();
	ofPoint getPoint();
	ofPoint getInterpolatedPoint();

protected:
	TouchStatus status;
	ofPoint point;
	ofPoint interpolatedPoint;
};



