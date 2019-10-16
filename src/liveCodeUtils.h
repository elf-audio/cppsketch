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

namespace liveCodeUtils {
	void init();
	std::string execute(std::string cmd, int *outExitCode = nullptr);
	float getSeconds();
	std::string getCwd();
	
	// this lists all directories and their subdirectories
	std::vector<std::string> getAllDirectories(std::string baseDir);
	std::string includeListToCFlags(const std::vector<std::string> &includes);
};



class File {
public:
	std::string path;

	File(std::string path = "");

	std::string fileName();

	bool remove();

	bool isDirectory() const;

	bool copy(std::string destinationPath);

	bool exists();

	bool operator < (const File& other) const
	{
		return path.compare(other.path)<0;
	}
	
	std::string getExtension() const;

	// reads the entire file into a string (must be ascii!)
	std::string getText();
	
	// writes whatever you pass in as a string into the file.
	void setText(std::string text);
};



class Directory: public File {
public:
	std::vector<File> files;
	int size();

	Directory(std::string path): File(path) {}
	Directory(File file) : File(file.path) {}
	File &operator[](unsigned int i);

	Directory();
	bool create(bool recursively = false);
	bool list();

	void emptyContents();
	bool contains(std::string fileName);
	
	void sort();
	bool up();
	
	static std::string cwd();
	
	void allowExtension(std::string ext);
	
private:
	void emptyContents(std::string dirPath);
	bool fileExtensionAllowed(const File &file);
	std::set<std::string> allowedExtensions;
	std::string lcase(std::string str);
};

