#include "Systems/ParticleSystem.h"
#include "Engine.h"
#include "entt/entt.hpp"
#include "Components.h"
#include "Util/Timer.h"

//Design: 
//Many potential generators: compute vel and pos given component transform
//One update function for entire particle set

using namespace LOA::Systems;

ParticleSystem::ParticleSystem(LOA::Engine &engine) : BaseSystem(engine), fireInstanceID(LOA::NullID), fireGenerator(1000000){

}

void ParticleSystem::init() {
	using namespace entt;

	Graphics::Scene &scene = engine.getScene();
	auto settings = Graphics::TEX::Builder().linear().rgba();
	scene.loadTEX("FireParticle"_hs, settings, "./res/textures/firebit2.png");

	Graphics::FireParticleMaterial material;
	material.diffuse_grid = "FireParticle"_hs;
	fireInstanceID = scene.createParticleInstance(fireGenerator.getMax(), material);
}



void ParticleSystem::update(float dt) {
	using namespace Component;

	Util::Timer timer("ParticleSystem");

	auto& registry = engine.getRegistry();

	//Check for collisions
	{
		auto view = registry.view<FireParticle, HitBox>();
		for (auto entity : view) {
			auto& particle = view.get<FireParticle>(entity);
			auto& hitbox = view.get<HitBox>(entity);
			if (hitbox.events.size() > 0) {
				//todo queue all entity deletions
				engine.deleteEntity(entity);
			}
		}

	}

	//Spawns particles
	{
		auto view = registry.view<FireParticle, Transformation>();
		for (auto entity : view) {
			auto& particle = view.get<FireParticle>(entity);
			auto& transform = view.get<Transformation>(entity);

			fireGenerator.genParticles(particle.spawn_rate * dt, transform.pos);
			particle.life_time -= dt;
			if (particle.life_time < 0) {
				engine.deleteEntity(entity);
			}
		}

	}

	//Updates particles
	{
		auto view = registry.view<Component::Camera, Component::Transformation>();
		glm::vec3 camera_pos(0);
		if (view.begin() != view.end()) {	//if there exists a camera with a transform
			entt::entity entity = view.front();
			auto& transform = view.get<Component::Transformation>(entity);
			camera_pos = transform.pos;
		}

		//Sorts the particles by distance from camera
		fireGenerator.update(camera_pos, dt);

		//uploads particle simulation to renderer every frame
		auto& system = engine.getScene().getParticleSystemInstance(fireInstanceID);
		system.data.streamData(fireGenerator.getRenderData());
	}
}

