

class LiveAudio {
public:
	virtual void setup() {}
	virtual void audioOut(float *samples, int length, int numChans) {}	
};

