

#include <unistd.h>
#include "ReloadableClass.h"
#include "LiveAudio.h"
#include "RtAudio.h"

LiveAudio *audio = nullptr;

mutex audioMutex;

// Two-channel sawtooth wave generator.
int audioOutCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData) {

	float *output = (float*) outputBuffer;
	audioMutex.lock();
	if(audio) {
		audio->audioOut(output, nBufferFrames, 2);
	}
	audioMutex.unlock();
	return 0;
}


RtAudio rtAudio;

void startRtAudio() {
	
	RtAudio::StreamParameters parameters;
	parameters.deviceId = rtAudio.getDefaultOutputDevice();
	parameters.nChannels = 2;
	parameters.firstChannel = 0;
	unsigned int bufferFrames = 256; // 256 sample frames
	
	try {
		rtAudio.openStream( &parameters, NULL, RTAUDIO_FLOAT32, 44100, &bufferFrames, &audioOutCallback, nullptr );
		rtAudio.startStream();
	} catch ( RtAudioError& e ) {
		e.printMessage();
		exit( 0 );
	}
}



int main(int argc, char * argv[]) {

	ReloadableClass<LiveAudio> dylib;

	dylib.reloaded = [](LiveAudio *ptr) {
		audioMutex.lock();
		audio = ptr;
		audio->setup();
		audioMutex.unlock();
	};

	dylib.reloadFailed = [](const string &msg) {
		audioMutex.lock();
		audio = nullptr;
		audioMutex.unlock();
	};
	
	dylib.init("MyLiveAudio.h");
	

	startRtAudio();

	while(1) {
		
		dylib.checkForChanges();
		
		usleep(1000*1000);
	}

	return 0;
}