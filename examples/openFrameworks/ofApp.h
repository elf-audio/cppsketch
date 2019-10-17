#pragma once

#include "ofMain.h"

class Particle {
public:
	glm::vec3 pos;
	float speed = 1;
	Particle() {
		pos = {ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()), 0};
		speed = ofRandom(0.5, 2.f);
	}
	void update() {
		pos += (glm::vec3(ofNoise(pos.x*0.01*speed + ofGetElapsedTimef()*0.4, pos.y*0.001), ofNoise(pos.x*0.0015, pos.y*0.0024), 0) - glm::vec3(0.5, 0.5, 0.f)) * 4.f * speed;
		
		if(pos.x<0) pos.x += ofGetWidth();
		else if(pos.x>ofGetWidth()) pos.x -= ofGetWidth();
			
		if(pos.y<0) pos.y += ofGetHeight();
		else if(pos.y>ofGetHeight()) pos.y -= ofGetHeight();
		
	}
};

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	vector<Particle> particles;
};
