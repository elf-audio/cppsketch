#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	auto includePath = std::filesystem::path(__FILE__).parent_path();
	auto pathToLiveFile = includePath / "MyApp.h";
	
	printf("%s\n", pathToLiveFile.c_str());
	
	reloader.reloaded = [this](ofBaseApp *app) {
		liveApp = app;
		liveApp->setup();
	};

	reloader.init(pathToLiveFile.string(), {includePath.string()});
}

//--------------------------------------------------------------
void ofApp::update(){
	reloader.checkForChanges();
//	if(liveApp) liveApp->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
//	if(liveApp) liveApp->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//	if(liveApp) liveApp->keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
//	if(liveApp) liveApp->keyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
//	if(liveApp) liveApp->mouseMoved(x,y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
//	if(liveApp) liveApp->mouseDragged(x,y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
//	if(liveApp) liveApp->mousePressed(x,y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
//	if(liveApp) liveApp->mouseReleased(x,y, button);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
//	if(liveApp) liveApp->mouseEntered(x,y);
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
//	if(liveApp) liveApp->mouseExited(x,y);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
//	if(liveApp) liveApp->windowResized(w, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
//	if(liveApp) liveApp->gotMessage(msg);
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
//	if(liveApp) liveApp->dragEvent(dragInfo);
}
