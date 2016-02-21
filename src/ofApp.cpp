#include "ofApp.h"
#include "const.h"
#include <string>

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(ofColor::black);
	ofSoundStreamSetup(2,0, this, kSampleRate, kBlockSize, 3);
	if(!pd.init(2, 0, 44100, kTicksPerBuffer, false)) {
		OF_EXIT_APP(1);
	}

	// allocation
	for(int i = 0; i< kMaxTouch; i++){
		touches.push_back(Touch());
	}
	for(int i = 1; i < kMaxTouch; i++){
		scopeArrays.push_back(std::vector<float>(kArraySize));
	}

	tags.push_back(Tag("1","carrier","X:freq","Y:waveshape", "-","-", ofColor::lightBlue));
	tags.push_back(Tag("2","modulartor1","X:freq","Y:waveshape", "-","Dist:index", ofColor::orange));
	tags.push_back(Tag("3","modulartor2","X:freq","Y:waveshape", "-","Dist:index", ofColor::lightGreen));
	tags.push_back(Tag("4","random-gen","X:freq","Y:waveshape", "-","-", ofColor::lightCyan));
	tags.push_back(Tag("5","pulser","x:freq","Y:waveshape", "-","-", ofColor::lightPink));


	pd.subscribe("toOF");
	pd.addReceiver(*this);

	pd.start();
	Patch patch = pd.openPatch("pd/synth.pd");

	ofSetCircleResolution(kCircleResolution);
	ofSetLineWidth(kNormalLineWidth);
	myfont.load("verdana.ttf", kNormalFontSize);

	ofxAccelerometer.setup();
}

void ofApp::print(const std::string& message) {
	ofLog() << message;
}

//--------------------------------------------------------------
void ofApp::update(){

	updateStatistics();
	updateArray();
	interpolate();
	sendTouchMessages();
	sendGeneralMessages();

}

void ofApp::interpolate(){
	for(int i = 0 ;i< kMaxTouch;i++ ){
		touches[i].interpolate(touches[0].getPoint());
	}
}

void ofApp::sendTouchMessages(){
	for(int i = 0 ;i< kMaxTouch;i++ ){
		if(touches[i].getStatus() != TouchStatus::OFF){
			sendTouchMessage(i);
		}
	}
}

void ofApp::sendTouchMessage(int index){
	ofPoint size = ofGetWindowSize();
	auto touch = touches[index];
	List list;
	list.addFloat(index);
	list.addFloat((float)touch.getPoint().x / size.x);
	list.addFloat((float)touch.getPoint().y / size.y);
	list.addFloat((float)touch.angle);
	list.addFloat((float)touch.distance);
	list.addFloat((float) touch.getStatus());
	pd.sendMessage("fromOF", "touches", list);
}

void ofApp::sendGeneralMessages(){
	List accelList;
	accelList.addFloat(accel[0]);
	accelList.addFloat(accel[1]);
	accelList.addFloat(accel[2]);
	pd.sendMessage("fromOF", "accels", accelList);

	List centroidList;
	centroidList.addFloat(centroid.x);
	centroidList.addFloat(centroid.y);
	pd.sendMessage("fromOF", "centroid", centroidList);

	List stretchList;
	stretchList.addFloat(stretch);
	pd.sendMessage("fromOF", "stretch", stretchList);
}

void ofApp::updateStatistics(){

	float x = 0, y = 0;
	int count = 0;
	for(int i = 0 ; i < kMaxTouch; i++){
		if(touches[i].getStatus() != TouchStatus::OFF){
			if(i > 0){
				ofPoint pivot = touches[0].getInterpolatedPoint();
				ofPoint target = touches[i].getInterpolatedPoint();
				ofPoint local = ofPoint(target.x - pivot.x, target.y - pivot.y);
				touches[i].distance = pivot.distance(target);
				touches[i].angle = atan2f(local.y, local.x) / M_PI * 180.0;
				//ofLog() << touches[i].angle;
			}
			x += touches[i].getInterpolatedPoint().x;
			y += touches[i].getInterpolatedPoint().y;
			count++;
		}
	}
	x /= (float)count;
	y /= (float)count;
	centroid = ofPoint(x,y);
	numTouches = count;
	accel = ofxAccelerometer.getForce().getNormalized();

	stretch = 0.0;
	if( touches[0].getStatus() != TouchStatus::OFF || numTouches >= 2){
		for(int i = 1 ; i < kMaxTouch; i++){
			auto status = touches[i].getStatus();
			if(status != TouchStatus::OFF){
				stretch += touches[0].getInterpolatedPoint().distance(centroid);
			}
		}
		stretch /= numTouches;
	}

	//ofLog() << accel[0] << " " << accel[1] << " " << accel[2];
}

void ofApp::updateArray(){
	pd.readArray("scope", scopeArray);
	pd.readArray("mod1Scope", scopeArrays[0]);
	pd.readArray("mod2Scope", scopeArrays[1]);
	pd.readArray("mod3Scope", scopeArrays[2]);
	pd.readArray("mod4Scope", scopeArrays[3]);

}

//--------------------------------------------------------------
void ofApp::draw(){
	if(touches[0].getStatus() == TouchStatus::OFF){
		ofSetColor(ofColor::white);
		myfont.drawString("touch me!", ofGetWidth()/2-100, ofGetHeight()/2-100);
		return;
	}
	drawTouches();
	drawInterpolations();
	drawDistances();
	drawNetwork();
	drawCentroid();
	drawWaveform();
}

void ofApp::drawTouches(){
	ofNoFill();

	for(int i = 0;i < kMaxTouch; i++){
		auto touch = touches[i];
		if (touch.getStatus() == TouchStatus::MATCHED || touch.getStatus() == TouchStatus::INTERPOLATED){
			ofSetColor(ofColor::white);
			ofPoint touchPoint = touch.getPoint();
			myfont.drawString(tags[i].number, touchPoint.x, touchPoint.y - kCaptionOffset * 2);

			drawCaptions(i, touch.getInterpolatedPoint());
			ofSetLineWidth(kNormalLineWidth);
			ofDrawCircle(touch.getPoint(), kCircleSize);

			ofSetColor(ofColor::darkGray);
			ofPoint interpolatedPoint = touch.getInterpolatedPoint();
			ofSetLineWidth(kThinLineWidth);
			ofDrawLine(0, interpolatedPoint.y ,ofGetWidth(), interpolatedPoint.y);
			ofDrawLine(interpolatedPoint.x, 0 ,interpolatedPoint.x, ofGetHeight());
		}
	}
}

void ofApp::drawInterpolations(){
	ofSetLineWidth(kThinLineWidth);
	ofFill();
	for(int i = 0;i < kMaxTouch; i++){
		auto touch = touches[i];
		TouchStatus status = touch.getStatus();
		if(status == TouchStatus::OFF){
			continue;
		}else if(status == TouchStatus::INTERPOLATED){
			ofSetColor(ofColor::white);
			ofDrawLine(touch.getPoint(), touch.getInterpolatedPoint());
		}else{
			ofSetColor(ofColor::gray);
		}
		ofDrawCircle(touch.getInterpolatedPoint(), kSmallCircleSize);
	}
}


void ofApp::drawCaptions(int index, ofPoint center){

	myfont.drawString(tags[index].function, center.x -kCaptionOffset, center.y-kCaptionOffset);
	myfont.drawString(tags[index].xmap, center.x + kCaptionOffset, center.y);
	myfont.drawString(tags[index].ymap, center.x, center.y + kCaptionOffset);
}

void ofApp::drawDistances(){
	ofSetColor(ofColor::white);
	ofSetLineWidth(kStemLineWidth);
	for(int i = 1;i < kMaxTouch; i++){
		auto touch = touches[i];
		if (touch.getStatus() != TouchStatus::OFF){
			ofPoint origin = touches[0].getInterpolatedPoint();
			ofPoint middle((touch.getInterpolatedPoint().x + origin.x)/2.0,  (touch.getInterpolatedPoint().y + origin.y)/2.0 );
			ofDrawLine(origin, touch.getInterpolatedPoint());
			myfont.drawString(tags[i].distanceMap, middle.x +kCaptionMargin, middle.y+kCaptionMargin);
		}
	}
}

void ofApp::drawWaveform(){

	ofSetLineWidth(kWaveformLineWidth);

	for(int i = 0; i < kMaxTouch-1; i++){
		if(touches[i+1].getStatus() != TouchStatus::OFF){
			std::vector<ofPoint> linePoints(kArraySize);
			for(int j = 0; j < kArraySize; j++) {
				linePoints.push_back(ofPoint(j * kWaveformScale , scopeArrays[i][j]));
			}
			ofPolyline polyline(linePoints);

			ofPushMatrix();
			ofTranslate(touches[0].getInterpolatedPoint());
			ofRotateZ(touches[i+1].angle);
			ofScale(touches[i+1].distance, 100, 1);
			ofSetColor(tags[i+1].color);
			polyline.draw();
			ofPopMatrix();
		}
	}

	std::vector<ofPoint> linePoints(kArraySize);
	float horizontalStep = (float)ofGetWidth() / (float)kArraySize;

	float height = ofGetHeight();
	ofSetColor(ofColor::darkBlue);
	ofPolyline polyline;
	for(int i = 0; i < kArraySize; i++) {
		polyline.addVertex(ofPoint(i * horizontalStep , scopeArray[i] * height / 4.0 + height/2.0 ));
	}
	ofSetLineWidth(kThinLineWidth);
	polyline.draw();

}

void ofApp::drawCentroid(){
	ofSetColor(ofColor::gray);
	ofNoFill();
	ofDrawCircle(centroid, kCentroidSize);
	myfont.drawString(std::string("stretch: ") + ofToString(stretch), centroid.x, centroid.y+kCaptionOffset);
}

void ofApp::drawNetwork(){
	ofSetColor(ofColor::gray);
	ofSetLineWidth(kThinLineWidth);

	ofPolyline polyline;
	for(auto touch : touches){
		if(touch.getStatus() != TouchStatus::OFF){
			ofDrawLine(centroid, touch.getInterpolatedPoint());
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){
	if(id >= kMaxTouch) return;
	if(id == 0){
		touches[id].setPoint(ofPoint(x,y));
		touches[id].setInterpolatedPoint(ofPoint(x,y));
	}else{
		touches[id].setPoint(ofPoint(x,y));
		touches[id].setInterpolatedPoint(touches[0].getInterpolatedPoint());
	}
	touches[id].setStatus(TouchStatus::INTERPOLATED);
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){
	if(id >= kMaxTouch) return;
	touches[id].setPoint(ofPoint(x,y));
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){
	if(id >=kMaxTouch ) return;
	if(id == 0){
		touches[id].setStatus(TouchStatus::OFF);
	}else{
		touches[id].setStatus(TouchStatus::RELEASE);
	}
	sendTouchMessage(id);
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id){


}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id){

}

//--------------------------------------------------------------
void ofApp::pause(){

}

//--------------------------------------------------------------
void ofApp::stop(){

}

//--------------------------------------------------------------
void ofApp::resume(){

}

//--------------------------------------------------------------
void ofApp::reloadTextures(){

}

//--------------------------------------------------------------
bool ofApp::backPressed(){
	return false;
}

//--------------------------------------------------------------
void ofApp::okPressed(){

}

//--------------------------------------------------------------
void ofApp::cancelPressed(){

}

void ofApp::audioOut(float * output, int bufferSize, int nChannels){
	pd.audioOut(output, bufferSize, nChannels);
}
