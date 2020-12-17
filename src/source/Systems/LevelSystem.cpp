
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
static std::string tiles = "./res/scene/scene_tiles.yaml";
static i64 last_time = 0;
static i64 last_time_tiles = 0;

void loadRoom(Engine &engine, entt::id_type model, glm::ivec2 loc, int rot) {
	using namespace entt;

	const glm::vec3 size(3);

	auto& scene = engine.getScene();

	//create instance
	LOA::ID id = scene.addInstance(model);
	auto& instance = scene.getInstance(id);
	glm::vec3 pos(-size.x * loc.x, 0, -size.z * loc.y);
	glm::quat rotation = glm::angleAxis(glm::pi<float>() * rot / 2.0f, glm::vec3(0, 1, 0));
	instance.transform = Util::make_transform(pos, rotation);

	Graphics::DissolveMaterial material;
	material.diffuse = "dungeon_pallet"_hs;
	material.dissolve_color = glm::vec3(.7f, .4f, .1f);
	material.offset = .05f;
	material.time = 0.0f;

	auto& registry = engine.getRegistry();
	auto entity = registry.create();
	registry.emplace<Graphics::DissolveMaterial>(entity, material);
	registry.emplace<Component::Renderable>(entity, id);
	registry.emplace<Component::LevelTile>(entity);
}

void erase_char(std::string& str, char c) {
	std::string::iterator end_pos = std::remove(str.begin(), str.end(), c);
	str.erase(end_pos, str.end());
}

bool read_int(c4::csubstr buffer, int& f) {
	size_t pos = c4::unformat(buffer, "{}", f);
	return pos != c4::csubstr::npos;
}

bool read_float(c4::csubstr buffer, float &f) {
	size_t pos = c4::unformat(buffer, "{}", f);
	return pos != c4::csubstr::npos;
}

bool read_vec3(c4::csubstr buffer, glm::vec3 &vec) {
	size_t pos = c4::unformat(buffer, "glm::vec3({}, {}, {})", vec.x, vec.y, vec.z);
	return pos != c4::csubstr::npos;
}

bool read_string(c4::csubstr buffer, std::string& str) {
	c4::from_chars(buffer, &str);
	return true;
}

void LevelSystem::loadAssets() {
	try {
		auto& scene = engine.getScene();
		auto file_str = LOA::Util::read_file(asset);
		if (file_str.has_value()) {
			std::string content = file_str.value();
			erase_char(content, '\r');
			erase_char(content, '\t');

			c4::csubstr src_view(c4::to_csubstr(content.c_str()));
			this->asset_tree = ryml::parse(src_view);

			ryml::NodeRef root = asset_tree.rootref();
			for (ryml::NodeRef& node : root) {
				std::string name = "";
				std::string path = "";
				glm::vec3 offset(0);
				float scale = 1;

				read_string(node.key(), name);
				read_string(node["path"].val(), path);

				read_vec3(node["offset"].val(), offset);
				read_float(node["scale"].val(), scale);

				scene.loadMesh(entt::hashed_string(name.c_str()), path, offset, glm::vec3(scale));
			}
		}

		last_time = Util::last_write(asset).value();
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}

void LevelSystem::loadTiles() {
	try {
		auto& scene = engine.getScene();
		auto file_str = LOA::Util::read_file(tiles);
		if (file_str.has_value()) {
			std::string content = file_str.value();
			erase_char(content, '\r');
			erase_char(content, '\t');

			c4::csubstr src_view(c4::to_csubstr(content.c_str()));
			ryml::Tree tree = ryml::parse(src_view);

			ryml::NodeRef root = tree.rootref();
			ryml::NodeRef tiles = root["tiles"];
			for (ryml::NodeRef& node : tiles) {
				std::string tileType;
				int rot;

				read_string(node["tile"].val(), tileType);
				read_int(node["rot"].val(), rot);



				for (ryml::NodeRef& loc : node["locations"]) {
					int x, z;
					read_int(loc[0].val(), x);
					read_int(loc[1].val(), z);

					glm::ivec2 l(x, z);
					loadRoom(engine, entt::hashed_string(tileType.c_str()), l, rot);
				}
			}
		}

		last_time_tiles = Util::last_write(asset).value();
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}

void LevelSystem::init() {
	using namespace entt;

	auto& scene = engine.getScene();
	scene.loadTEX("dungeon_pallet"_hs,"./res/models/dungeon_assets/dungeon-texture.png");
	
	loadAssets();
	loadTiles();

}

void LevelSystem::update(float delta) {

	auto destroy_tiles = [&]() {
		entt::registry& registry = engine.getRegistry();
		auto view = registry.view<Component::LevelTile>();
		registry.destroy(view.begin(), view.end());
	};

	if (i64 time = Util::last_write(asset).value(); time > last_time) {
		destroy_tiles();
		loadAssets();
		loadTiles();
	}
	
	if (i64 time = Util::last_write(tiles).value(); time > last_time_tiles) {
		destroy_tiles();
		loadTiles();
	}

	auto& registry = engine.getRegistry();

	auto view = registry.view<Graphics::DissolveMaterial>();
	for (auto entity : view) {
		if (Window::getInstance().isPressed('e'))
			view.get<Graphics::DissolveMaterial>(entity).time += delta / 2.0f;
		else if(Window::getInstance().isPressed('q'))
			view.get<Graphics::DissolveMaterial>(entity).time -= delta / 2.0f;
	}
}