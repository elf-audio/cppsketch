//
//  MZGL
//
//  Created by Marek Bereza on 15/01/2018.
//  Copyright © 2018 Marek Bereza. All rights reserved.
//
/*
 
 
 for this to work, you need to put this in your Other C++ flags
 -DSRCROOT=\"${SRCROOT}\"
 
 
 found some good nuggets here
 https://glandium.org/blog/?p=2764
 about how to do precompiled headers.
 
 What this does is precompile headers when the app starts if we're doing a LiveCodeApp.
 It shouldn't really be in here, this should be for a general RecompilingDylib
 
 */

#pragma once
#include "FileWatcher.h"
#include "Dylib.h"
#include "liveCodeUtils.h"
#include <sys/stat.h>
#include <fstream>


class RecompilingDylib {
public:
	string LIBROOT = "";
	
	Dylib dylib;
	

	FileWatcher watcher;
	string path;
	
	function<void(void*)> recompiled;
	function<void()> willCloseDylib;
	
	function<void()> successCallback;
	function<void(string)> failureCallback;
	
	void setup(string path, string LIBROOT = "") {
		this->LIBROOT = LIBROOT;
		this->path = findFile(path);
		
		precompileHeaders();
		
		watcher.watch(this->path);
		watcher.touched = [this]() {
			recompile();
		};
	}
	
	void update() {
		watcher.tick();
	}
	
	
	void precompileHeaders() {
		if(LIBROOT!="") {
			string cmd = "g++ -std=c++14 -stdlib=libc++ "
				+ getAllIncludes(string(LIBROOT))
				+ " " + string(LIBROOT) + "/mzgl/App.h";
			printf("Precompiling headers: %s\n", cmd.c_str());
			liveCodeUtils::execute(cmd);
		} else {
			printf("Warning: not using any precompiled headers\n");
		}
	}
	
	
	
	
private:
	string lastErrorStr;
	void recompile() {
		//printf("Need to recompile %s\n", path.c_str());
		float t = liveCodeUtils::getSeconds();

		string dylibPath = cc();
		if(dylibPath!="") {
			
			loadDylib(dylibPath);
			printf("Compile took %.0fms\n", (liveCodeUtils::getSeconds() - t)*1000.f);
			if(successCallback!=nullptr) {
				successCallback();
			}
		} else {
			if(failureCallback!=nullptr) {
				failureCallback(lastErrorStr);
			}
		}
	}
	
	string getAllIncludes() {
		return " -I. -Isrc"//getAllIncludes(string(SRCROOT))

		// this is the precomp header + " -include " + string(LIBROOT) + "/mzgl/App.h"
		//+ getAllIncludes(string(LIBROOT)) + "/mzgl/ "
		//+ getAllIncludes(string(LIBROOT)+"/../opt/dsp/")
		;
	}
	
	string cc() {
		// call our makefile
		string objectName = getObjectName(path);
		string objFile = "/tmp/" + objectName + ".o";
		string cppFile = "/tmp/"+objectName + ".cpp";
		string dylibPath = "/tmp/" + objectName + ".dylib";
		makeCppFile(cppFile, objectName);

		string includes = getAllIncludes();
		
			
		
		string cmd = "g++ -std=c++14 -g -Wno-deprecated-declarations -stdlib=libc++ -c " + cppFile + " -o " + objFile + " "
					+ includes;
		
		int exitCode = 0;
		string res =  liveCodeUtils::execute(cmd, &exitCode);
		
		//printf("Exit code : %d\n", exitCode);
		if(exitCode==0) {
			cmd = "g++ -dynamiclib -g -undefined dynamic_lookup -o "+dylibPath + " " + objFile;
			liveCodeUtils::execute(cmd);
			return dylibPath;
		} else {
			lastErrorStr = res;
			return "";
		}
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
	
	string getAllIncludes(string basePath) {
		string includes = "";
		vector<Directory> dirs;
		Directory dir(basePath);
		recursivelyFindAllDirs(dir, dirs);
		for(int i = 0; i < dirs.size(); i++) {
		//	printf("%s\n", dirs[i].path.c_str());
			includes += " -I" + dirs[i].path;
		}
		return includes;
	}
	
	
	void makeCppFile(string path, string objName) {
		ofstream outFile(path.c_str());
		
		outFile << "#include \""+objName+".h\"\n\n";
		outFile << "extern \"C\" {\n\n";
		outFile << "\n\nvoid *getPluginPtr() {return new "+objName+"(); };\n\n";
		outFile << "}\n\n";
		
		outFile.close();
		
	}
	
	string getObjectName(string p) {
		// split on last '/'
		int indexOfLastSlash = p.rfind('/');
		if(indexOfLastSlash!=-1) {
			p = p.substr(indexOfLastSlash + 1);
		}
		
		// split on last '.'
		int indexOfLastDot = p.rfind('.');
		if(indexOfLastDot!=-1) {
			p = p.substr(0, indexOfLastDot);
		}
		return p;
	}
	
	void loadDylib(string dylibPath) {

		if(dylib.isOpen()) {
			if(willCloseDylib) willCloseDylib();
			dylib.close();
		}
		dylib.open(dylibPath);
		void *dlib = dylib.get("getPluginPtr");
		if(recompiled) {
			recompiled(dlib);
		}
	}

	
	/////////////////////////////////////////////////////////
	
	
	
	
	
	
	string findFile(string file) {
		string f = file;
		if(fileExists(f)) return f;
		f = "src/" + f;
		if(fileExists(f)) return f;
		f = "../" + f;
		if(fileExists(f)) return f;
		//f = string(SRCROOT) + "/" + file;
		//if(fileExists(f)) return f;
		f = Directory::cwd() + "/" + file;
		if(fileExists(f)) return f;
		printf("Error: can't find source file %s\n", file.c_str());
		return f;
	}
	
	bool fileExists(string path) {
		struct stat fileStat;
		return !(stat(path.c_str(), &fileStat) < 0);
	}
};
