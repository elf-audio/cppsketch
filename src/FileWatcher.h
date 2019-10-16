#pragma once
#include <functional>
#include <string>


class FileWatcher {
public:
	std::function<void()> touched;
	
	void watch(std::string path);
	
	// for now you have to call tick() every frame or so to check whether file has been updated
	void tick();
	
private:
	long prevUpdateTime = 0;
	std::string path;

};

