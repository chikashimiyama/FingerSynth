#include "ofApp.h"
#include "const.h"
#include <string>

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(0,0,10,255);
	ofSoundStreamSetup(2,0, this, kSampleRate, kBlockSize, 3);
	if(!pd.init(2, 0, 44100, kTicksPerBuffer, false)) {
		OF_EXIT_APP(1);
	}

	for(int i = 0; i< kMaxTouch; i++){
		touches.push_back(Touch());
	}

	tags.push_back(Tag("1","carrier","freq","waveshape", "-","-", ofColor::lightBlue));
	tags.push_back(Tag("2","modulartor1","freq","waveshape", "-","index", ofColor::orange));
	tags.push_back(Tag("3","modulartor2","freq","waveshape", "-","index", ofColor::lightGreen));
	tags.push_back(Tag("4","noise-gen","freq","waveshape", "-","-", ofColor::lightCyan));
	tags.push_back(Tag("5","pulser","freq","waveshape", "-","-", ofColor::lightPink));


	pd.subscribe("toOF");
	pd.addReceiver(*this);

	pd.start();
	Patch patch = pd.openPatch("pd/synth.pd");

	ofSetCircleResolution(kCircleResolution);
	ofSetLineWidth(kNormalLineWidth);
	myfont.load("verdana.ttf", kNormalFontSize);

}

void ofApp::print(const std::string& message) {
	ofLog() << message;
}

//--------------------------------------------------------------
void ofApp::update(){
	int previousNumTouches = numTouches;
	updateStatistics();
	List list;
	if(previousNumTouches == 0 && numTouches > 0){
		pd.sendMessage("fromOF", "start", list );
	}else if(previousNumTouches > 0 && numTouches == 0){
		pd.sendMessage("fromOF", "stop", list );

	}

	updateArray();
}

void ofApp::updateStatistics(){

	float x = 0, y = 0;
	int count = 0;
	for(int i = 0 ; i < kMaxTouch; i++){
		if(touches[i].status){
			if(i > 0){
				ofPoint pivot = touches[0].point;
				ofPoint target = touches[i].point;
				ofPoint local = ofPoint(target.x - pivot.x, target.y - pivot.y);
				touches[i].distance = pivot.distance(target);
				touches[i].angle = atan2f(local.y, local.x) / M_PI * 180.0;
				//ofLog() << touches[i].angle;
			}
			x += touches[i].point.x;
			y += touches[i].point.y;
			count++;
		}
	}
	x /= (float)count;
	y /= (float)count;
	centroid = ofPoint(x,y);
	numTouches = count;

}

void ofApp::updateArray(){
	pd.readArray("scope", scopeArray);
	pd.readArray("mod1Scope", mod1ScopeArray);
	pd.readArray("mod2Scope", mod2ScopeArray);
}

//--------------------------------------------------------------
void ofApp::draw(){
	drawTouches();
	drawDistances();
	drawNetwork();
	drawCentroid();
	drawWaveform();
}

void ofApp::drawTouches(){
	ofSetColor(255, 255, 255, 255);

	ofSetLineWidth(kNormalLineWidth);

	for(int i = 0;i < kMaxTouch; i++){
		auto touch = touches[i];
		if (touch.status){
			ofNoFill();
			myfont.drawString(tags[i].number, touch.point.x, touch.point.y-kCaptionOffset);
			myfont.drawString(tags[i].function,touch.point.x, touch.point.y+kCaptionOffset);
			ofDrawCircle(touch.point, kCircleSize);
		}
	}
}

void ofApp::drawDistances(){
	ofSetColor(255, 255, 255, 255);
	ofSetLineWidth(kStemLineWidth);
	for(int i = 1;i < kMaxTouch; i++){
		auto touch = touches[i];
		if (touch.status){
			ofDrawLine(touches[0].point, touch.point);
		}
	}
}

void ofApp::drawWaveform(){


	ofSetLineWidth(kWaveformLineWidth);

	for(int i = 1; i < 3; i++){
		if(touches[i].status){
			ofSetColor(tags[i].color);
			std::vector<ofPoint> linePoints(kArraySize);
			for(size_t i = 0; i < mod1ScopeArray.size()-1; ++i) {
				linePoints.push_back(ofPoint(i * kWaveformScale, mod1ScopeArray[i]));
			}
			ofPolyline polyline(linePoints);

			ofPushMatrix();
			ofTranslate(touches[0].point);
			ofRotateZ(touches[i].angle);
			ofScale(touches[i].distance, 200, 1);
			polyline.draw();
			ofPopMatrix();
		}
	}
}

void ofApp::drawCentroid(){
	ofSetColor(125, 125, 125, 255);
	ofDrawCircle(centroid, kCentroidSize);
}

void ofApp::drawNetwork(){
	ofSetColor(125, 125, 125, 255);
	ofSetLineWidth(kThinLineWidth);

	ofPolyline polyline;
	for(auto touch : touches){
		if(touch.status){
			ofDrawLine(centroid, touch.point);
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
	touches[id].status = true;
	touches[id].point = ofPoint(x,y);
	sendTouchMessage(x,y,id, 1);
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){
	if(id >= kMaxTouch) return;
	touches[id].point = ofPoint(x,y);
	sendTouchMessage(x,y,id, 1);
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){
	if(id >=kMaxTouch ) return;
	touches[id].status = false;
	sendTouchMessage(x,y,id, 0);
}

void ofApp::sendTouchMessage(int x, int y, int id, int status){
	List list;
	ofPoint size = ofGetWindowSize();
	list.addFloat(id);
	list.addFloat((float)x / size.x);
	list.addFloat((float)y / size.y);
	list.addFloat((float)status);
	pd.sendMessage("fromOF", "touches", list);
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
