#pragma once
#include <functional>
#include <string>
using namespace std;

class FileWatcher {
public:
	function<void()> touched;
	
	void watch(string path);
	// for now you have to call tick() every frame or so to check whether file has been updated
	void tick();
	
private:
	long prevUpdateTime = 0;
	string path;
	int waitCount = 0;
};

