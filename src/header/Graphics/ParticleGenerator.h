#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Graphics/ParticleRenderData.h"

namespace LOA::Graphics {
	struct Particle {
		glm::vec3 pos;
		glm::vec3 vel;
		glm::u8vec4 color;
		float size;
		float angle;
		float life;
		float camera_dist;
	};

	class ParticleGenerator {
	public:
		ParticleGenerator(size_t max_particles);

		//not to be called after update
		void genParticles(int num);
		void update(glm::vec3 camera_pos, float delta);
	private:
		size_t max_particles;
		std::vector<Particle> particles;
		std::vector<RenderData> renderData;
	};
}