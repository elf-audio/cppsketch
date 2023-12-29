

#include <unistd.h>
#include "ReloadableClass.h"
#include "LiveAudio.h"
#include "RtAudio.h"
#include <mutex>

LiveAudio *audio = nullptr;

std::mutex audioMutex;

// Two-channel sawtooth wave generator.
int audioOutCallback(void *outputBuffer,
					 void *inputBuffer,
					 unsigned int nBufferFrames,
					 double streamTime,
					 RtAudioStreamStatus status,
					 void *userData) {
	float *output = (float *) outputBuffer;
	audioMutex.lock();
	if (audio) {
		audio->audioOut(output, nBufferFrames, 2);
	} else {
		memset(output, 0, nBufferFrames * 2 * sizeof(float));
	}
	audioMutex.unlock();
	return 0;
}

RtAudio rtAudio;

void startRtAudio() {
	RtAudio::StreamParameters parameters;
	parameters.deviceId		  = rtAudio.getDefaultOutputDevice();
	parameters.nChannels	  = 2;
	parameters.firstChannel	  = 0;
	unsigned int bufferFrames = 256; // 256 sample frames

	try {
		rtAudio.openStream(&parameters, NULL, RTAUDIO_FLOAT32, 44100, &bufferFrames, &audioOutCallback, nullptr);
		rtAudio.startStream();
	} catch (RtAudioError &e) {
		e.printMessage();
		exit(0);
	}
}

int main(int argc, char *argv[]) {
	ReloadableClass<LiveAudio> dylib;

	dylib.willCloseDylib = []() {
		audioMutex.lock();
		audio = nullptr;
		audioMutex.unlock();
	};
	dylib.reloaded = [](LiveAudio *ptr) {
		audioMutex.lock();
		audio = ptr;
		audio->setup();
		audioMutex.unlock();
	};

	dylib.init("MyLiveAudio.h");

	startRtAudio();

	while (1) {
		dylib.checkForChanges();

		usleep(1000 * 1000);
	}

	return 0;
}
