#pragma once

#include <string>
class Dylib {
public:
	
	bool open(std::string path);
	void *get(std::string methodName);
	void close();
	
	bool isOpen();
private:
	
	void *dylib = nullptr;
};
