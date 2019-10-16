#include "FileWatcher.h"


#include <sys/stat.h>
#include <unistd.h>


void FileWatcher::watch(string path) {
	this->path = path;	
}

void FileWatcher::tick() {
	
	// // slight relief on the disk
	// waitCount++;
	// if(waitCount < 30) {
	// 	return;
	// }
	// waitCount = 0;
	
	struct stat fileStat;
	if(stat(path.c_str(), &fileStat) < 0) {
		printf("Couldn't stat file %s\n", path.c_str());
		char d[512];
		getcwd(d, 512);
		printf("Curr Dir: %s\n", d);
		return;
	}
	long currUpdateTime = fileStat.st_mtime;
	if(currUpdateTime!=prevUpdateTime) {
		if(touched) touched();
	}
	prevUpdateTime = currUpdateTime;
}





