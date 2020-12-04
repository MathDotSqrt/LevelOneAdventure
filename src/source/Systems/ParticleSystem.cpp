#include "Systems/ParticleSystem.h"
#include "Engine.h"
#include "entt/entt.hpp"
#include "Components.h"

//Design: 
//Many potential generators: compute vel and pos given component transform
//One update function for entire particle set

using namespace LOA::Systems;

ParticleSystem::ParticleSystem() : fireInstanceID(LOA::NullID), fireGenerator(10000){

}

void ParticleSystem::init(Engine& engine) {
	using namespace entt;

	Graphics::Scene &scene = engine.getScene();
	auto settings = Graphics::TEX::Builder().linear().rgba();
	scene.loadTEX("FireParticle"_hs, settings, "./res/textures/FireParticle.png");

	Graphics::FireParticleMaterial material;
	material.diffuse_grid = "FireParticle"_hs;
	fireInstanceID = scene.createParticleInstance(fireGenerator.getMax(), material);
}



void ParticleSystem::update(Engine& engine, float dt) {
	using namespace Component;

	auto& registry = engine.getRegistry();

	//Spawns particles
	{
		auto view = registry.view<FireParticle, Transformation>();

		for (auto entity : view) {
			auto& particle = view.get<FireParticle>(entity);
			auto& pos = view.get<Transformation>(entity).pos;

			fireGenerator.genParticles(particle.spawn_rate * dt, pos);
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

		fireGenerator.update(camera_pos, dt);

		auto& system = engine.getScene().getParticleSystemInstance(fireInstanceID);
		system.data.streamData(fireGenerator.getRenderData());
	}
}

