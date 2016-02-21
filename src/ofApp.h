#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxAccelerometer.h"
#include "ofxPd.h"
#include "const.h"
#include "Touch.h"
using namespace pd;

class ofApp : public ofxAndroidApp,  public PdReceiver{

	protected:

	struct Tag{
		std::string number;
		std::string function;
		std::string xmap;
		std::string ymap;
		std::string angleMap;
		std::string distanceMap;
		ofColor color;
		Tag(std::string number,
			std::string function,
			std::string xmap,
			std::string ymap,
			std::string angleMap,
			std::string distanceMap,
			ofColor color):
				number(number), function(function), xmap(xmap), ymap(ymap),angleMap(angleMap),distanceMap(distanceMap),color(color){}
	};

		std::vector<Touch> touches;
		std::vector<float> scopeArray;
		std::vector<std::vector<float>> scopeArrays;


		std::vector<Tag> tags;
		ofPoint centroid;
		int numTouches;
		float stretch;

		void drawTouches();
		void drawInterpolations();
		void drawCaptions(int index, ofPoint point);
		void drawCentroid();
		void drawDistances();
		void drawNetwork();
		void drawWaveform();

		void sendTouchMessage(int index);
		void sendTouchMessages();
		void sendGeneralMessages();

		void updateArray();
		void print(const std::string& message);
	public:
		
		void setup();
		void update();
		void updateStatistics();
		void interpolate();
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
		ofVec3f accel;
};
