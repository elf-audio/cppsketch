RTAUDIO = ../../ext/rtaudio
FRAMEWORKS = -framework CoreAudio -framework CoreFoundation
INCLUDES = -I../../src/ -I$(RTAUDIO)
CPPFILES = ../../src/Dylib.cpp ../../src/FileWatcher.cpp ../../src/liveCodeUtils.cpp
CPPFLAGS = -D__MACOSX_CORE__ -stdlib=libc++ -std=c++14 $(FRAMEWORKS)
SOURCES = main.cpp $(CPPFILES) $(RTAUDIO)/RtAudio.cpp
all:
	g++ $(CPPFLAGS) $(SOURCES) $(INCLUDES) -o audio
