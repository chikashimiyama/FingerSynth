#include "Touch.h"
#include "ofxPd.h"

void Touch::process(const ofPoint &goal, ofxPd &pd, const ofPoint &force){
	interpolate(goal, force);
	sendMessage(pd);
	rippleSize.update(kRippleAnimationStep);
}

void Touch::interpolate(const ofPoint &goal, const ofVec3f &force){
	if(status == TouchStatus::INTERPOLATED){
			ofPoint step = ofPoint( (point.x - interpolatedPoint.x) * kInterpolationRatio , (point.y - interpolatedPoint.y) * kInterpolationRatio);
			ofPoint forceFactor = ofPoint(step.x * kForceInfluence * fabs(force.x), step.y * kForceInfluence * fabs(force.y));
			ofPoint forceAffectedStep;
			if((force.x > 0.0 && step.x > 0.0) || (force.x < 0.0 && step.x < 0.0)){
				forceAffectedStep.x = step.x + forceFactor.x;
			}else{
				forceAffectedStep.x = step.x - forceFactor.x;
			}

			if((force.y > 0.0 && step.y > 0.0) || (force.y < 0.0 && step.y < 0.0)){
				forceAffectedStep.y = step.y + forceFactor.y;
			}else{
				forceAffectedStep.y = step.y - forceFactor.y;
			}

			interpolatedPoint.x += forceAffectedStep.x;
			interpolatedPoint.y += forceAffectedStep.y;

			if(point.distance(interpolatedPoint) < kNearEnoughThreshold){
				status = TouchStatus::MATCHED;
				interpolatedPoint = point;
			}
		}else if(status == TouchStatus::RELEASE){
			ofPoint step = ofPoint( (goal.x - interpolatedPoint.x) * kInterpolationRatio , (goal.y - interpolatedPoint.y) * kInterpolationRatio);
			ofPoint forceFactor = ofPoint(step.x * kForceInfluence * fabs(force.x), step.y * kForceInfluence * fabs(force.y));
			ofPoint forceAffectedStep;
			if((force.x > 0.0 && step.x > 0.0) || (force.x < 0.0 && step.x < 0.0)){
				forceAffectedStep.x = step.x + forceFactor.x;
			}else{
				forceAffectedStep.x = step.x - forceFactor.x;
			}

			if((force.y > 0.0 && step.y > 0.0) || (force.y < 0.0 && step.y < 0.0)){
				forceAffectedStep.y = step.y + forceFactor.y;
			}else{
				forceAffectedStep.y = step.y - forceFactor.y;
			}
			interpolatedPoint.x += forceAffectedStep.x;
			interpolatedPoint.y += forceAffectedStep.y;
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
	rippleSize.reset(0.0);
	rippleSize.animateFromTo(kRippleSize, 0.0);
}

void Touch::rippleOut(){
	rippleSize.reset(0.0);
	rippleSize.animateFromTo(0.0,kRippleSize);
}

void Touch::draw(const ofPoint &pivot, const ofPoint &centroid){
	if (status == TouchStatus::OFF)return;
	if (status != TouchStatus::RELEASE){
		drawTouchCircle();
	}
	drawInterpolatedCircle();
	drawXYLines();
	drawCaptions();
	drawArc();
	drawNetwork(centroid);
	drawRipple();
	if(index != 0){
		drawDistance(pivot);
		drawWaveform(pivot);
	}
}

void Touch::drawArc(){
	ofNoFill();
	ofSetColor(ofColor::gray);
	ofPolyline arc;
	if(angle < 0 ){
		arc.arc(interpolatedPoint, kArcRadius, kArcRadius, angle+180,180, true, 80);
	}else{
		arc.arc(interpolatedPoint, kArcRadius, kArcRadius, -180,-180+angle, true, 80);
	}
	arc.draw();
}

void Touch::drawTouchCircle(){
	ofNoFill();
	ofSetColor(ofColor::white);
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
	middle = ofPoint ((interpolatedPoint.x + pivot.x)/2.0, (interpolatedPoint.y + pivot.y)/2.0 );
	ofSetColor(ofColor::white);
	ofDrawLine(pivot, interpolatedPoint);
	ofSetColor(ofColor::gray);
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

void Touch::drawRipple(){
	if(!rippleSize.isAnimating()) return;
	ofNoFill();
	ofSetColor(ofColor::darkGray);
	ofSetLineWidth(kThinLineWidth);
	ofDrawCircle(point, kCircleSize * rippleSize.getCurrentValue());

}

void Touch::drawCaptions(){

	if(status != TouchStatus::RELEASE){
		ofSetColor(ofColor::white);
		font.drawString(number, point.x, point.y - kCaptionOffset * 2);
	}

	ofSetColor(ofColor::gray);
	font.drawString(function, interpolatedPoint.x -kCaptionOffset, interpolatedPoint.y-kCaptionOffset);
	font.drawString(xmap +"\n" + ofToString(interpolatedPoint.x), interpolatedPoint.x + kCaptionOffset, interpolatedPoint.y);
	font.drawString(ymap +"\n" + ofToString(interpolatedPoint.y), interpolatedPoint.x, interpolatedPoint.y + kCaptionOffset);

	if(index != 0 ){
		font.drawString(distanceMap + "\n" + ofToString(distance), middle.x +kCaptionMargin, middle.y+kCaptionMargin);
		font.drawString(angleMap + "\n" + ofToString(abs(angle)), interpolatedPoint.x, interpolatedPoint.y + kArcRadius + kCaptionMargin);
	}
}


