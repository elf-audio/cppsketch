
#include <string>
#include <functional>
#include <vector>

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

class LiveAudioWeb {
public:
	
	

	// call this to execute js in your page
	void executeJS(const std::string &js) {
		// if emscripten
#ifdef __EMSCRIPTEN__
		emscripten_run_script(js.c_str());
#else
		// if native
		jsExternalCall(js);
#endif
	} 
	
	// called at the beginning of the newly reloaded sketch
	virtual void setup() {}

	// calls on main thread, so this is where you do your comms
	virtual void update() {}


	virtual void audioOut(float *samples, int length, int numChans) {}	
	virtual void setParameter(const std::string &key, const std::string &value) {}
	// virtual void jsReceived(const std::string &s, const std::vector<std::string> &params) {}

	// implementation detail, do not change
	std::function<void(const std::string &js)> jsExternalCall;
};

