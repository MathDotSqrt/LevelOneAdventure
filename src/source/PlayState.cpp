#include "PlayState.h"
#include "Systems/LevelSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ParticleSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/VelocitySystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/ShaderSystem.h"
#include "Components.h"
#include "Window.h"

#include "Graphics/ParticleGenerator.h"
#include "Graphics/TEX.h"
#include "Graphics/GeometryBuilder.h"

#include "Util/Timer.h"

using namespace LOA;

PlayState::PlayState(){
	using namespace entt;
	using namespace Component;

	auto& scene = engine.getScene();

	scene.meshCache.load<Graphics::MeshLoader>("sphere"_hs, "./res/models/basic/uv_sphere.fbx");
	scene.meshCache.load<Graphics::MeshLoader>("cube"_hs, Graphics::gen_cube(1));
	scene.loadTEX("uv_debug_grid"_hs, Graphics::TEX::Builder().mipmapLinear(), "./res/textures/uv_grid.jpg");


	auto& registry = engine.getRegistry();

	engine.addSystem<Systems::ParticleSystem>();

	engine.addSystem<Systems::ShaderSystem>();

	engine.addSystem<Systems::InputSystem>();
	engine.addSystem<Systems::MovementSystem>();
	engine.addSystem<Systems::RenderSystem>();

	engine.addSystem<Systems::PhysicsSystem>();
	engine.addSystem<Systems::VelocitySystem>();

	engine.addSystem<Systems::LevelSystem>();

	//Scene set up
	{
		scene.setAmbientLight(Graphics::AmbientLight{glm::vec3(1), .3f});
		scene.setDirLight(Graphics::DirLight{glm::vec3(1, .9, .6), glm::vec3(1, -1, -.1), .05f});
	}

	//Fire
	{
		ID point_light = scene.addPointLight(Graphics::PointLight{});
		entt::entity fire = registry.create();
		registry.emplace<Transformation>(fire, glm::vec3(0), glm::angleAxis(glm::pi<float>() / 4, glm::vec3(0, 0, 1)));
		registry.emplace<PointLight>(fire, point_light, glm::vec3(.5, .3, .1), 5.0f);
		registry.emplace<FireParticle>(fire, 100.0f);
	}

	

	//Player
	{
		ID point_light = scene.addPointLight(Graphics::PointLight{});
		scene.meshCache.load<Graphics::MeshLoader>("cube"_hs, Graphics::gen_cube(1));
		ID cubeID = scene.addInstance("cube"_hs, Graphics::BasicDeferredMaterial{"uv_debug_grid"_hs});

		player = registry.create();
		registry.emplace<Transformation>(player, glm::vec3(0, 0, 10));
		registry.emplace<Velocity>(player, glm::vec3(0, 0, 0));
		registry.emplace<Direction>(player, glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		registry.emplace<MovementState>(player);
		registry.emplace<Input>(player);
		registry.emplace<Renderable>(player, cubeID);
		registry.emplace<PointLight>(player, point_light, glm::vec3(.7, .6, .5), 1.f, 10.0f);
		registry.emplace<CharacterController>(player);

	}

	//Camera 
	{
		ID camera_light = scene.addPointLight(Graphics::PointLight{});

		camera = registry.create();
		registry.emplace<Transformation>(camera, glm::vec3(-2.5, 10, 10), glm::angleAxis(glm::pi<float>() / 4, glm::vec3(-1, 0, 0)));
		registry.emplace<Velocity>(camera, glm::vec3(0, 0, 0));
		registry.emplace<Camera>(camera, glm::radians(80.0f), 1.0f, .01f, 1000.0f, player);
		registry.emplace<Direction>(camera, glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		//registry.emplace<MovementState>(camera);
		registry.emplace<PointLight>(camera, camera_light, glm::vec3(.7, .6, .5), 1.0f, 10.0f);
		registry.emplace<Input>(camera);
	}
	
	

}

void PlayState::update(float dt) {
	auto& window = Window::getInstance();

	if (window.isPressed('t') && !swap) {
		auto& registry = engine.getRegistry();
		if (registry.has<Component::MovementState>(camera)) {
			registry.remove<Component::MovementState>(camera);
			registry.emplace<Component::MovementState>(player);
		}
		else{
			registry.emplace<Component::MovementState>(camera);
			registry.remove<Component::MovementState>(player);
		}
	}
	swap = window.isPressed('t');

	engine.update(dt);
}

void PlayState::render() {	
	engine.render();
}