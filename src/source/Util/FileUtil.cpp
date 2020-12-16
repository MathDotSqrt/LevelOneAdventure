#include "Util/FileUtil.h"

#include <fstream>
#include <sstream>
#include <iostream>

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