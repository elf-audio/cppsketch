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
#include <algorithm>
#include <cstring>

using namespace std;
namespace liveCodeUtils {
	std::chrono::system_clock::time_point startTime;
};

void liveCodeUtils::init() {
	liveCodeUtils::startTime = std::chrono::system_clock::now();
}

string liveCodeUtils::execute(string cmd, int *outExitCode) {
#ifndef _WIN32
	printf("Executing %s", cmd.c_str());
	cmd += " 2>&1";
	FILE *pipe = popen(cmd.c_str(), "r");
	if (!pipe) return "ERROR";
	char buffer[128];
	std::string result = "";
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != NULL) result += buffer;
	}

	int exitCode = pclose(pipe);
	if (outExitCode != nullptr) {
		*outExitCode = WEXITSTATUS(exitCode);
	}
	printf("%s\n", result.c_str());
	return result;
#else
	return "Error - can't do this";
#endif
}

float liveCodeUtils::getSeconds() {
	auto end									  = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - liveCodeUtils::startTime;
	return elapsed_seconds.count();
}

vector<fs::path> liveCodeUtils::getAllDirectories(string baseDir) {
	std::vector<fs::path> subdirectories;

	// Check if the given path is a directory
	if (!fs::is_directory(baseDir)) {
		return subdirectories;
	}

	// Iterate over the directory entries recursively
	for (const auto &entry: fs::recursive_directory_iterator(baseDir)) {
		// Check if the entry is a directory
		if (fs::is_directory(entry)) {
			subdirectories.push_back(entry.path());
		}
	}

	return subdirectories;
}

std::vector<std::string> liveCodeUtils::getAllHeaderFiles(std::string baseDir) {
	std::vector<std::string> headerFiles;

	// Check if the given path is a directory
	if (!fs::is_directory(baseDir)) {
		return headerFiles;
	}

	// Iterate over the directory entries recursively
	for (const auto &entry: fs::recursive_directory_iterator(baseDir)) {
		if (entry.is_regular_file()) {
			// Check the file extension
			if (entry.path().extension() == ".h" || entry.path().extension() == ".hpp") {
				headerFiles.push_back(entry.path().string());
			}
		}
	}

	return headerFiles;
}

std::string liveCodeUtils::includeListToCFlags(const std::vector<std::string> &includes) {
	string str;
	for (auto &inc: includes) {
		str += " -I" + inc + " ";
	}
	return str;
}
