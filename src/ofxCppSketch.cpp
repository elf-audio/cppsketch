
#include "ofxCppSketch.h"

//static ReloadableSoundStream ofxCppSketch::soundStream;

// I know this is bad, basically, the shared_ptr will be owned forever and never freed,
// but it's the format which oF wants.
shared_ptr<ReloadableSoundStream> ofxCppSketch::getSoundStream() {
	static shared_ptr<ReloadableSoundStream> soundStream = nullptr;
	if(soundStream==nullptr) {
		soundStream = make_shared<ReloadableSoundStream>();
	}
	return soundStream;
}
