RTAUDIO = ../../ext/rtaudio
FRAMEWORKS = -framework CoreAudio -framework CoreFoundation
INCLUDES = -I../../src/ -I$(RTAUDIO)
CPPFLAGS = -D__MACOSX_CORE__ -stdlib=libc++ -std=c++14 $(FRAMEWORKS)
SOURCES = main.cpp ../../src/*.cpp $(RTAUDIO)/RtAudio.cpp
all:
	g++ $(CPPFLAGS) $(SOURCES) $(INCLUDES) -o audio
