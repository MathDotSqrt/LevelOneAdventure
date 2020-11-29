#include "PlayState.h"
#include "Systems/LevelSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/VelocitySystem.h"
#include "Systems/RenderSystem.h"
#include "Components.h"

using namespace LOA;

PlayState::PlayState() {
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
	
	engine.addSystem<Systems::LevelSystem>();
	engine.addSystem<Systems::InputSystem>();
	engine.addSystem<Systems::MovementSystem>();
	engine.addSystem<Systems::VelocitySystem>();
	engine.addSystem<Systems::RenderSystem>();

}

void PlayState::update(float dt) {
	engine.update(dt);
}

void PlayState::render() {
	engine.render();
}