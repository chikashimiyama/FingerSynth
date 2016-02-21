#include "Touch.h"

void Touch::interpolate(const ofPoint &goal){
	if(status == TouchStatus::INTERPOLATED){
		ofPoint step = ofPoint( (point.x - interpolatedPoint.x) * kInterpolationRatio , (point.y - interpolatedPoint.y) * kInterpolationRatio);
		interpolatedPoint.x += step.x;
		interpolatedPoint.y += step.y;

		if(point.distance(interpolatedPoint) < kNearEnoughThreshold){
			status = TouchStatus::MATCHED;
			interpolatedPoint = point;
		}
	}else if(status == TouchStatus::RELEASE){
		ofPoint step = ofPoint( (goal.x - interpolatedPoint.x) * kInterpolationRatio , (goal.y - interpolatedPoint.y) * kInterpolationRatio);
		interpolatedPoint.x += step.x;
		interpolatedPoint.y += step.y;
		if(goal.distance(interpolatedPoint) < kNearEnoughThreshold){
			status = TouchStatus::OFF;
			interpolatedPoint = goal;
		}
	}
}

void Touch::setStatus(const TouchStatus &newStatus){
	status = newStatus;
}

void Touch::setPoint(const ofPoint &newPoint){
	point = newPoint;
	if(point.distance(interpolatedPoint) >= kNearEnoughThreshold){
		status = TouchStatus::INTERPOLATED;
	}else{
		setInterpolatedPoint(point);
		status = TouchStatus::MATCHED;
	}
}
void Touch::setInterpolatedPoint(const ofPoint &newPoint){
	interpolatedPoint = newPoint;
}

TouchStatus Touch::getStatus(){
	return status;
}

ofPoint Touch::getPoint(){
	return point;
}

ofPoint Touch::getInterpolatedPoint(){
	return interpolatedPoint;
}
