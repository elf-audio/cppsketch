#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0);
	ofSetCircleResolution(66);
	ofSetWindowShape(400, 400);
	particles.resize(10000);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
}

//--------------------------------------------------------------
void ofApp::update(){
	for(auto &p : particles) {
		p.update();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofVboMesh mesh;
	
	for(auto &p : particles) {
		mesh.addVertex(p.pos);
		//p.draw();
	}
	mesh.draw(OF_MESH_POINTS);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
}
