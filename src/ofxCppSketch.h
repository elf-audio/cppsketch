#pragma once

#include "ofMain.h"
#include "ReloadableClass.h"

/**
 * To make this work on xcode 11, you need to change a setting in your build settings, called "Dead code stripping" - change it to NO
 */

class ofxCppSketch: public ofBaseApp {
public:
	

	ofxCppSketch(string className, string mainFilePath) : ofBaseApp() {
		this->className = className;
		srcDir = std::filesystem::path(mainFilePath).parent_path();
	}
	
	void setup() override {
		reloader.reloaded = [this](ofBaseApp *app) {
			liveApp = app;
			liveApp->setup();
		};
		
		// the only thing you have to change in settings is "Dead Code Stripping"

		auto pathToLiveFile = srcDir / (className + ".h");
		
		auto includes = getAllIncludes();
		
		// start the auto-reloading
		auto headerToPrecompile = getOfLibPath() / "ofMain.h";
		reloader.init(pathToLiveFile.string(), includes, headerToPrecompile.string());
	}
	
protected:
	
	
	std::filesystem::path getLibsPath() {
		return srcDir.parent_path() / OF_PATH / "libs";

	}
	
	std::filesystem::path getOfLibPath() {
		return getLibsPath() / "openFrameworks";
	}
	
	/**
	 * returns a list of all includes needed to compile the live-coded ofApp
	 */
	vector<string> getAllIncludes() {
		// add the core oF includes
		
		auto libsPathStr = getLibsPath().string();
		
		vector<string> includes = liveCodeUtils::getAllDirectories(getOfLibPath().string());
				
		// add dependencies
		includes.push_back(libsPathStr + "/rtAudio/include");
		includes.push_back(libsPathStr + "/fmodex/include");
		includes.push_back(libsPathStr + "/freetype/include");
		includes.push_back(libsPathStr + "/boost/include");
		
		includes.push_back(libsPathStr + "/cairo/include/cairo");
		includes.push_back(libsPathStr + "/curl/include");
		includes.push_back(libsPathStr + "/json/include");
		includes.push_back(libsPathStr + "/tess2/include");
		
		includes.push_back(libsPathStr + "/glew/include");
		includes.push_back(libsPathStr + "/pugixml/include");
		includes.push_back(libsPathStr + "/glfw/include");
		includes.push_back(libsPathStr + "/uriparser/include");
		
		includes.push_back(libsPathStr + "/glm/include");
		includes.push_back(libsPathStr + "/FreeImage/include");
		includes.push_back(libsPathStr + "/utf8/include");
		
		includes.push_back(srcDir.string());
		return includes;
	}
	
	
	void update() override {
		reloader.checkForChanges();
		if(liveApp) liveApp->update();
	}
	
	void draw() override {
		if(liveApp) liveApp->draw();
	}
	
	void keyPressed(int key) override {
		if(liveApp) liveApp->keyPressed(key);
	}
	
	void keyReleased(int key) override {
		if(liveApp) liveApp->keyReleased(key);
	}
	
	void mouseMoved(int x, int y) override {
		if(liveApp) liveApp->mouseMoved(x,y);
	}
	
	void mouseDragged(int x, int y, int button) override {
		if(liveApp) liveApp->mouseDragged(x,y, button);
	}
	
	void mousePressed(int x, int y, int button) override {
		if(liveApp) liveApp->mousePressed(x,y, button);
	}
	
	void mouseReleased(int x, int y, int button) override {
		if(liveApp) liveApp->mouseReleased(x,y, button);
	}
	
	void mouseEntered(int x, int y) override {
		if(liveApp) liveApp->mouseEntered(x,y);
	}
	
	void mouseExited(int x, int y) override {
		if(liveApp) liveApp->mouseExited(x,y);
	}
	
	void windowResized(int w, int h) override {
		if(liveApp) liveApp->windowResized(w, h);
	}
	
	void dragEvent(ofDragInfo dragInfo) override {
		if(liveApp) liveApp->dragEvent(dragInfo);
	}
	
	void gotMessage(ofMessage msg) override {
		if(liveApp) liveApp->gotMessage(msg);
	}
	
private:
	ReloadableClass<ofBaseApp> reloader;
	ofBaseApp *liveApp = nullptr;
	string className;
	std::filesystem::path srcDir;
	std::filesystem::path OF_PATH = "../../..";
};

