#include "LiveAudioWeb.h"
#include "Biquad.h"
#include <math.h>

using namespace std;










class DubDelayEffectKernel {
public:
	vector<float> buffer;
	float value = 0;
	float	actualDelayTime;
	int		writeHead;
	float	readHead;
	int		delay;
	float	feedback;
	float mix;
	float lerpAmt;
	float lerpAmtM;
	static constexpr int MAXDELAY = 44100;
	DubDelayEffectKernel() {
		actualDelayTime = 0;
		writeHead		= 0;
		readHead		= 0;
		delay		= 11025;
		feedback		= 0.9;
		mix = 0.5;
		buffer.resize(MAXDELAY);
		fill(buffer.begin(), buffer.end(), 0);
		lerpAmt = 0.99975;
		lerpAmtM = 1 - lerpAmt;
		filt.calc(Biquad::BANDPASS_CSG, 700, 48000, 0.86, 0, false);
	}
	
	void clear() {
		fill(buffer.begin(), buffer.end(), 0);
	}
	
	float readFrac(vector<float> &buff, float index) {
		int i = index;
		int j = index + 1;
		if(j >= buff.size()) j -= buff.size();
		float frac = index - i;
		
		return buff[i] * (1 - frac) + buff[j] * frac;
	}

	Biquad filt;
	float filter(float in) {
		return tanh(filt.filter(in));
	}

	float process(float inp) {
		delay = 1000.f + value*21050;
		writeHead++;
		writeHead %= MAXDELAY;
		
		actualDelayTime = actualDelayTime * lerpAmt + delay * lerpAmtM;
		
		readHead = writeHead - actualDelayTime;
		if(readHead<0) readHead += MAXDELAY;
		
		float out = readFrac(buffer, readHead);
		
		buffer[writeHead] = filter(out * feedback + inp);
		
		return inp + (out - inp) * mix;
	}
};




class DubDelayEffect {
public:
	DubDelayEffectKernel l, r;
	float wetMixValue = 0.5f;
	
	bool wasEnabled = false;
	float value = 0.f;

	void setDelayTime(float dt) {
		l.value = dt;
		r.value = 0.03 + dt * 0.97;

	}
	void setFeedback(float fb) {
		l.feedback = fb;
		r.feedback = fb;
	}

	void setLevel(float level) {
		l.mix = level;
		r.mix = level;
	}
	void process(float *b, int length) {
		

		if(l.mix<=0.f) return;
		

		for(int i = 0; i < length; i++) {

			
			b[i*2] = l.process(b[i*2]);
			b[i*2+1] = r.process(b[i*2+1]);
		}
	}
};





















class Voice {
public:
	
	

	int pos = 1000000;
	double ph = 0;




	// these are the coefficients - need to get the scale mapping right
	float baseFreq = 50;
	
	// amplitude
	float ampCurve = 1.2;
	float attackAmt = 40;
	float releaseAmt = 10000;
	
	// fm 
	float ratio = 2.0f;
	float fmAmt = 2.0f;
	float fmFeedback = 0.1f;
	float fmRelease = 5000;
	float fmCurve = 1.2;
	

	// pitch
	float pitchAmt = 200;
	float pitchRelease = 10000;
	float pitchCurve = 10;
	
	


	float lastOp1 = 0.f;

	
	void trigger() {
		pos = 0;
		ph = 0;
	}

	double phase = 0.0;
	float level = 0.5;
	void setLevel(float l) {
		level = l;
	}
	void setPitch(float p) {
		baseFreq = 50 + 1000*p;
	}

	void setAttack(float att) {
		attackAmt = 10 + att * 100;
	}
	void setRelease(float rel) {
		releaseAmt = 1000 + 20000 * rel;
	}
	void setRatio(float rat) {
		ratio = 0.5 + 12 * rat;
	}
	void setFMAmt(float fmAmt) {
		this->fmAmt = fmAmt * 10;
	} 

	void setFMFeedback(float fmFeedback) {
		this->fmFeedback = fmFeedback;
	}

	void setFMRelease(float fmRelease) {
		this->fmRelease = 100 + fmRelease * 10000;
	}
	void setPitchModAmt(float pitchModAmt) {
		this->pitchAmt = pitchModAmt * 1000;
	}
	void setPitchRelease(float pitchRelease) {
		this->pitchRelease = 100 + pitchRelease * 20000;
	}

	float pan = 0.5;
	float reverbSend = 0.f;
	float delaySend = 0.f;

	void setPan(float pan) {
		this->pan = pan;
	}

	void setReverbSend(float reverbSend) {
		this->reverbSend = reverbSend;
	}

	void setDelaySend(float delaySend) {
		this->delaySend = delaySend;
	}


	void getSample(float &L, float &R, float &reverb, float &delay) {
		
		

		float env = 0.f;
		
		if(pos<attackAmt) {
			env = pos / attackAmt;
		} else {
			env = 1.f - (pos - attackAmt) / releaseAmt;
			if(env<0) env = 0.f;
		}
		env = pow(env, ampCurve);


		float fmEnv = 1.f - (pos / fmRelease);
		if(fmEnv<0) fmEnv = 0;
		fmEnv = pow(fmEnv, fmCurve);
		
		float pitchEnv = 1.f - (pos / pitchRelease);
		if(pitchEnv<0) pitchEnv = 0;
		pitchEnv = pow(pitchEnv, pitchCurve);

		
		float op1 = sin(ph*ratio + lastOp1*fmFeedback);
		lastOp1 = op1;
		float op2 = sin(ph + op1*fmEnv*fmAmt);


		float w = 2.0 * M_PI / 44100.f;
		
		ph += w * (baseFreq + pitchEnv * pitchAmt);

		pos++;



		float out = op2 * env *level;
		if(pan>0.5f) {
			float lVolTarget = 1.f - 2.f * (pan-0.5);
			L += out * lVolTarget;
			R += out;
		} else {
			float rVolTarget = 2.f * pan;
			L += out;
			R += out * rVolTarget;
		}
		reverb += out * reverbSend;
		delay += out * delaySend;
	}


	vector<int> pattern;
	Voice() : pattern(16, 0) {

	}
};


class MyLiveAudioWeb : public LiveAudioWeb {
public:
	vector<Voice> voices;
	DubDelayEffect del;

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

	int period = 5000;
	void getSample(float &L, float &R, float &reverb, float &delay) {

		

		if(pos>period) {
			tick();
			pos = 0;
			osc.phase = 0;
			op1.phase = 0;
			beat = (beat+1) % 16;
		}

		for(auto &v : voices) {
			v.getSample(L, R, reverb, delay);
		}
	
		
		pos++;

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
			string param = parts[2];



			float val = stof(value);
			if(param=="pitch") {
				voices[voiceNo].setPitch(val);
			} else if(param=="attack") {
				voices[voiceNo].setAttack(val);
			} else if(param=="release") {
				voices[voiceNo].setRelease(val);
			} else if(param=="ratio") {
				voices[voiceNo].setRatio(val);
			} else if(param=="fmAmt") {
				voices[voiceNo].setFMAmt(val);
			} else if(param=="fmFeedback") {
				voices[voiceNo].setFMFeedback(val);
			} else if(param=="fmRelease") {
				voices[voiceNo].setFMRelease(val);
			} else if(param=="pitchModAmt") {
				voices[voiceNo].setPitchModAmt(val);
			} else if(param=="pitchRelease") {
				voices[voiceNo].setPitchRelease(val);
			} else if(param=="level") {
				voices[voiceNo].setLevel(val);
			} else if(param=="pan") {
				voices[voiceNo].setPan(val);
			} else if(param=="reverbSend") {
				voices[voiceNo].setReverbSend(val);
			} else if(param=="delaySend") {
				voices[voiceNo].setDelaySend(val);
			}

			
		} else if(parts[0]=="glob") {
			if(parts[1]=="delayTime") {
				del.setDelayTime(stof(value));
			} else if(parts[1]=="delayFeedback") {
				del.setFeedback(stof(value));
			} else if(parts[1]=="delayLevel") {
				del.setLevel(stof(value));
			} else if(parts[1]=="bpm") {
				float bpm = stof(value);
				float bps = bpm / 60.f;
				float sampsPerBeat = 44100.f / bps;
				period = sampsPerBeat/4;
				printf("%d\n", period);
			}
		} else if(parts[0]=="seq") {
			int voiceNo = stoi(parts[1]);
			int stepNo = stoi(parts[2]);
			bool on = value=="true";
			voices[voiceNo].pattern[stepNo] = on;
		}
	}

	vector<float> delaySignal;
	void audioOut(float *samples, int length, int numChans) override {
		if(delaySignal.size()<length) {
			delaySignal.resize(8192);
		}
		float L, R;
		float delay = 0;
		float reverb = 0;
		for(int i = 0; i < length; i++) {
			L = 0.f;
			R = 0.f;
			delay = 0;
			reverb = 0;
			getSample(L, R, reverb, delay);
			samples[i*2] = L;
			samples[i*2+1] = R;
			delaySignal[i*2] = delaySignal[i*2+1] = delay;
		}

		del.process(delaySignal.data(), length);
		for(int i = 0; i < length*2; i++) {
			samples[i] += delaySignal[i];
		}
	}	
};