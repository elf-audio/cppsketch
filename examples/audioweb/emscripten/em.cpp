
#include "MyLiveAudioWeb.h"

MyLiveAudioWeb audioweb;

extern "C" {

	void getSamples(float *samples, int length, int numChans) {
		audioweb.audioOut(samples, length, numChans);
	}

	void update() {
		audioweb.update();
	}

	void setParameter(char *key, char *value) {
		audioweb.setParameter(key, value);
	}

};