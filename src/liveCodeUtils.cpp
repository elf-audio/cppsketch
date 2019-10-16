//
//  glue.h
//  example-simple
//
//  Created by Marek Bereza on 16/10/2019.
//

#include "liveCodeUtils.h"
#include <stdio.h>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
namespace liveCodeUtils {
	std::chrono::system_clock::time_point startTime;
};

void liveCodeUtils::init() {
	liveCodeUtils::startTime = std::chrono::system_clock::now();
}

string liveCodeUtils::execute(string cmd, int *outExitCode) {
#ifdef __APPLE__
	printf("Executing %s", cmd.c_str());
	cmd += " 2>&1";
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe) return "ERROR";
	char buffer[128];
	std::string result = "";
	while(!feof(pipe)) {
		if(fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	
	int exitCode = pclose(pipe);
	if(outExitCode!=nullptr) {
		*outExitCode = WEXITSTATUS(exitCode);
	}
	printf("%s\n", result.c_str());
	return result;
#else
	return "Error - can't do this";
#endif
}



float liveCodeUtils::getSeconds() {

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-liveCodeUtils::startTime;
	return elapsed_seconds.count();
}



void recursivelyFindAllDirs(Directory curr, vector<Directory> &dirs) {
	dirs.push_back(curr);
	curr.list();
	for(int i = 0; i < curr.size(); i++) {
		if(curr[i].isDirectory()) {
			recursivelyFindAllDirs(Directory(curr[i]), dirs);
		}
	}
}


vector<string> liveCodeUtils::getAllDirectories(string baseDir) {
	vector<Directory> dirs;
	Directory dir(baseDir);
	recursivelyFindAllDirs(dir, dirs);
	vector<string> ret;
	for(auto &d : dirs) {
		ret.push_back(d.path);
	}
	return ret;
}


std::string liveCodeUtils::includeListToCFlags(const std::vector<std::string> &includes) {
	string str;
	for(auto &inc : includes) {
		str += " -I"+inc + " ";
	}
	return str;
}



File::File(string path) {
	this->path = path;
}

string File::fileName() {
	// take off trailing slash
	if(path.rfind("/")==path.size()-1) {
		path.resize(path.size()-1);
	}
	return path.substr(path.rfind("/")+1);
}



string File::getExtension() const {
	int dotPos = path.rfind(".");
	if(dotPos!=-1) {
		return path.substr(dotPos+1);
	} else {
		return "";
	}
}




#include <dirent.h>
#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#include <fstream>




bool File::exists() { 

	#ifdef _WIN32
	printf("ERROR File::exists not supported on windows");
	return false;
	#else 
	struct stat stFileInfo; 
	
	// Attempt to get the file attributes 
	return stat(path.c_str(),&stFileInfo)==0; 
	#endif
}






bool File::remove() {
	if(isDirectory()) {
	//	printf("Can't delete file as it's a directory\n");
		return rmdir(path.c_str())==0;
	} else {
	
		//std::filesystem::path::remove(path);
		return ::remove(path.c_str())==0;
	}
}

bool File::isDirectory() const {
	struct stat stFileInfo; 
	
	// Attempt to get the file attributes 
	if(stat(path.c_str(),&stFileInfo)!=0) {
		printf("File not found\n");
		return false;
	}
	return (stFileInfo.st_mode & S_IFMT) == S_IFDIR;

}

bool File::copy(string destinationPath) {
	if(isDirectory()) {
		printf("File::copy only works on normal files, not directories\n");
		return false;
	}
	FILE *in, *out;

	in = fopen(path.c_str(), "r" );
	out = fopen(destinationPath.c_str(), "w" );
	if(in==NULL || !in) {

		printf("%s: No such file or directory\n",path.c_str());

		return false;
	} else if(out==NULL || !out) {
		printf("%s: No such file or directory\n",destinationPath.c_str());
		return false;
	}

	int c;
	while((c=getc(in))!=EOF)
		putc(c,out);

	fclose(in);
	fclose(out);
	return true;
}



string File::getText() {
	ifstream t(path);
	string str;
	
	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);
	
	str.assign((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());
	return str;
}

void File::setText(string text) {
	ofstream out(path);
	out << text;
	out.close();
}



int Directory::size() {
	return files.size();
}



File &Directory::operator[](unsigned int i) {
	return files[i];
}

Directory::Directory() {

}

static void _mkdirRecursively(const char *dir) {
	char tmp[256];
	char *p = NULL;
	size_t len;
	
	snprintf(tmp, sizeof(tmp),"%s",dir);
	len = strlen(tmp);
	if(tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for(p = tmp + 1; *p; p++)
		if(*p == '/') {
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	mkdir(tmp, S_IRWXU);
}


bool Directory::create(bool recursively) {
	if(!recursively) {
		if(mkdir(path.c_str(), S_IRWXU)==0) {
			return true;
		} else {
			return false;
		}
	} else {
		_mkdirRecursively(path.c_str());
		return true;
	}
}
bool Directory::contains(string fileName) {
	if(files.size()==0) list();
	for(auto &p : files) {
		if(p.fileName()==fileName) return true;
	}
	return false;
}

void Directory::sort() {
	std::sort(files.begin(), files.end());
}

bool Directory::list() {

	if(path.size()>0 && path.back()!='/') {
		path += "/";
	}

	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(path.c_str()))!=NULL) {
		while((ent = readdir(dir))) {
			string fileName = ent->d_name;
			if(fileName!="." && fileName!=".." && fileName!=".DS_Store") {
				File file = File(path + fileName);
				if(fileExtensionAllowed(file)) {
					files.push_back(file);
				}
//			printf("%s\n", files.back().path.c_str());
			}
		}
		closedir(dir);
	} else {
		printf("Couldn't open dir\n");
		perror("");
		return false;
	}
	return true;

}

void Directory::emptyContents() {
	emptyContents(path);
}

void Directory::emptyContents(string dirPath) {
	char file[512];
	DIR *dp;
	struct dirent *dirp;
	struct stat statbuf;
	
	dp = opendir(dirPath.c_str());
	if(dp!=NULL) {
		while((dirp=readdir(dp))!=NULL) {
			if((strcmp(dirp->d_name,".")!=0) && (strcmp(dirp->d_name,"..")!=0)) {
				sprintf(file,"%s/%s",  path.c_str(), dirp->d_name);
				if(dirp->d_type==DT_DIR) {
					emptyContents(file);
					rmdir(file);
				} else {
					::remove(file);
				}
			}
		}
	}
}
bool Directory::up() {
	int slashPos = path.rfind('/');

	if(slashPos!=-1) {
		path = path.substr(0, slashPos);
	} else {
		printf("No directory to go up to\n");
		return false;
	}
	list();
	return true;
}


void Directory::allowExtension(string ext) {
	allowedExtensions.insert(lcase(ext));
}

string Directory::lcase(string str) {
	string out = str;
	std::transform(str.begin(), str.end(), out.begin(), ::tolower);

	return out;
}

bool Directory::fileExtensionAllowed(const File &file) {
	string ext = lcase(file.getExtension());
	if(allowedExtensions.size()==0) return true;
	// now, given that there are extension filters
	if(ext=="") return false;
	return (allowedExtensions.find(ext)!=allowedExtensions.end());
}


string Directory::cwd() {
	char dir[1024];
	getcwd(dir, 1023);
	return dir;
}
