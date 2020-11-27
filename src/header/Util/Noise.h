#pragma once

#include "common.h"
#include <vector>

namespace LOA::Util {

	std::vector<float> gen_simplex_3D_texture(u64 width, float scale = 1);
	std::vector<float> gen_perlin_3D_texture(u64 width, float scale=1);
}