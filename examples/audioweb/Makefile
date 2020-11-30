RTAUDIO = ../../ext/rtaudio
FRAMEWORKS = -framework CoreAudio -framework CoreFoundation -framework Cocoa -framework WebKit
INCLUDES = -I../../src/ -I$(RTAUDIO)
CPPFILES = ../../src/Dylib.cpp ../../src/FileWatcher.cpp ../../src/liveCodeUtils.cpp
CPPFLAGS = -stdlib=libc++ -std=c++14 $(FRAMEWORKS)


CPPFLAGS =  -std=c++14 

ifeq ($(OS),Windows_NT)
	
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CPPFLAGS += -ldl -D__LINUX_ALSA__ -lasound -lpthread
	endif
	ifeq ($(UNAME_S),Darwin)
		CPPFLAGS +=  -stdlib=libc++ -D__MACOSX_CORE__ $(FRAMEWORKS)
	endif
endif



EMSCRIPTEN_OPTIONS = -s WASM=1 \
-s ALLOW_MEMORY_GROWTH=1 \
-s EXPORTED_FUNCTIONS="['_getSamples', '_update', '_malloc', '_free']" \
-s EXTRA_EXPORTED_RUNTIME_METHODS="['cwrap','ccall']"


SOURCES = main.mm AppDelegate.mm $(CPPFILES) $(RTAUDIO)/RtAudio.cpp
all:
	g++ $(CPPFLAGS) $(SOURCES) $(INCLUDES) -o audioweb

web:
	em++ emscripten/em.cpp -O3 $(INCLUDES) -I. -o emscripten/em.js $(EMSCRIPTEN_OPTIONS)