#pragma once

#include "ofMain.h"

class MyApp : public ofBaseApp {
public:
	void setup() {
		ofBackground(0);
		ofSetCircleResolution(66);
	}
	
	void draw() {
		ofSetColor(255, 255, 0);
		ofDrawCircle(100, 100, 100);
		ofDrawRectangle(100, 100, 100, 100);
	}
	
	void mouseMoved(int x, int y) {
		
	}
	
	void mouseDragged(int x, int y, int button) {
		
	}
	
	void mousePressed(int x, int y, int button) {
		
	}
	
	void mouseReleased(int x, int y, int button) {
		
	}
};
