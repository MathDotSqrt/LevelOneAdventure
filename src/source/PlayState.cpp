#include "PlayState.h"
#include "Systems/LevelSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/VelocitySystem.h"
#include "Systems/RenderSystem.h"
#include "Components.h"
#include "Window.h"

#include "Graphics/ParticleGenerator.h"

using namespace LOA;

PlayState::PlayState() : generator(20000){
	using namespace entt;
	using namespace Component;

	auto& scene = engine.getScene();

	scene.addPointLight(Graphics::PointLight{ glm::vec3(0, 0, 0), glm::vec3(.1, .2, 1), 10 });
	scene.addPointLight(Graphics::PointLight{ glm::vec3(0, 5, -10), glm::vec3(1, .3, .2), 5 });

	auto& registry = engine.getRegistry();

	//Camera 
	{
		ID camera_light = scene.addPointLight(Graphics::PointLight{});

		entt::entity camera = registry.create();
		registry.emplace<Transformation>(camera, glm::vec3(0, 0, 0));
		registry.emplace<Velocity>(camera, glm::vec3(0, 0, 0));
		registry.emplace<Camera>(camera, glm::radians(80.0f), 1.0f, .01f, 1000.0f);
		registry.emplace<Direction>(camera, glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		registry.emplace<MovementState>(camera);
		registry.emplace<PointLight>(camera, camera_light, glm::vec3(1, .3, .2) * .4f, 3.0f);
		registry.emplace<Input>(camera);
	}
	
	{
		Graphics::ParticleMaterial material{0};
		id = scene.createParticleInstance(generator.getMax(), material);
	}

	engine.addSystem<Systems::LevelSystem>();
	engine.addSystem<Systems::InputSystem>();
	engine.addSystem<Systems::MovementSystem>();
	engine.addSystem<Systems::VelocitySystem>();
	engine.addSystem<Systems::RenderSystem>();

}

void PlayState::update(float dt) {
	engine.update(dt);
	
	auto camera = engine.getRegistry().view<Component::Camera>()[0];
	glm::vec3 pos = engine.getRegistry().get<Component::Transformation>(camera).pos;;

	auto& window = Window::getInstance();
	if (!window.isPressed('p')) {
		generator.genParticles(20);
		generator.update(pos, dt / 4);
	}
}

void PlayState::render() {	
	auto& system = engine.getScene().getParticleSystemInstance(id);
	system.data.streamData(generator.getRenderData());
	engine.render();
}