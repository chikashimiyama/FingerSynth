#include "ofApp.h"
#include "const.h"
#include <string>

//--------------------------------------------------------------
void ofApp::setup(){
	int ticksPerBuffer = 8;
	ofSetBackgroundColor(0,0,0,255);
	ofSoundStreamSetup(2,0, this, 44100, 512, 3);
	if(!pd.init(2, 0, 44100, ticksPerBuffer, false)) {
		OF_EXIT_APP(1);
	}
	for(int i = 0; i < 5; i++){
		std::string index = ofToString(i+1);
		touches.push_back(Touch(index));
	}

	pd.start();
	Patch patch = pd.openPatch("synth.pd");

	ofSetCircleResolution(kCircleResolution);
	ofSetLineWidth(kNormalLineWidth);
	myfont.load("frabk.ttf", kNormalFontSize);

}

//--------------------------------------------------------------
void ofApp::update(){

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
	for(int i = 0;i < 5; i++){
		auto touch = touches[i];
	}
}

void ofApp::drawNetwork(){
	ofPolyline polyline;
	int i = 0;
	float x = 0, y = 0;
	for(auto touch : touches){
		if(touch.status){
			x += touch.point.x;
			y += touch.point.y;
			i++;
		}
	}
	if(i < 2) return;

	x /= (float)i;
	y /= (float)i;
	ofDrawCircle(x, y, kCentroidSize);
	ofSetLineWidth(kThinLineWidth);

	for(auto touch : touches){
		if(touch.status){
			ofDrawLine(ofPoint(x,y), touch.point);
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
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){
	if(id >= kMaxTouch) return;
	touches[id].point = ofPoint(x,y);
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){
	if(id >=kMaxTouch ) return;
	touches[id].status = false;
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
