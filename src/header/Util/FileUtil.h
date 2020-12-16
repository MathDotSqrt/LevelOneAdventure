#pragma once
#include <string>
#include <optional>

namespace LOA::Util {
	std::optional<std::string> read_file(const std::string &path);
}