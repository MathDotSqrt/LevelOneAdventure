#include "PlayState.h"
#include "Systems/LevelSystem.h"
#include "Systems/HealthSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ParticleSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/VelocitySystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/ShaderSystem.h"
#include "Components.h"
#include "Window.h"
#include "Systems/MouseCast.h"
#include "Systems/AISystem.h"

#include "Graphics/ParticleGenerator.h"
#include "Graphics/TEX.h"
#include "Graphics/GeometryBuilder.h"

#include "Util/Timer.h"
#include "Physics/PhysicsScene.h"

using namespace LOA;

PlayState::PlayState(){
	using namespace entt;
	using namespace Component;

	auto& scene = engine.getScene();

	scene.loadMesh("sphere"_hs, "./res/models/basic/uv_sphere.fbx");
	scene.loadRawMesh("cube"_hs, Graphics::gen_cube(1));
	//scene.loadMesh("dragon"_hs, "./res/models/dragon/dragon2.stl");

	scene.loadTEX("uv_debug_grid"_hs, Graphics::TEX::Builder().mipmapLinear(), "./res/textures/uv_grid.jpg");


	auto& registry = engine.getRegistry();

	//Game Code
	engine.addSystem<Systems::InputSystem>();
	engine.addSystem<Systems::AISystem>();
	engine.addSystem<Systems::MovementSystem>();
	engine.addSystem<Systems::HealthSystem>();

	//Rendering/Physics updating
	engine.addSystem<Systems::ParticleSystem>();
	engine.addSystem<Systems::RenderSystem>();
	engine.addSystem<Systems::PhysicsSystem>();
	engine.addSystem<Systems::MouseCast>();

	//Utility stuff
	engine.addSystem<Systems::LevelSystem>();
	engine.addSystem<Systems::ShaderSystem>();
	

	entt::entity player = engine.getPlayer();
	entt::entity camera = engine.getMainCamera();

	//Scene set up
	{
		scene.setAmbientLight(Graphics::AmbientLight{glm::vec3(1), .3f});
		scene.setDirLight(Graphics::DirLight{glm::vec3(1, .9, .6), glm::vec3(1, -1, -.1), .05f});
	}

	//Fire
	{
		ID point_light = scene.addPointLight(Graphics::PointLight{});
		entt::entity fire = registry.create();
		registry.emplace<Transformation>(fire, glm::vec3(17, 6.5, 15), glm::angleAxis(glm::pi<float>() / 4, glm::vec3(0, 0, 1)));
		registry.emplace<PointLight>(fire, point_light, glm::vec3(.5, .3, .1), 5.0f, 5.0f);
		registry.emplace<FireParticle>(fire, 200.0f, 1000.0f);
	}

	//Player
	{
		Graphics::DissolveMaterial material;
		material.color = glm::vec3(1, 1, 1);
		material.texture = "uv_debug_grid"_hs;
		material.offset = .01f;
		material.dissolve_color = glm::vec3(1, .8, .4);

		ID cubeID = scene.addInstance("cube"_hs, material);

		ID point_light = scene.addPointLight(Graphics::PointLight{});

		registry.emplace<Transformation>(player, glm::vec3(0, 0, 12));
		registry.emplace<Velocity>(player, glm::vec3(0, 0, 0));
		registry.emplace<Direction>(player, glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		registry.emplace<MovementState>(player);
		registry.emplace<Input>(player);
		registry.emplace<Renderable>(player, cubeID);
		registry.emplace<PointLight>(player, point_light, glm::vec3(.7, .6, .5), 1.f, 10.0f);
		registry.emplace<CharacterController>(player);
		registry.emplace<HitBox>(player, EventType::CHARACTER, glm::vec3(.5));
		registry.emplace<Graphics::DissolveMaterial>(player, material);
		//registry.emplace<HealthComponent>(player);

	}

	//Camera 
	{
		ID camera_light = scene.addPointLight(Graphics::PointLight{});

		registry.emplace<Transformation>(camera, glm::vec3(-2.5, 10, 10), glm::angleAxis(glm::pi<float>() / 4, glm::vec3(-1, 0, 0)));
		registry.emplace<Velocity>(camera, glm::vec3(0, 0, 0));
		registry.emplace<Camera>(camera, glm::radians(80.0f), 1.0f, .01f, 1000.0f, player);
		registry.emplace<Direction>(camera, glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		//registry.emplace<MovementState>(camera);
		registry.emplace<PointLight>(camera, camera_light, glm::vec3(.7, .6, .5), 1.0f, 10.0f);
		registry.emplace<Input>(camera);
	}
	
	
	engine.initSystems();
}

void PlayState::update(float dt) {
	auto& window = Window::getInstance();
	auto& registry = engine.getRegistry();

	entt::entity player = engine.getPlayer();
	entt::entity camera = engine.getMainCamera();

	if (window.isPressed('t')) {
		if (registry.has<Component::MovementState>(camera)) {
			registry.remove<Component::MovementState>(camera);
			registry.emplace<Component::MovementState>(player);
		}
		else{
			registry.emplace<Component::MovementState>(camera);
			registry.remove<Component::MovementState>(player);
		}
	}

	engine.update(dt);

}

void PlayState::render() {	
	engine.render();
}