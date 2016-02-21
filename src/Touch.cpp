#include "Touch.h"

void Touch::interpolate(){
	if(interpolation){
		ofPoint step = ofPoint( (point.x - interpolatedPoint.x) * kInterpolationRatio , (point.y - interpolatedPoint.y) * kInterpolationRatio);
		interpolatedPoint.x += step.x;
		interpolatedPoint.y += step.y;

		if(point.distance(interpolatedPoint) < kNearEnoughThreshold){
			interpolation = false;
			interpolatedPoint = point;
		}
	}
}

void Touch::setStatus(const bool &newStatus){
	status = newStatus;
	interpolatedPoint = point;
}

void Touch::setPoint(const ofPoint &newPoint){
	point = newPoint;
	if(point.distance(interpolatedPoint) >= kNearEnoughThreshold){
		interpolation = true;
	}else{
		interpolatedPoint = point;
		interpolation = false;
	}
}

bool Touch::getStatus(){
	return status;
}

ofPoint Touch::getPoint(){
	return point;
}

ofPoint Touch::getInterpolatedPoint(){
	return interpolatedPoint;
}
