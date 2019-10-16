#pragma once
#include <string>
using namespace std;
class Dylib {
public:
	
	void open(string path);
	void *get(string methodName);
	void close();
	
	bool isOpen();
private:
	
	void *dylib = NULL;
};
