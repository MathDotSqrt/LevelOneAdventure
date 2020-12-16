#include "Util/FileUtil.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

using namespace LOA::Util;

std::optional<std::string> LOA::Util::read_file(const std::string &path) {
	std::ifstream reader(path);
	if (reader) {
		std::stringstream buffer;
		buffer << reader.rdbuf();
		std::string file = buffer.str();
		return file;
	}
	else {
		std::cerr << "ERROR: Could not find file: " << path << "\n";
		return {};
	}
}

std::optional<i64> LOA::Util::last_write(const std::string& path) {
	struct stat result;
	if (stat(path.c_str(), &result) == 0) {
		auto new_time = result.st_mtime;

		return new_time;
	}

	return {};
}