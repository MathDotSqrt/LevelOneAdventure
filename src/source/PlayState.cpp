#include "PlayState.h"
#include "Systems/MovementSystem.h"
#include "Systems/RenderSystem.h"
#include "Components.h"

using namespace LOA;

PlayState::PlayState() {
	using namespace entt;
	using namespace Component;

	auto& scene = engine.getScene();

	scene.loadMesh("room"_hs, "res/models/demo_room/room.xobj");
	scene.loadTEX("room_diffuse"_hs, "res/models/demo_room/room.png");

	Graphics::BasicLitMaterial room_material;
	room_material.diffuse = "room_diffuse"_hs;
	auto instance_id = scene.addInstance("room"_hs, room_material);

	scene.addPointLight(Graphics::PointLight{ glm::vec3(0, 0, 0), glm::vec3(.1, .2, 1), 30 });
	scene.addPointLight(Graphics::PointLight{glm::vec3(0, 5, -10), glm::vec3(1, .3, .2), 30});


	auto& registry = engine.getRegistry();

	entt::entity room = registry.create();
	registry.emplace<Transformation>(room, glm::vec3(0, -5, -10));
	registry.emplace<Velocity>(room, glm::vec3(0, .1f, 0));
	registry.emplace<Renderable>(room, instance_id);
	
	engine.addSystem<Systems::MovementSystem>();
	engine.addSystem<Systems::RenderSystem>();

}

void PlayState::update(float dt) {
	
	engine.update(dt);
}

void PlayState::render() {
	engine.render();
}