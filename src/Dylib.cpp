

#include "Dylib.h"

#include <dlfcn.h>

using namespace std;

bool Dylib::open(string path) {
	close();
	
	dylib = dlopen(path.c_str(), RTLD_LAZY);
	if(dylib==nullptr) {
		printf("Got dlopen error: %s\n", dlerror());
	}
	return dylib != nullptr;
}

void *Dylib::get(string methodName) {
	void *ptrFunc = dlsym(dylib, methodName.c_str());
	if(ptrFunc!=nullptr) {
		return ((void *(*)())ptrFunc)();
	} else {
		printf("Couldn't find the %s() function\n", methodName.c_str());
		return nullptr;
	}
}

void Dylib::close() {
	if(dylib!=nullptr) {
		dlclose(dylib);
		dylib = nullptr;
	}
}

bool Dylib::isOpen() {
	return dylib != nullptr;
}
