#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

#include "Graphics/ParticleRenderData.h"

namespace LOA::Graphics {
	struct Particle {
		glm::vec3 pos;
		glm::vec3 vel;
		glm::u8vec4 color;
		f32 index;
		float size;
		float angle;
		float life;
		float camera_dist;
	};

	class ParticleGenerator {
	public:
		ParticleGenerator(size_t max_particles);

		//not to be called after update
		void genParticles(float num, glm::vec3 transform = glm::vec3(0));
		void update(glm::vec3 camera_pos, float delta);

		const std::vector<RenderData>& getRenderData() const;
		size_t getMax() const;
	private:
		float accum = 0;
		size_t max_particles;
		std::vector<Particle> particles;
		std::vector<RenderData> renderData;
	};
}