#include "LiveAudio.h"

#include <math.h>
#include <vector>
using namespace std;

class MyLiveAudio : public LiveAudio {
public:
	
	float random() {
		return (rand() % 10000) / 5000.f - 1.f;
	}
	int counter = 0;

	float kickEnvelope = 0.f;
	float snareEnvelope = 0.f;
	float hatEnvelope = 0.f;
	
	int beat = 0;
	
	vector<float> kick = {
		1, 0, 0, 1,
		0, 0, 0.8, 0
	};
	
	vector<float> snare = {
		0, 0, 0, 0,
		1, 0, 0, 0
	};
	vector<float> hat = {
		0.5, 0.25, 1, 0.25,
		0.1, 0.25, 1, 0.25
	};
	
	class Filter {
	public:
		float c = 0.5;
		float out = 0.f;
		float loPass(float f) {
			out = out * c + f * (1.f - c);
			return out;
		}
		float hiPass(float f) {
			return f - loPass(f);
		}
	};

	class Voice {
	public:
		float mtof(float f) {
			return (8.17579891564 * exp(.0577622650 * f));
		}
		int pos = 0;
		double frequency = 0.f;
		float envelope = 0;
		double phase = 0.f;
		int duration = 0;

		vector<int> melody = { 
			48, 2, 43, 1, 44, 1, 46, 2, 44, 1, 43, 1, 41, 2, 41, 1, 44, 1, 48, 2, 46, 1, 
			44, 1, 43, 3, 44, 1, 46, 2, 48, 2, 44, 2, 41, 2, 41, 4,  0, 0, 46, 2, 49, 1, 
			53, 2, 51, 1, 49, 1, 48, 3, 44, 1, 48, 2, 46, 1, 44, 1, 43, 3, 44, 0, 46, 2, 
			48, 2, 44, 2, 41, 2, 41, 4};

		void tick() {
			duration--;
			if(duration<=0) {
				
				if(melody[pos*2]!=0) {
					frequency = mtof(20 + melody[pos*2]);
					duration = melody[pos*2+1];
					envelope = 1;
					phase = 0;
				}
				pos++;
				pos %= melody.size()/2;
			}
		}

		float getSample() {
			envelope *= 0.9997;
			float out = (sin(phase) + sin(phase * 3)*0.5 + sin(phase * 5)*0.25 + sin(phase * 7)*0.125) * 0.2 * envelope;
			phase += frequency * M_PI * 2.f / 44100.f;
			return out;
		}
	};
	

	Filter snareFilter;
	Filter hatFilter;
	Voice voice;
	
	float getSample() {

		if(counter==0) {
			
			if(kick[beat]>0) {
				kickEnvelope = kick[beat];
			}
			
			if(snare[beat]>0) {
				snareEnvelope = snare[beat];
			}
			if(hat[beat%hat.size()]>0) {
				hatEnvelope = hat[beat];
			}

			voice.tick();
			beat++;
			beat %= kick.size();
		}
			
		kickEnvelope *= 0.999f;
		snareEnvelope *= 0.9993f;
		hatEnvelope *= 0.99;
		return
		kickEnvelope * sin(counter * 0.01) +
		snareFilter.loPass(snareEnvelope * random()) +
		hatFilter.hiPass(hatEnvelope * random()) +
		voice.getSample()
		;
	}
 
	

	
	 
	void audioOut(float *samples, int length, int numChans) override {
		snareFilter.c = 0.5;
		for(int i = 0; i < length; i++) {
			counter++;
			if(counter>8000) {
				counter = 0;
			} 
			samples[i*2] = samples[i*2+1] = getSample();
		}
	}	
};
