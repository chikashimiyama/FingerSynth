#include "Touch.h"
#include "ofxPd.h"

void Touch::process(const ofPoint &goal, ofxPd &pd){
	interpolate(goal);
	sendMessage(pd);
}

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

void Touch::sendMessage(ofxPd &pd){
	pd::List list;
	ofPoint size = ofGetWindowSize();
	list.addFloat(index);
	list.addFloat(point.x / size.x);
	list.addFloat(point.y / size.y);
	list.addFloat(angle);
	list.addFloat(distance);
	list.addFloat((float)status);
	pd.sendMessage("fromOF", "touches", list);
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

void Touch::rippleIn(){
	rippleSize.reset(1.0);
}

void Touch::rippleOut(){

}

void Touch::draw(const ofPoint &pivot, const ofPoint &centroid){
	if (status == TouchStatus::MATCHED || status == TouchStatus::INTERPOLATED){
		drawFingerIndex();
		drawTouchCircle();
		drawInterpolatedCircle();
		drawXYLines();
		drawCaptions();
		drawArc();
		drawNetwork(centroid);
		if(number != "1"){
			drawDistance(pivot);
			drawWaveform(pivot);
		}
	}
}

void Touch::drawArc(){
	ofNoFill();
	ofSetColor(ofColor::gray);
	ofPolyline arc;
	arc.arc(interpolatedPoint, kArcRadius, kArcRadius, angle+180,180, true, 80);
	arc.draw();
}

void Touch::drawFingerIndex(){
	ofSetColor(ofColor::white);
	font.drawString(number, point.x, point.y - kCaptionOffset * 2);
}

void Touch::drawTouchCircle(){
	ofNoFill();
	ofSetLineWidth(kNormalLineWidth);
	ofDrawCircle(point, kCircleSize);
}

void Touch::drawInterpolatedCircle(){
	ofSetLineWidth(kThinLineWidth);
	ofFill();
	if(status == TouchStatus::OFF){
		return;
	}else if(status == TouchStatus::INTERPOLATED){
		ofSetColor(ofColor::white);
		ofDrawLine(point, interpolatedPoint);
	}else{
		ofSetColor(ofColor::gray);
	}
	ofDrawCircle(interpolatedPoint, kSmallCircleSize);
}

void Touch::drawDistance(const ofPoint& pivot){
	ofSetLineWidth(kStemLineWidth);
	if (status != TouchStatus::OFF){

		ofPoint middle((interpolatedPoint.x + pivot.x)/2.0, (interpolatedPoint.y + pivot.y)/2.0 );
		ofSetColor(ofColor::white);
		ofDrawLine(pivot, interpolatedPoint);
		ofSetColor(ofColor::gray);
		font.drawString(distanceMap + "\n" + ofToString(distance), middle.x +kCaptionMargin, middle.y+kCaptionMargin);
	}
}

void Touch::drawXYLines(){
	ofSetColor(ofColor::darkGray);
	ofSetLineWidth(kThinLineWidth);
	ofDrawLine(0, interpolatedPoint.y ,ofGetWidth(), interpolatedPoint.y);
	ofDrawLine(interpolatedPoint.x, 0 ,interpolatedPoint.x, ofGetHeight());
}

void Touch::drawWaveform(const ofPoint &pivot){
	if(status != TouchStatus::OFF){
		ofSetLineWidth(kWaveformLineWidth);
		std::vector<ofPoint> linePoints(kArraySize);
		ofSetColor(color);

		ofPolyline polyline;
		for(int i = 0; i < kArraySize; i++) {
			polyline.addVertex(ofPoint(i * kWaveformScale , scopeArray[i]));
		}
		ofPushMatrix();
		ofTranslate(pivot);
		ofRotateZ(angle);
		ofScale(distance, 100, 1);
		polyline.draw();
		ofPopMatrix();
	}
}

void Touch::drawNetwork(const ofPoint &centroid){
	ofSetColor(ofColor::gray);
	ofSetLineWidth(kThinLineWidth);

	ofPolyline polyline;
	if(status != TouchStatus::OFF){
		ofDrawLine(centroid, interpolatedPoint);
	}
}

void Touch::drawCaptions(){
	ofSetColor(ofColor::gray);
	font.drawString(function, interpolatedPoint.x -kCaptionOffset, interpolatedPoint.y-kCaptionOffset);
	font.drawString(xmap +"\n" + ofToString(interpolatedPoint.x), interpolatedPoint.x + kCaptionOffset, interpolatedPoint.y);
	font.drawString(ymap +"\n" + ofToString(interpolatedPoint.y), interpolatedPoint.x, interpolatedPoint.y + kCaptionOffset);
}
