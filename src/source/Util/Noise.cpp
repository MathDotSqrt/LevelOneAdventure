#include "Util/Noise.h"
#include <glm/gtc/noise.hpp>
#include <glm/glm.hpp>

std::vector<float> LOA::Util::gen_simplex_3D_texture(u64 width, float scale) {
	std::vector<float> buffer;

	buffer.reserve(width * width * width);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < width; k++) {
				glm::vec3 pos(i, j, k);
				float scaled = glm::simplex(pos * scale) * .5f + .5f;
				buffer.push_back(scaled);
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
				glm::vec3 pos(i + 13, j + 17, k + 19);
				float scaled = (glm::perlin(pos * scale) + .5f);
				buffer.push_back(glm::max(scaled, 0.0f));
			}
		}
	}

	return buffer;
}