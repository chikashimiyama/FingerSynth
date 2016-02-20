#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxPd.h"

using namespace pd;

class Touch {
public:
	Touch(std::string index):point(ofPoint(0,0)), path(ofPath()), index(index), status(false){
		path.circle(point, 120);
	}
	ofPoint point;
	ofPath path;
	const std::string index;
	bool status;
};

class ofApp : public ofxAndroidApp,  public PdReceiver{
	protected:
		std::vector<Touch> touches;
		ofPoint centroid;
		int numTouches;

		void drawTouches();
		void drawCentroid();
		void drawNetwork();

		void sendTouchMessage(int x, int y, int id, int status);
		void print(const std::string& message);
	public:
		
		void setup();
		void update();
		void updateStatistics();
		void draw();
		

		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);

		void touchDown(int x, int y, int id);
		void touchMoved(int x, int y, int id);
		void touchUp(int x, int y, int id);
		void touchDoubleTap(int x, int y, int id);
		void touchCancelled(int x, int y, int id);
		void swipe(ofxAndroidSwipeDir swipeDir, int id);

		void pause();
		void stop();
		void resume();
		void reloadTextures();

		bool backPressed();
		void okPressed();
		void cancelPressed();

		void audioOut(float * output, int bufferSize, int nChannel);
		ofxPd pd;
		ofTrueTypeFont myfont;


};
