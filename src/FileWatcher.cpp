#include "FileWatcher.h"


#include <sys/stat.h>
#include <unistd.h>

using namespace std;

void FileWatcher::watch(string path) {
	this->path = path;	
}

void FileWatcher::tick() {
	

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





