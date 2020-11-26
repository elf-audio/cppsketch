
#include <string>
#include <functional>
#include <vector>
class LiveAudioWeb {
public:
	
	

	// call this to execute js in your page
	void executeJS(const std::string &js) {
		jsExternalCall(js);
	} 
	
	// called at the beginning of the newly reloaded sketch
	virtual void setup() {}

	// calls on main thread, so this is where you do your comms
	virtual void update() {}


	virtual void audioOut(float *samples, int length, int numChans) {}	

	virtual void jsReceived(const std::string &s, const std::vector<std::string> &params) {}

	// implementation detail, do not change
	std::function<void(const std::string &js)> jsExternalCall;
};

