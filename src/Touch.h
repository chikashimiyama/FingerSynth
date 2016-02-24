
#pragma once
#include "const.h"
#include "ofxPd.h"
#include "ofxAnimatableFloat.h"

using namespace std;

enum class TouchStatus{
	OFF =0, INTERPOLATED =1, MATCHED =2, RELEASE =3
};

class Touch {
protected:
	int index;
	string number;
	string function;
	string xmap;
	string ymap;
	string angleMap;
	string distanceMap;
	ofColor color;
	ofPath path;
	float distance;
	float angle;
	TouchStatus status;
	ofPoint point;
	ofPoint interpolatedPoint;
	ofPoint middle;
	ofxAnimatableFloat rippleSize;
	const ofTrueTypeFont &font;
	std::vector<float> scopeArray;

	void interpolate(const ofPoint &goal, const ofVec3f &force);

	void drawTouchCircle();
	void drawInterpolatedCircle();
	void drawXYLines();
	void drawCaptions();
	void drawArc();
	void drawDistance(const ofPoint &pivot);
	void drawWaveform(const ofPoint &pivot);
	void drawNetwork(const ofPoint &centroid);
	void drawRipple();

public:
	Touch(const int &index, const string &number, const string &function, const string &xmap, const string &ymap,
			const string &angleMap, const string &distanceMap, const ofColor &color, const ofTrueTypeFont &font):
	index(index),
	number(number),
	function(function),
	xmap(xmap),
	ymap(ymap),
	angleMap(angleMap),
	distanceMap(distanceMap),
	color(color),
	path(ofPath()),
	distance(0),
	angle(0),
	status(TouchStatus::OFF),
	point(ofPoint(0,0)),
	interpolatedPoint(ofPoint(0,0)),
	font(font)
	{
		path.circle(point, kCircleSize);
		rippleSize.reset(0.0);
		rippleSize.setRepeatType(PLAY_ONCE);
		rippleSize.setCurve(EASE_OUT);
	}


	void process(const ofPoint &goal, ofxPd &pd, const ofVec3f &force);
	void setStatus(const TouchStatus &newStatus){status = newStatus;}
	void setPoint(const ofPoint &newPoint);
	void setAngle(const float& newAngle){angle = newAngle;}
	void setDistance(const float &newDistance){distance = newDistance;}
	void setInterpolatedPoint(const ofPoint &newPoint){interpolatedPoint = newPoint;}
	void rippleIn();
	void rippleOut();
	void draw(const ofPoint &pivot, const ofPoint &centroid);
	void sendMessage(ofxPd &pd);

	TouchStatus getStatus(){return status;}
	ofPoint getPoint(){return point;}
	ofPoint getInterpolatedPoint(){return interpolatedPoint;}
	float getAngle(){return angle;}
	float getDistance(){return distance;}
	vector<float> &getScopeArray(){return scopeArray;}
};



