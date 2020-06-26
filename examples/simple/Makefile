CPPFILES = ../../src/Dylib.cpp ../../src/FileWatcher.cpp ../../src/liveCodeUtils.cpp



CPPFLAGS =  -std=c++14 
ifeq ($(OS),Windows_NT)
	
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CPPFLAGS += -ldl 
	endif
	ifeq ($(UNAME_S),Darwin)
		CPPFLAGS +=  -stdlib=libc++ 
	endif
endif

all:
	g++ $(CPPFLAGS) main.cpp $(CPPFILES) -I../../src/ -o simple
