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

	myfont.load("frabk.ttf", kNormalFontSize);

	for(int i = 0; i< kMaxTouch; i++){
		touches.push_back(Touch(0, "1","carrier","x:freq","Y:waveshape", "-","-", ofColor::lightBlue, myfont));
		touches.push_back(Touch(1, "2","modulator1","x:freq","y:waveshape", "angle:distortion","distance:depth", ofColor::orange, myfont));
		touches.push_back(Touch(2, "3","modulator2","x:freq","y:waveshape", "angle:distortion","distance:depth", ofColor::lightGreen, myfont));
		touches.push_back(Touch(3, "4","modulator3","x:freq","y:waveshape", "angle:distortion","distance:depth", ofColor::lightCyan, myfont));
		touches.push_back(Touch(4, "5","pulser","x:freq","Y:waveshape", "angle:distortion","distance:depth", ofColor::lightPink, myfont));
	}

	pd.subscribe("toOF");
	pd.addReceiver(*this);

	pd.start();
	Patch patch = pd.openPatch("pd/synth.pd");

	ofSetCircleResolution(kCircleResolution);
	ofSetLineWidth(kNormalLineWidth);

	ofxAccelerometer.setup();
	setupBackground();
}

void ofApp::setupBackground(){
	background.addColor(ofColor::black);
	background.addVertex(ofVec2f(0, 0));
	background.addColor(ofColor::black);
	background.addVertex(ofVec2f(ofGetWidth(), 0));
	background.addColor(ofColor::black);
	background.addVertex(ofVec2f(ofGetWidth(), ofGetHeight()));
	background.addColor(ofColor::black);
	background.addVertex(ofVec2f(0, ofGetHeight()));
	background.addIndex(0);
	background.addIndex(1);
	background.addIndex(2);
	background.addIndex(0);
    background.addIndex(3);
    background.addIndex(2);
}

void ofApp::print(const std::string& message) {
	ofLog() << message;
}

//--------------------------------------------------------------
void ofApp::update(){

	updateStatistics();
	updateArray();
	updateBackground();
	process();
	sendGeneralMessages();

}

void ofApp::updateBackground(){
	float right = (accel.x + 0.6) * 0.3;
	float left = 0.6 - right;
	float top = (accel.y + 0.6) * 0.3;
	float bottom =  0.6 - top;

	float leftTop = left * top;
	float rightTop = right * top;
	float rightBottom = right * bottom;
	float leftBottom = left * bottom;

	background.setColor(0, ofFloatColor(leftTop,leftTop,leftTop,1.0));
	background.setColor(1, ofFloatColor(rightTop,rightTop,rightTop,1.0));
	background.setColor(2, ofFloatColor(rightBottom,rightBottom,rightBottom,1.0));
	background.setColor(3, ofFloatColor(leftBottom,leftBottom,leftBottom,1.0));
}

void ofApp::process(){
	for(int i = 0 ;i< kMaxTouch;i++ ){
		touches[i].process(touches[0].getPoint(), pd);
	}
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
				touches[i].setDistance(pivot.distance(target));
				touches[i].setAngle(atan2f(local.y, local.x) / M_PI * 180.0);
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
	pd.readArray("mod1Scope", touches[1].getScopeArray());
	pd.readArray("mod2Scope", touches[2].getScopeArray());
	pd.readArray("mod3Scope", touches[3].getScopeArray());
	pd.readArray("mod4Scope", touches[4].getScopeArray());
}

//--------------------------------------------------------------
void ofApp::draw(){

	drawTilt();
	if(touches[0].getStatus() == TouchStatus::OFF){
		ofSetColor(ofColor::white);
		myfont.drawString("touch me!", ofGetWidth()/2-100, ofGetHeight()/2-100);
		return;
	}
	drawTouches();
	drawCentroid();
	drawBackgroundWaveform();
}

void ofApp::drawTilt(){
	background.draw();
	ofPoint tiltPoint((accel.x+1.0) * ofGetWidth() / 2.0,  (accel.y+1.0) * ofGetHeight() / 2.0);
	ofNoFill();
	ofSetColor(ofColor::darkGray);
	ofSetLineWidth(kThinLineWidth);
	ofDrawCircle(tiltPoint, kCircleSize);
	ofDrawCircle(tiltPoint, kSmallCircleSize);
	myfont.drawString("H.Tilt:filter " + ofToString(tiltPoint.x), tiltPoint.x + kCaptionOffset*2, tiltPoint.y);
	myfont.drawString("V.Tilt:reverb " + ofToString(tiltPoint.y), tiltPoint.x, tiltPoint.y + kCaptionOffset*2);


}
void ofApp::drawTouches(){
	for(int i = 0;i < kMaxTouch; i++){
		touches[i].draw(touches[0].getInterpolatedPoint(), centroid);
	}
}

void ofApp::drawBackgroundWaveform(){

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

	myfont.drawString(std::string("centroid: ") + ofToString(centroid.x) + "," + ofToString(centroid.y), centroid.x, centroid.y-kCaptionOffset);
	myfont.drawString(std::string("stretch: ") + ofToString(stretch), centroid.x, centroid.y+kCaptionOffset);
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
	touches[id].rippleIn();
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
	touches[id].sendMessage(pd);
	touches[id].rippleOut();
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
