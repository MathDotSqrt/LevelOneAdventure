#include "Graphics/ParticleGenerator.h"

#include <glm/gtx/norm.hpp>
#include <random>
#include <assert.h>
#include <algorithm>
#include "common.h"

using namespace LOA::Graphics;


ParticleGenerator::ParticleGenerator(size_t max) : max_particles(max) {
	particles.reserve(max);
	renderData.reserve(max);



}

void ParticleGenerator::genParticles(int num) {
	static std::mt19937 rng(1337);

	assert(num >= 0);
	assert(num < max_particles);
	
	std::uniform_real<float> u_f(-.1f, .1f);
	std::uniform_int<u8> u_i(0, 255);

	for (int i = 0; i < num && particles.size() < max_particles; i++) {
		Particle p;
		p.pos = glm::vec3(u_f(rng), u_f(rng), u_f(rng));
		p.vel = glm::vec3(u_f(rng), u_f(rng), u_f(rng));
		p.color = glm::u8vec4(u_i(rng), u_i(rng), u_i(rng), u_i(rng));
		p.size = .1f;
		p.angle = 0;
		p.life = 1.0f;

		particles.push_back(p);
	}
}

void ParticleGenerator::update(glm::vec3 camera_pos, float delta) {
	for (auto& particle : particles) {
		particle.life -= delta;
		if (particle.life > 0) {
			particle.vel += glm::vec3(0, -9.81, 0) * delta * .5f;
			particle.pos += particle.vel * delta;
			particle.camera_dist = glm::length2(camera_pos - particle.pos);
		}
		else {
			particle.camera_dist = -1;
		}
	}

	auto sort_dist = [](const Particle& l, const Particle& r) {
		return l.camera_dist < r.camera_dist;
	};

	auto find_dead = [](const Particle& p) {
		return p.life <= 0;
	};

	std::sort(particles.begin(), particles.end(), sort_dist);
	auto found = std::find_if(particles.begin(), particles.end(), find_dead);
	particles.erase(found, particles.end());

	renderData.clear();
	for (const auto& p : particles) {
		renderData.push_back({glm::vec4(p.pos, p.size), p.color});
	}
}