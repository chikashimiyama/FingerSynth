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

	for(int i = 0; i< kMaxTouch; i++){
		touches.push_back(Touch());
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

}

void ofApp::print(const std::string& message) {
	ofLog() << message;
}

//--------------------------------------------------------------
void ofApp::update(){
	updateStatistics();

	sendTouchMessage();
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
	ofSetColor(ofColor::white);

	ofSetLineWidth(kNormalLineWidth);

	for(int i = 0;i < kMaxTouch; i++){
		auto touch = touches[i];
		if (touch.status){
			ofNoFill();
			drawCaptions(i, touch.point);
			ofDrawCircle(touch.point, kCircleSize);
		}
	}
}

void ofApp::drawCaptions(int index, ofPoint center){

	myfont.drawString(tags[index].number + " : " + tags[index].function, center.x -kCaptionOffset, center.y-kCaptionOffset);
	myfont.drawString(tags[index].xmap, center.x + kCaptionOffset, center.y);
	myfont.drawString(tags[index].ymap, center.x, center.y + kCaptionOffset);
}

void ofApp::drawDistances(){
	ofSetColor(ofColor::white);
	ofSetLineWidth(kStemLineWidth);
	for(int i = 1;i < kMaxTouch; i++){
		auto touch = touches[i];
		if (touch.status){
			ofPoint middle((touch.point.x + touches[0].point.x)/2.0,  (touch.point.y + touches[0].point.y)/2.0 );
			ofDrawLine(touches[0].point, touch.point);
			myfont.drawString(tags[i].distanceMap, middle.x +kCaptionMargin, middle.y+kCaptionMargin);
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
			ofScale(touches[i].distance, 100, 1);
			polyline.draw();
			ofPopMatrix();
		}
	}
}

void ofApp::drawCentroid(){
	ofSetColor(ofColor::gray);
	ofDrawCircle(centroid, kCentroidSize);
}

void ofApp::drawNetwork(){
	ofSetColor(ofColor::gray);
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

void ofApp::sendTouchMessage(){
	List list;
	ofPoint size = ofGetWindowSize();

	for(int i = 0 ;i< kMaxTouch;i++ ){
		auto touch = touches[i];
		list.addFloat(i);
		list.addFloat((float)touch.point.x / size.x);
		list.addFloat((float)touch.point.y / size.y);
		list.addFloat((float)touch.angle);
		list.addFloat((float)touch.distance);
		list.addFloat((float)touch.status);
		pd.sendMessage("fromOF", "touches", list);
	}
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
