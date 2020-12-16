#include "Systems/ParticleSystem.h"
#include "Engine.h"
#include "entt/entt.hpp"
#include "Components.h"
#include "Util/Timer.h"

//Design: 
//Many potential generators: compute vel and pos given component transform
//One update function for entire particle set

using namespace LOA::Systems;

ParticleSystem::ParticleSystem(LOA::Engine &engine) : BaseSystem(engine), fireInstanceID(LOA::NullID), fireGenerator(10000){

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

	//Spawns particles
	{
		auto view = registry.view<FireParticle, Transformation>();
		for (auto entity : view) {
			auto& particle = view.get<FireParticle>(entity);
			auto& transform = view.get<Transformation>(entity);

			fireGenerator.genParticles(particle.spawn_rate * dt, transform.pos);
			particle.life_time -= dt;
			if (particle.life_time < 0) {
				//ok to destroy current entity when iterating
				registry.destroy(entity);
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

