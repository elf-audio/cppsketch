#pragma once
#include <functional>
#include <string>
#include <vector>

class FileWatcher {
public:
	std::function<void()> touched;
	
	void watch(const std::string &path);
	// for now you have to call tick() every frame or so to check whether file has been updated
	void tick();
	
private:
	struct WatchedFile {
		std::string path;
		long prevUpdateTime = 0;
		WatchedFile(const std::string &path) : path(path) {}
	};
	
	std::vector<WatchedFile> watchedFiles;
//	long prevUpdateTime = 0;
//	std::string path;

};

