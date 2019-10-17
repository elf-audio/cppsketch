#include "FileWatcher.h"


#include <sys/stat.h>
#include <unistd.h>

using namespace std;

void FileWatcher::watch(const string &path) {
	watchedFiles.emplace_back(path);
}



void FileWatcher::tick() {
	bool hasBeenTouched = false;
	for(auto &file : watchedFiles) {
		struct stat fileStat;
		if(stat(file.path.c_str(), &fileStat) < 0) {
			printf("Couldn't find file %s\n", file.path.c_str());
			return;
		}
		long currUpdateTime = fileStat.st_mtime;
		if(currUpdateTime!=file.prevUpdateTime) {
			hasBeenTouched = true;
		}
		file.prevUpdateTime = currUpdateTime;
	}
	
	if(hasBeenTouched && touched!=nullptr) touched();
}





