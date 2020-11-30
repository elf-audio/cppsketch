#include "LiveAudioWeb.h"
#include <math.h>

using namespace std;


#define SINE 0
#define SQUARE 1
#define RANDOM 2


class Filter {
public:
	float cutoff;
	float x = 0, y = 0;
	float process(float input,float cutoff1, float resonance) {
		cutoff=cutoff1;
		if (resonance<1.) resonance = 1.;
		float z=cos(M_PI*2.f*cutoff/44100.f);
		float c=2-2*z;
		float r=(sqrt(2.0)*sqrt(-pow((z-1.0),3.0))+resonance*(z-1))/(resonance*(z-1));
		x=x+(input-y)*c;
		y=y+x;
		x=x*r;
		return y;
	}
};



class LFO {
public:
	float speed = 0.f;
	float depth = 0.f;
	int type = SINE;
	bool isAmp = false;

	void setSpeed(float speed) {
		this->speed = 0.001 + speed*speed*speed;
	}

	void setDepth(float depth) {
		this->depth = depth*depth;
		if(isAmp) this->depth *= 0.5;
	}

	void setType(string type) {
		if(type=="SINE") {
			this->type = SINE;
		} else if(type=="SQUARE") {
			this->type = SQUARE;
		} else if(type=="RANDOM") {
			this->type = RANDOM;
		}
	}

	double phase = 0;
	float randVal = 0.f;
	float getSample() {
		float bias = 0.f;
		if(isAmp) {
			bias = 1.f - depth;
		}
		phase += speed*0.1;
		if(phase>M_PI*2.0) {
			phase -= M_PI *2.0;
			if(type==RANDOM) {
				randVal = ((rand() %10000) / 10000.f)*2.f - 0.1f;
			}
		}
		switch(type) {

			case SINE: return bias + sin(phase)*depth;
			case SQUARE: return bias + (phase>M_PI?1:-1)*depth;
			case RANDOM: return bias + randVal;
		}
		return 0;
	}
};
class Envelope {
public:
	float attack = 10;
	float release = 1000;
	float curve = 1; // 0.5 to 2?

	float targetAttack = 10;
	float targetRelease = 1000;
	float targetCurve = 1;
	int pos = 1000000;
	
	void trigger() {
		pos = -1;
	}

	void setAttack(float a) {
		targetAttack = 10 + a*a * 20000;

	}

	void setRelease(float r) {
		targetRelease = 10 + r*r * 80000;
	}

	void setCurve(float c) {
		targetCurve = c*2 +1;
	}
	float getSample() {
		pos++;
		attack = attack * 0.999 + targetAttack * 0.001;
		release = release * 0.999 + targetRelease * 0.001;
		curve = curve * 0.999 + targetCurve * 0.001;
		if(pos<attack) {
			return pow(pos / attack, 8.f);
		} else if(pos<attack + release) {
			return pow(1.f - (pos - attack) / release, 8.f);
		} else {
			return 0;
		}
	}
};


class Voice {
public:
	LFO pitchLFO;
	LFO ampLFO;
	LFO filterLFO;
	
	Filter filter;

	Envelope ampEnv;
	Envelope pitchEnv;
	Envelope filterEnv;

	float pitch = 1.f;
	float amp = 0.7;

	float targetPitch = 1;
	float targetAmp = 0.7;


	void setPitch(float _pitch) {
		this->targetPitch = 1.f + _pitch*8.0;
	}

	void setAmp(float amp) {
		this->targetAmp = amp;
	}

	float resonance = 2;
	float cutoffCentre = 400;
	float targetCutoffCentre = 400;
	void setCutoff(float c) {
		targetCutoffCentre = 100 + c *10000;
	}

	
	void trigger() {
		ampEnv.trigger();
	}

	double phase = 0.0;
	
	float getSample() {
		pitch = pitch * 0.999 + targetPitch * 0.001;
		amp = amp * 0.99 + targetAmp * 0.01;
		cutoffCentre = cutoffCentre * 0.999 + targetCutoffCentre * 0.001;
		// cutoffCentre = 10000;
		// resonance = 5;

		phase += 0.01f*pitch + pitchLFO.getSample();

		float out = sin(phase);
		// out = filter.process(out, cutoffCentre, resonance);//cutoffCentre, resonance);


		out *= amp*0.5f * ampEnv.getSample()*ampLFO.getSample();
		return out;
	}
	vector<int> pattern;
	Voice() : pattern(16, 0) {
		ampLFO.isAmp = true;
	}
};


class MyLiveAudioWeb : public LiveAudioWeb {
public:
	vector<Voice> voices;
	MyLiveAudioWeb() : voices(6) {

	}

	struct SinOsc {
		double phase = 0;
		double freq = 440;

		SinOsc() {

		}
		float getSample() {
			phase += freq * M_PI * 2.0 / 48000.f;
			return sin(phase);
		}
	};

	SinOsc osc;
	SinOsc op1;

	int pos = 0;
	int beat = 15;
	int sendBeat = -1;
	void tick() {
		sendBeat = beat;
		for(auto &v : voices) {
			if(v.pattern[beat]) {
				v.trigger();
			}
		}
	}

	float getSample() {

		

		if(pos>5000) {
			tick();
			pos = 0;
			osc.phase = 0;
			op1.phase = 0;
			beat = (beat+1) % 16;
		}
		float out = 0;
		for(auto &v : voices) {
			out += v.getSample();
		}
	
		
		pos++;

		return out;
	}
	
	int iii = 0;

	void update() override {
		if(sendBeat!=-1) {
			executeJS("setBeat("+to_string(sendBeat)+")");
			sendBeat = -1;
		}
	}



	vector<string> split(const string& s, char c) {
		vector<string> v;
		string::size_type i = 0;
		string::size_type j = s.find(c);

		while (j != string::npos) {
			v.push_back(s.substr(i, j-i));
			i = ++j;
			j = s.find(c, j);

			if (j == string::npos)
				v.push_back(s.substr(i, s.length()));
		}
		return v;
	}



	void setParameter(const std::string &key, const std::string &value) override {
		// printf("%s = %s\n", key.c_str(), value.c_str());
		auto parts = split(key, '_');
		if(parts[0]=="param") {
			int voiceNo = stoi(parts[1]);
			string paramName = parts[2];
			// value is the value


			if(paramName=="pitchMod.lfo.wave") {
				voices[voiceNo].pitchLFO.setType(value);
			} else if(paramName=="filterMod.lfo.wave") {
				voices[voiceNo].filterLFO.setType(value);
			} else if(paramName=="ampMod.lfo.wave") {
				voices[voiceNo].ampLFO.setType(value);
			} else {

				float val = stof(value);
				if(paramName=="pitch") {
					voices[voiceNo].setPitch(val);
				} else if(paramName=="pitchMod.lfo.speed") {
					voices[voiceNo].pitchLFO.setSpeed(val);
				} else if(paramName=="pitchMod.lfo.depth") {
					voices[voiceNo].pitchLFO.setDepth(val);
				} else if(paramName=="filterMod.lfo.speed") {
					voices[voiceNo].filterLFO.setSpeed(val);
				} else if(paramName=="filterMod.lfo.depth") {
					voices[voiceNo].filterLFO.setDepth(val);
				} else if(paramName=="ampMod.lfo.speed") {
					voices[voiceNo].ampLFO.setSpeed(val);
				} else if(paramName=="ampMod.lfo.depth") {
					voices[voiceNo].ampLFO.setDepth(val);
				} else if(paramName=="level") {
					voices[voiceNo].setAmp(val);
				} else if(paramName=="cutoff") {
					voices[voiceNo].setCutoff(val);
				} else if(paramName=="resonance") {
					voices[voiceNo].resonance = 1 + 9.f*val;
				} else if(paramName=="ampMod.envelope.attack") {
					voices[voiceNo].ampEnv.setAttack(val);
				} else if(paramName=="ampMod.envelope.release") {
					voices[voiceNo].ampEnv.setRelease(val);
				} else if(paramName=="ampMod.envelope.curve") {
					voices[voiceNo].ampEnv.setCurve(val);
				} else if(paramName=="filterMod.envelope.attack") {
					voices[voiceNo].filterEnv.setAttack(val);
				} else if(paramName=="filterMod.envelope.release") {
					voices[voiceNo].filterEnv.setRelease(val);
				} else if(paramName=="filterMod.envelope.curve") {
					voices[voiceNo].filterEnv.setCurve(val);
				} else if(paramName=="pitchMod.envelope.attack") {
					voices[voiceNo].pitchEnv.setAttack(val);
				} else if(paramName=="pitchMod.envelope.release") {
					voices[voiceNo].pitchEnv.setRelease(val);
				} else if(paramName=="pitchMod.envelope.curve") {
					voices[voiceNo].pitchEnv.setCurve(val);
				}
			}

		} else if(parts[0]=="seq") {
			printf("here\n");
			int voiceNo = stoi(parts[1]);
			int stepNo = stoi(parts[2]);
			bool on = value=="true";
			voices[voiceNo].pattern[stepNo] = on;
			// printf("%d [%d] = %d %s\n", voiceNo, stepNo, on, parts[3].c_str());
		}
	}


	void audioOut(float *samples, int length, int numChans) override {
		for(int i = 0; i < length; i++) {
			samples[i*2] = samples[i*2+1] = getSample();
		}
	}	
};