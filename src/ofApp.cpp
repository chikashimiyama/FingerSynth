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
	for(int i = 0; i < kMaxTouch; i++){
		std::string index = ofToString(i+1);
		touches.push_back(Touch(index));
	}


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
}

void ofApp::updateStatistics(){

	float x = 0, y = 0;
	int i = 0;
	for(auto touch : touches){
		if(touch.status){
			x += touch.point.x;
			y += touch.point.y;
			i++;
		}
	}
	x /= (float)i;
	y /= (float)i;
	centroid = ofPoint(x,y);
	numTouches = i;
}
//--------------------------------------------------------------
void ofApp::draw(){
	drawTouches();
	drawNetwork();
	drawCentroid();

}

void ofApp::drawTouches(){
	for(int i = 0;i < kMaxTouch; i++){
		auto touch = touches[i];
		if (touch.status){
			ofNoFill();
			myfont.drawString(touch.index, touch.point.x, touch.point.y-kCaptionOffset);
			ofDrawCircle(touch.point, kCircleSize);
		}
	}
}


void ofApp::drawCentroid(){
	ofDrawCircle(centroid, kCentroidSize);
}

void ofApp::drawNetwork(){
	ofPolyline polyline;
	ofSetLineWidth(kThinLineWidth);
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
