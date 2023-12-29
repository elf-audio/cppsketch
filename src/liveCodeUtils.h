//
//  glue.h
//  example-simple
//
//  Created by Marek Bereza on 16/10/2019.
//

#pragma once

#include <string>
#include <set>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

namespace liveCodeUtils {
	void init();
	std::string execute(std::string cmd, int *outExitCode = nullptr);
	float getSeconds();
	std::string getCwd();

	// this lists all directories and their subdirectories
	std::vector<fs::path> getAllDirectories(std::string baseDir);
	std::string includeListToCFlags(const std::vector<std::string> &includes);

	// this returns a list of paths to all .h files in the baseDir
	std::vector<std::string> getAllHeaderFiles(std::string baseDir);
}; // namespace liveCodeUtils
