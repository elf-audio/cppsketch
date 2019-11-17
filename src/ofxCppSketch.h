#pragma once

#include "ofMain.h"
#include "ReloadableClass.h"
#include "ReloadableSoundStream.h"
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
		reloader.reloadStarted = [this]() {
			getSoundStream()->audioMutex.lock();
			getSoundStream()->inCallback = nullptr;
			getSoundStream()->outCallback = nullptr;
		};
		reloader.reloaded = [this](ofBaseApp *app) {
			
			liveApp = app;
			liveApp->setup();
			getSoundStream()->audioMutex.unlock();
		};
		
		// the only thing you have to change in settings is "Dead Code Stripping"

		auto pathToLiveFile = srcDir / (className + ".h");
		
		auto includes = getAllIncludes();
		
		// start the auto-reloading
		auto headerToPrecompile = getOfLibPath() / "ofMain.h";
		reloader.init(pathToLiveFile.string(), includes, headerToPrecompile.string());
		
		// watch any other header files for changes, but not any .cpp files
		// because that's too complicated for now
		auto otherHeadersInSrc = liveCodeUtils::getAllHeaderFiles(srcDir.string());
		for(auto header : otherHeadersInSrc) {
			reloader.addFileToWatch(header);
		}
	}
	
	// I know this is bad, basically, the shared_ptr will be owned forever and never freed,
	// but it's the format which oF wants.
	static shared_ptr<ReloadableSoundStream> getSoundStream();

protected:
	
	
	std::filesystem::path getLibsPath() {
		return srcDir.parent_path() / OF_PATH / "libs";

	}
	std::filesystem::path getAddonsPath() {
		return srcDir.parent_path() / OF_PATH / "addons";

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
		
		auto addons = getAddonNames();
		
		for(const auto &addon : addons) {
			auto addonPath = getAddonsPath() / addon;
			vector<string> allAddonIncludes = getAllIncludeDirsForAddon(addonPath);
			includes.insert(includes.end(), allAddonIncludes.begin(), allAddonIncludes.end());
			
		}
		
		return includes;
	}
	
	bool isIncludePath(string path) {
		int incPos = path.rfind("/include");
		return (incPos==path.size()-8) ||
		(incPos==path.size()-9 && path[path.size()-1]=='/');
	}
	
	
	vector<string> getAllIncludeDirsForAddon(const std::filesystem::path &addonPath) {
		vector<string> includes;
		includes.push_back((addonPath / "src").string());
		// get all dirs recursively
		auto allDirs = liveCodeUtils::getAllDirectories(addonPath.c_str());
		for(const auto &dir: allDirs) {
			if(isIncludePath(dir)) {
				includes.push_back(dir);
			}
		}

		return includes;
	}
	
	vector<string> getAddonNames() {
		// this reads the addons.make file for adding
		// addon paths to the includes
		std::ifstream infile((srcDir.parent_path() / "addons.make").string());
		vector<string> addons;
		std::string line;
		while (std::getline(infile, line)) {
			if(line.size()>3 /*&& line!="ofxCppSketch"*/) { // check there's some text on the line
				addons.push_back(line);
				printf("%s\n", line.c_str());
			}
		}
		return addons;
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

