#include "Graphics/ParticleGenerator.h"

#include <glm/gtx/norm.hpp>
#include <assert.h>
#include <random>
#include <algorithm>
#include "common.h"

using namespace LOA::Graphics;


ParticleGenerator::ParticleGenerator(size_t max) : max_particles(max) {
	particles.reserve(max);
	renderData.reserve(max);



}

void ParticleGenerator::genParticles(float num, glm::vec3 pos) {
	static std::mt19937 rng(1337);

	assert(num >= 0);
	assert(num < max_particles);

	accum += num;

	std::exponential_distribution u_Y(.7f);
	std::uniform_real<float> u_X(-.2f, .2f);
	std::uniform_real<float> u_angle(0, 2 * glm::pi<float>());
	std::uniform_int<u8> u_i(0, 255);
	std::uniform_int<i32> u_index(0, 3);

	while (accum > 1 && particles.size() < max_particles) {

		glm::vec3 rand_pos = glm::vec3(u_X(rng), u_X(rng), u_X(rng)) + pos;
		glm::vec3 rand_vel = glm::vec3(u_X(rng) * 2, u_Y(rng), u_X(rng) * 2);
		glm::u8vec4 rand_color = glm::u8vec4(u_i(rng), u_i(rng), u_i(rng), 100);

		Particle p;
		p.pos = rand_pos;
		p.vel = rand_vel;
		p.color = rand_color;
		p.size = 1.0f + u_X(rng);
		p.angle = u_angle(rng);
		p.life = .6f;
		p.index = u_index(rng);

		particles.push_back(p);
		accum -= 1;
	}
}

void ParticleGenerator::update(glm::vec3 camera_pos, float delta) {
	for (auto& particle : particles) {
		particle.life -= delta;
		if (particle.life > 0) {
			particle.vel += glm::vec3(0, .05f, 0) * delta * .5f;
			particle.pos += particle.vel * delta;
			particle.camera_dist = glm::length2(camera_pos - particle.pos);
		}
		else {
			particle.camera_dist = -1;
		}
	}

	auto sort_dist = [](const Particle& l, const Particle& r) {
		return l.camera_dist > r.camera_dist;
	};

	auto find_dead = [](const Particle& p) {
		return p.life <= 0;
	};

	std::sort(particles.begin(), particles.end(), sort_dist);
	auto found = std::find_if(particles.begin(), particles.end(), find_dead);
	particles.erase(found, particles.end());

	renderData.clear();
	for (const auto& p : particles) {
		renderData.push_back({ glm::vec4(p.pos, p.size), p.color, glm::vec2(p.index, p.angle), p.life});
	}
}

const std::vector<RenderData>& ParticleGenerator::getRenderData() const {
	return renderData;
}

size_t ParticleGenerator::getMax() const {
	return max_particles;
}