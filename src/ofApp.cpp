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
	sendTouchMessages();
	updateArray();
	interpolate();
}

void ofApp::interpolate(){
	for(int i = 0 ;i< kMaxTouch;i++ ){
		touches[i].interpolate();
	}
}

void ofApp::sendTouchMessages(){
	for(int i = 0 ;i< kMaxTouch;i++ ){
		if(touches[i].status){
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
	list.addFloat((float)touch.status);
	pd.sendMessage("fromOF", "touches", list);
}

void ofApp::updateStatistics(){

	float x = 0, y = 0;
	int count = 0;
	for(int i = 0 ; i < kMaxTouch; i++){
		if(touches[i].status){
			if(i > 0){
				ofPoint pivot = touches[0].getInterpolatedPoint();
				ofPoint target = touches[i].getInterpolatedPoint();
				ofPoint local = ofPoint(target.x - pivot.x, target.y - pivot.y);
				touches[i].distance = pivot.distance(target);
				touches[i].angle = atan2f(local.y, local.x) / M_PI * 180.0;
				//ofLog() << touches[i].angle;
			}
			x += touches[i].getPoint().x;
			y += touches[i].getPoint().y;
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
	drawInterpolations();
	drawDistances();
	drawNetwork();
	drawCentroid();
	drawWaveform();
}

void ofApp::drawTouches(){
	ofSetColor(ofColor::white);
	ofSetLineWidth(kNormalLineWidth);
	ofNoFill();

	for(int i = 0;i < kMaxTouch; i++){
		auto touch = touches[i];
		if (touch.status){
			drawCaptions(i, touch.getPoint());
			ofDrawCircle(touch.getPoint(), kCircleSize);

		}
	}
}

void ofApp::drawInterpolations(){
	ofFill();

	for(int i = 0;i < kMaxTouch; i++){
		auto touch = touches[i];
		if(touch.status){
			if(touch.interpolation){
				ofSetColor(ofColor::white);
			}else{
				ofSetColor(ofColor::gray);
			}
			ofDrawCircle(touch.getInterpolatedPoint(), kSmallCircleSize);
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
			ofPoint origin = touches[0].getInterpolatedPoint();
			ofPoint middle((touch.getInterpolatedPoint().x + origin.x)/2.0,  (touch.getInterpolatedPoint().y + origin.y)/2.0 );
			ofDrawLine(origin, touch.getInterpolatedPoint());
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
			ofTranslate(touches[0].getInterpolatedPoint());
			ofRotateZ(touches[i].angle);
			ofScale(touches[i].distance, 100, 1);
			polyline.draw();
			ofPopMatrix();
		}
	}
}

void ofApp::drawCentroid(){
	ofSetColor(ofColor::gray);
	ofNoFill();
	ofDrawCircle(centroid, kCentroidSize);
}

void ofApp::drawNetwork(){
	ofSetColor(ofColor::gray);
	ofSetLineWidth(kThinLineWidth);

	ofPolyline polyline;
	for(auto touch : touches){
		if(touch.status){
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
	touches[id].status = true;
	touches[id].setPoint(ofPoint(x,y));
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){
	if(id >= kMaxTouch) return;
	touches[id].setPoint(ofPoint(x,y));
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){
	if(id >=kMaxTouch ) return;
	touches[id].status = false;
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
