#include "LiveAudio.h"



class MyLiveAudio : public LiveAudio {
public:

	float phase = 0;
	float lfo = 0;

	void audioOut(float *samples, int length, int numChans) override {
		for(int i = 0; i < length; i++) {
			samples[i*2] = samples[i*2+1] = phase * 0.2;
			phase += 0.01 + lfo*0.001;
			if(phase>1.f) phase -= 2.f;
			lfo += 0.000005;
			if(lfo>1.f) lfo -= 2.f;
		}
	}	
};
