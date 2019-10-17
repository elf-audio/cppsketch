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
#include <vector>

using namespace std;

template <class T>
class ReloadableClass {
public:
	
	Dylib dylib;
	
	FileWatcher watcher;
	string path;
	
	function<void(T*)> reloaded;
	function<void()> willCloseDylib;
	
	function<void(const string&)> reloadFailed;
	vector<string> includePaths;
	string prefixHeader;
	string cppFile = "";
	
	/**
	 * Parameters:
	 *  path - path to the header file of the live class - must have the same name as the class itself
	 *  includePaths - optional all the paths to include
	 *  headerToPrecompile - optional - if you pass in a main header file, it will be precompiled for faster loading times.
	 */
	void init(const string &path, const vector<string> &includePaths = {}, const string &headerToPrecompile = "") {
		
		liveCodeUtils::init();
		this->includePaths = includePaths;
		this->path = findFile(path);
		
		prefixHeader = headerToPrecompile;
		if(headerToPrecompile!="") {
			precompileHeader(headerToPrecompile);
		}
		
		watcher.watch(this->path);
		
		cppFile = getCppFileForHeader(this->path);
		
		if(cppFile!="") {
			watcher.watch(this->cppFile);
		}
		
		watcher.touched = [this]() {
			recompile();
		};
	}
	
	void checkForChanges() {
		watcher.tick();
	}
	
	string getCppFileForHeader(string p) {
		int lastDot = p.rfind('.');
		if(lastDot==-1) return "";
		string cpp = p.substr(0, lastDot) + ".cpp";
		printf("cpp file is at %s\n", cpp.c_str());
		if(File(cpp).exists()) return cpp;
		return "";
	}
	
	void precompileHeader(const string &headerToPrecompile) {
		
		string cmd = "g++ -std=c++11 -x c++-header -stdlib=libc++ "
			+ liveCodeUtils::includeListToCFlags(includePaths)
			+ " " + headerToPrecompile;
		printf("Precompiling headers: %s\n", cmd.c_str());
		liveCodeUtils::execute(cmd);
	}
	
	
private:
	string lastErrorStr;
	void recompile() {
		//printf("Need to recompile %s\n", path.c_str());
		float t = liveCodeUtils::getSeconds();

		string dylibPath = cc();
		if(dylibPath!="") {
			
			auto *obj = loadDylib(dylibPath);
			if(obj != nullptr) {
				printf("\xE2\x9C\x85\xE2\x9C\x85\xE2\x9C\x85 Success loading \xE2\x9C\x85\xE2\x9C\x85\xE2\x9C\x85\n");
				printf("Compile took %.0fms\n", (liveCodeUtils::getSeconds() - t)*1000.f);
				if(reloaded) reloaded(obj);
			} else {
				printf("\xE2\x9D\x8C\xE2\x9D\x8C\xE2\x9D\x8C Error: No dice loading dylib \xE2\x9D\x8C\xE2\x9D\x8C\xE2\x9D\x8C\n");
				if(reloadFailed) reloadFailed("Couldn't read dylib\n");
			}

		} else {
			if(reloadFailed) reloadFailed(lastErrorStr);
		}
	}
	
	string getAllIncludes() {
		string userIncludes = liveCodeUtils::includeListToCFlags(includePaths);
		return " -I. "//getAllIncludes(string(SRCROOT))
		+ userIncludes
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
		
		string prefixHeaderFlag = "";
		if(prefixHeader!="") {
			prefixHeaderFlag = "-include " + prefixHeader + " ";
		}
		
		string cmd = "g++ -g -std=c++11 -stdlib=libc++ "+prefixHeaderFlag+" -Wno-deprecated-declarations -c " + cppFile + " -o " + objFile + " "
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
	
	
	
	
	void makeCppFile(string path, string objName) {
		ofstream outFile(path.c_str());
		
		outFile << "#include \""+objName+".h\"\n\n";
		outFile << "extern \"C\" {\n\n";
		outFile << "\n\nvoid *getPluginPtr() {return new "+objName+"(); };\n\n";
		if(cppFile!="") {
			// include the contents of the cpp file if it exists
			ifstream inFile(cppFile);
			string line;
			if (inFile.is_open()) {
			  while ( getline (inFile,line) ) {
				outFile << line << '\n';
			  }
			  inFile.close();
			} else {
				printf("Error reading %s\n", cppFile.c_str());
			}
		}
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
	
	T *loadDylib(string dylibPath) {

		if(dylib.isOpen()) {
			if(willCloseDylib) willCloseDylib();
			dylib.close();
		}

		if(!dylib.open(dylibPath)) {
			return nullptr;
		}
		return (T*) dylib.get("getPluginPtr");
	}

	
	/////////////////////////////////////////////////////////
	
	
	string findFile(string file) {
		string f = file;
		if(File(f).exists()) return f;
		f = "src/" + f;
		if(File(f).exists()) return f;
		f = "../" + f;
		if(File(f).exists()) return f;
		//f = string(SRCROOT) + "/" + file;
		//if(fileExists(f)) return f;
		f = Directory::cwd() + "/" + file;
		if(File(f).exists()) return f;
		printf("Error: can't find source file %s\n", file.c_str());
		return f;
	}
};
