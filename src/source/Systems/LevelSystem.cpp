
#include "Systems/LevelSystem.h"
#include "Engine.h"
#include "Util/TransformUtil.h"
#include "Components.h"
#include "Window.h"

#include <ryml.hpp>
#include <c4/charconv.hpp>
#include <c4/format.hpp>
#include <c4/std/string.hpp>

#include "util/FileUtil.h"

using namespace LOA;
using namespace LOA::Systems;

static std::string asset = "./res/scene/dungeon_asset.yaml";
static i64 last_time = 0;

ID loadRoom(Engine &engine, entt::id_type model, glm::ivec2 loc, int rot) {
	using namespace entt;

	const glm::vec3 size(3);

	auto& scene = engine.getScene();

	Graphics::DissolveMaterial material;
	material.diffuse = "dungeon_pallet"_hs;
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

void erase_char(std::string& str, char c) {
	std::string::iterator end_pos = std::remove(str.begin(), str.end(), c);
	str.erase(end_pos, str.end());
}

bool read_float(c4::csubstr buffer, float &f) {
	size_t pos = c4::unformat(buffer, "{}", f);
	return pos != c4::csubstr::npos;
}

bool read_vec3(c4::csubstr buffer, glm::vec3 &vec) {
	size_t pos = c4::unformat(buffer, "glm::vec3({}, {}, {})", vec.x, vec.y, vec.z);
	return pos != c4::csubstr::npos;
}

void load_assets(Engine &engine) {
	auto& scene = engine.getScene();
	auto file_str = LOA::Util::read_file(asset);
	if (file_str.has_value()) {
		std::string content = file_str.value();
		erase_char(content, '\r');
		erase_char(content, '\t');

		c4::csubstr src_view(c4::to_csubstr(content.c_str()));
		ryml::Tree tree = ryml::parse(src_view);
		
		ryml::NodeRef root = tree.rootref();
		for (ryml::NodeRef &node : root) {
			std::string name = "";
			std::string path = "";
			glm::vec3 offset(0);
			float scale = 1;

			c4::from_chars(node.key(), &name);
			c4::from_chars(node["path"].val(), &path);

			read_vec3(node["offset"].val(), offset);
			read_float(node["scale"].val(), scale);
			
			scene.loadMesh(entt::hashed_string(name.c_str()), path, offset, glm::vec3(scale));
		}
	}

	last_time = Util::last_write(asset).value();

}

void LevelSystem::init() {
	using namespace entt;

	auto& scene = engine.getScene();
	scene.loadTEX("dungeon_pallet"_hs,"./res/models/dungeon_assets/dungeon-texture.png");
	
	load_assets(engine);

	//walls
	{
		loadRoom(engine, "wall_1"_hs, glm::ivec2(0, 0), 2);
		loadRoom(engine, "wall_3"_hs, glm::ivec2(0, 1), 3);
	}
	
	//floor
	{
		loadRoom(engine, "floor_stone1"_hs, glm::ivec2(0, 0), 0);
		loadRoom(engine, "floor_stone1"_hs, glm::ivec2(1, 0), 0);
		loadRoom(engine, "floor_stone2"_hs, glm::ivec2(1, 1), 0);
		loadRoom(engine, "floor_stone1"_hs, glm::ivec2(0, 1), 0);
	}
	
	//pillar
	{
		loadRoom(engine, "pillar_1"_hs, glm::ivec2(0, 1), 0);
	}

}

void LevelSystem::update(float delta) {

	if (i64 time = Util::last_write(asset).value(); time > last_time) {
		load_assets(engine);
	}
	
	auto& registry = engine.getRegistry();

	auto view = registry.view<Component::Dissolve>();
	for (auto entity : view) {
		if (Window::getInstance().isPressed('e'))
			view.get<Component::Dissolve>(entity).time += delta / 2.0f;
		else if(Window::getInstance().isPressed('q'))
			view.get<Component::Dissolve>(entity).time -= delta / 2.0f;
	}
}