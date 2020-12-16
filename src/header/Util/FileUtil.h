#pragma once
#include <string>
#include <optional>
#include "common.h"

namespace LOA::Util {

	std::optional<std::string> read_file(const std::string &path);

	std::optional<i64> last_write(const std::string& path);

}