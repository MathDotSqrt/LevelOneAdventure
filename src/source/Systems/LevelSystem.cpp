
#include "Systems/LevelSystem.h"
#include "Engine.h"
#include "Util/TransformUtil.h"
#include "Components.h"
#include "Window.h"

#include <iostream>
#include <fstream>


using namespace LOA;
using namespace LOA::Systems;

ID loadRoom(Engine &engine, entt::id_type model, entt::id_type tex, glm::ivec2 loc, int rot) {
	const glm::vec3 size(3);

	auto& scene = engine.getScene();

	Graphics::DissolveMaterial material;
	material.diffuse = tex;
	ID id = scene.addInstance(model, material);

	Graphics::Instance& instance = scene.getInstance(id);
	glm::vec3 pos(-size.x * loc.x, 0, -size.z * loc.y);
	glm::quat rotation = glm::angleAxis(glm::pi<float>() * rot / 2.0f, glm::vec3(0, 1, 0));
	instance.transform = Util::make_transform(pos, rotation);
	
	auto& registry = engine.getRegistry();
	auto entity = registry.create();
	registry.emplace<Component::Dissolve>(entity, 0.0f, .05f, glm::vec3(.7f, .4f, .1f));
	registry.emplace<Component::Renderable>(entity, id);
	
	return id;
}

void load_assets(Engine &engine) {
	auto& scene = engine.getScene();

}

void LevelSystem::init(Engine &engine) {
	using namespace entt;

	std::string rel = "./res/models/dungeon_assets/";

	auto& scene = engine.getScene();
	
	scene.loadTEX("dungeon_pallet"_hs, rel + "dungeon-texture.png");
	
	glm::vec3 scale(.1);
	glm::vec3 floor_offset(0, -.2 - .05f / 2, 0);
	glm::vec3 no_offset(0);

	glm::vec3 wall_offset(3 / 2.0f, 0, -3/2.0f - .05f);
	scene.loadMesh("floor_stone1"_hs, rel + "dungeon/floor-stone1.fbx", floor_offset, scale);
	scene.loadMesh("floor_stone2"_hs, rel + "dungeon/floor-stone2.fbx", floor_offset, scale);

	scene.loadMesh("pillar_1"_hs, rel + "dungeon/pillar-1.fbx", no_offset, scale);

	scene.loadMesh("wall_1"_hs, rel + "dungeon/wall-1.fbx", wall_offset, scale);
	scene.loadMesh("wall_3"_hs, rel + "dungeon/wall-3.fbx", wall_offset, scale);

	scene.loadMesh("door_left"_hs, rel + "dungeon/door-left.fbx", wall_offset, scale);
	scene.loadMesh("door_right"_hs, rel + "dungeon/door-right.fbx", wall_offset, scale);

	
	//walls
	{
		loadRoom(engine, "wall_1"_hs, "dungeon_pallet"_hs, glm::ivec2(0, 0), 2);
		loadRoom(engine, "wall_3"_hs, "dungeon_pallet"_hs, glm::ivec2(0, 1), 3);
		loadRoom(engine, "door_left"_hs, "dungeon_pallet"_hs, glm::ivec2(2, 0), 2);
		loadRoom(engine, "door_right"_hs, "dungeon_pallet"_hs, glm::ivec2(3, 0), 2);
	}
	
	//floor
	{
		loadRoom(engine, "floor_stone1"_hs, "dungeon_pallet"_hs, glm::ivec2(0, 0), 0);
		loadRoom(engine, "floor_stone1"_hs, "dungeon_pallet"_hs, glm::ivec2(1, 0), 0);
		loadRoom(engine, "floor_stone2"_hs, "dungeon_pallet"_hs, glm::ivec2(1, 1), 0);
		loadRoom(engine, "floor_stone1"_hs, "dungeon_pallet"_hs, glm::ivec2(0, 1), 0);
	}
	
	//pillar
	{
		loadRoom(engine, "pillar_1"_hs, "dungeon_pallet"_hs, glm::ivec2(0, 1), 0);
	}

	//for (int i = -10; i <= 10; i++) {
	//	for (int j = -10; j <= 10; j++) {
	//		auto room = (7 * i +  j * j) % 2 == 0 ? "floor_stone2"_hs : "floor_stone1"_hs;
	//		loadRoom(engine, room, "dungeon_pallet"_hs, glm::ivec2(i, j), 0);
	//	}
	//}


}

void LevelSystem::update(Engine& engine, float delta) {
	auto& registry = engine.getRegistry();

	auto view = registry.view<Component::Dissolve>();
	for (auto entity : view) {
		if (Window::getInstance().isPressed('e'))
			view.get<Component::Dissolve>(entity).time += delta / 2.0f;
		else if(Window::getInstance().isPressed('q'))
			view.get<Component::Dissolve>(entity).time -= delta / 2.0f;
	}
}