#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"

#include "ofxAccelerometer.h"
#include "const.h"
#include "Touch.h"
#include "ofxPd.h"

using namespace pd;

class ofApp : public ofxAndroidApp,  public PdReceiver{

protected:

		std::vector<Touch> touches;
		std::vector<float> scopeArray;
		ofxPd pd;
		ofTrueTypeFont myfont;
		ofVec3f accel;

		ofMesh background;

		ofPoint centroid;
		int numTouches;
		float stretch;

		void drawTouches();
		void drawCentroid();
		void drawNetwork();
		void drawBackgroundWaveform();
		void drawTilt();
		void sendTouchMessage(int index);
		void process();
		void sendGeneralMessages();

		void updateArray();
		void updateBackground();
		void print(const std::string& message);

		void setupBackground();
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

};
