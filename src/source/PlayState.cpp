#include "PlayState.h"
#include "Systems/LevelSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ParticleSystem.h"
#include "Systems/VelocitySystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/ShaderSystem.h"
#include "Components.h"
#include "Window.h"

#include "Graphics/ParticleGenerator.h"
#include "Graphics/TEX.h"

using namespace LOA;

PlayState::PlayState() : generator(2000){
	using namespace entt;
	using namespace Component;

	auto& scene = engine.getScene();

	auto& registry = engine.getRegistry();

	//Fire
	{
		ID point_light = scene.addPointLight(Graphics::PointLight{});
		entt::entity fire = registry.create();
		registry.emplace<Transformation>(fire, glm::vec3(0), glm::angleAxis(glm::pi<float>() / 4, glm::vec3(0, 0, 1)));
		registry.emplace<PointLight>(fire, point_light, glm::vec3(.5, .3, .1), 2.0f);
		registry.emplace<FireParticle>(fire, 100.0f);
	}

	//Camera 
	{
		ID camera_light = scene.addPointLight(Graphics::PointLight{});

		entt::entity camera = registry.create();
		registry.emplace<Transformation>(camera, glm::vec3(0, 0, 0));
		registry.emplace<Velocity>(camera, glm::vec3(0, 0, 0));
		registry.emplace<Camera>(camera, glm::radians(80.0f), 1.0f, .01f, 1000.0f);
		registry.emplace<Direction>(camera, glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		registry.emplace<MovementState>(camera);
		//registry.emplace<PointLight>(camera, camera_light, glm::vec3(1, .3, .2) * .4f, 1.0f);
		registry.emplace<Input>(camera);
	}
	
	engine.addSystem<Systems::LevelSystem>();
	engine.addSystem<Systems::ParticleSystem>();
	engine.addSystem<Systems::ShaderSystem>();


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