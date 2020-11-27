#include "Util/Noise.h"
#include <glm/gtc/noise.hpp>

std::vector<float> LOA::Util::gen_simplex_3D_texture(u64 width, float scale) {
	std::vector<float> buffer;

	buffer.reserve(width * width * width);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < width; k++) {
				glm::vec3 pos(i, j, k);
				buffer.push_back(glm::simplex(pos * scale));
			}
		}
	}

	return buffer;
}

std::vector<float> LOA::Util::gen_perlin_3D_texture(u64 width, float scale) {
	std::vector<float> buffer;

	buffer.reserve(width * width * width);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < width; k++) {
				glm::vec3 pos(i, j, k);
				buffer.push_back(glm::perlin(pos * scale));
			}
		}
	}

	return buffer;
}