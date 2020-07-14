RTAUDIO = ../../ext/rtaudio
FRAMEWORKS = -framework CoreAudio -framework CoreFoundation
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






SOURCES = main.cpp $(CPPFILES) $(RTAUDIO)/RtAudio.cpp
all:
	g++ $(CPPFLAGS) $(SOURCES) $(INCLUDES) -o tetris
