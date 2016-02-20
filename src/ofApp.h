#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxPd.h"

using namespace pd;

class Touch {
public:
	Touch():point(ofPoint(0,0)), path(ofPath()), distance(0), angle(0), status(false){
		path.circle(point, 120);
	}
	ofPoint point;
	ofPath path;
	float distance;
	float angle;
	bool status;
};


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
		std::vector<float> mod1ScopeArray;
		std::vector<float> mod2ScopeArray;
		std::vector<Tag> tags;
		ofPoint centroid;
		int numTouches;

		void drawTouches();
		void drawCentroid();
		void drawDistances();
		void drawNetwork();
		void drawWaveform();

		void sendTouchMessage(int x, int y, int id, int status);
		void updateArray();
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
