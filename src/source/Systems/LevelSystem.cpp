#include "Systems/LevelSystem.h"
#include "Engine.h"
#include "Util/TransformUtil.h"

using namespace LOA;
using namespace LOA::Systems;

ID loadRoom(Graphics::Scene &scene, entt::id_type model, entt::id_type tex, glm::ivec2 loc, int rot) {
	const glm::vec3 size(5, 5, 5);

	Graphics::BasicLitMaterial material;
	material.diffuse = tex;
	ID id = scene.addInstance(model, material);

	Graphics::Instance& instance = scene.getInstance(id);

	glm::vec3 offset = size * .5f;
	glm::vec3 pos(size.x * loc.x, 0, size.z * loc.y);
	glm::quat rotation = glm::angleAxis(glm::pi<float>() * rot / 2.0f, glm::vec3(0, 1, 0));

	instance.transform = Util::make_transform(pos, rotation);

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

	loadRoom(scene, "boss_room"_hs, "boss_diffuse"_hs, glm::ivec2(0, 0), 0);
	loadRoom(scene, "boss_room"_hs, "boss_diffuse"_hs, glm::ivec2(-1, 0), 1);
	loadRoom(scene, "boss_room"_hs, "boss_diffuse"_hs, glm::ivec2(-1, 1), 2);
	loadRoom(scene, "thirsty_room"_hs, "thirsty_diffuse"_hs, glm::ivec2(0, 1), 3);
	loadRoom(scene, "thirsty_room"_hs, "thirsty_diffuse"_hs, glm::ivec2(0, 2), 2);
	
}

void LevelSystem::update(Engine& engine, float delta) {

}