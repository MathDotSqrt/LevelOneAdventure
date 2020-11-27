#include "Systems/LevelSystem.h"
#include "Engine.h"
#include "Util/TransformUtil.h"
#include "Components.h"
#include "Window.h"

using namespace LOA;
using namespace LOA::Systems;

ID loadRoom(Engine &engine, entt::id_type model, entt::id_type tex, glm::ivec2 loc, int rot) {
	const glm::vec3 size(5, 5, 5);

	auto& scene = engine.getScene();

	//Graphics::BasicLitMaterial material;
	//material.diffuse = tex;

	Graphics::DissolveMaterial material;
	material.diffuse = tex;

	ID id = scene.addInstance(model, material);

	Graphics::Instance& instance = scene.getInstance(id);

	glm::vec3 offset = size * .5f;
	glm::vec3 pos(size.x * loc.x, 0, size.z * loc.y);
	glm::quat rotation = glm::angleAxis(glm::pi<float>() * rot / 2.0f, glm::vec3(0, 1, 0));

	instance.transform = Util::make_transform(pos, rotation);
	
	auto& registry = engine.getRegistry();
	auto entity = registry.create();

	registry.emplace<Component::Dissolve>(entity, 0.0f, .05f, glm::vec3(.3f, .2f, .7f));
	registry.emplace<Component::Renderable>(entity, id);
	//registry.emplace<Component::Transformation>(entity, pos, rotation);
	//registry.emplace<Component::Velocity>(entity, glm::vec3(0, 0, 0));

	return id;
}

void LevelSystem::init(Engine &engine) {
	using namespace entt;

	const glm::vec3 offset(-2.5, -5, 2.5);


	auto& scene = engine.getScene();
	
	scene.loadMesh("demo_room"_hs, "res/models/demo_room/room.xobj", offset);
	scene.loadMesh("boss_room"_hs, "res/models/boss_room/boss_room.xobj", offset);
	scene.loadMesh("thirsty_room"_hs, "res/models/thirsty_room/thirsty_room.xobj", offset);

	scene.loadTEX("boss_diffuse"_hs, "res/models/boss_room/boss_room.png");
	scene.loadTEX("demo_diffuse"_hs, "res/models/demo_room/room.png");
	scene.loadTEX("thirsty_diffuse"_hs, "res/models/thirsty_room/thirsty_room.png");

	loadRoom(engine, "boss_room"_hs, "boss_diffuse"_hs, glm::ivec2(0, 0), 0);
	loadRoom(engine, "boss_room"_hs, "boss_diffuse"_hs, glm::ivec2(-1, 0), 1);
	loadRoom(engine, "boss_room"_hs, "boss_diffuse"_hs, glm::ivec2(-1, 1), 2);
	loadRoom(engine, "thirsty_room"_hs, "thirsty_diffuse"_hs, glm::ivec2(0, 1), 3);
	loadRoom(engine, "thirsty_room"_hs, "thirsty_diffuse"_hs, glm::ivec2(0, 2), 2);
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