#pragma once

#include "common.h"
#include <glm/glm.hpp>
#include <vector>

namespace LOA::Util {
	//TODO: maybe make this constexpr
	std::vector<float> gen_simplex_3D_texture(u64 width, float scale = 1);
	std::vector<float> gen_perlin_3D_texture(u64 width, float scale=1);

	std::vector<glm::vec3> gen_ssao_kernel(size_t num_samples);
	std::vector <glm::vec3> gen_ssao_rotation(size_t width);
}