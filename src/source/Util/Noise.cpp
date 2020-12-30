#include "Util/Noise.h"
#include <glm/gtc/noise.hpp>
#include <glm/glm.hpp>
#include <random>
#include <assert.h>

//TODO: maybe make this constexpr
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

//TODO: maybe make this constexpr
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

//TODO: maybe make this constexpr
std::vector<glm::vec3> LOA::Util::gen_ssao_kernel(size_t num_kernels) {
	//I fucking hate c++
	static std::mt19937 rng{ std::random_device()() };
	assert(num_kernels > 0);
	
	std::uniform_real_distribution<float> U_x(-1, 1);
	std::uniform_real_distribution<float> U_y(-1, 1);
	std::uniform_real_distribution<float> U_z(0, 1);

	std::vector<glm::vec3> samples;
	for (size_t i = 0; i < num_kernels; i++) {
		glm::vec3 sample{
			U_x(rng),
			U_y(rng),
			U_z(rng)
		};
		sample = glm::normalize(sample);

		//reject close to parallel rays
		//https://mtnphil.wordpress.com/2013/06/26/know-your-ssao-artifacts/
		if (glm::dot(sample, glm::vec3(0, 0, 1)) < .15) {
			i--;
			continue;
		}

		float scale = (float)i / num_kernels;
		scale = glm::mix(.1f, 1.f, scale * scale);
		sample *= scale;

		samples.push_back(sample);
	}

	return samples;
}

std::vector<glm::vec3> LOA::Util::gen_ssao_rotation(size_t width) {
	static std::mt19937 rng{ std::random_device()() };
	assert(width > 0);

	std::uniform_real_distribution<float> U_x(-1, 1);
	std::uniform_real_distribution<float> U_y(-1, 1);

	std::vector<glm::vec3> rotations;
		
	for (size_t i = 0; i < width * width; i++) {
		glm::vec3 noise{
			U_x(rng),
			U_y(rng),
			0
		};

		rotations.push_back(noise);
	}

	return rotations;
}