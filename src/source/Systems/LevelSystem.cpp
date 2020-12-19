
#include "Systems/LevelSystem.h"
#include "Engine.h"
#include "Util/TransformUtil.h"
#include "Components.h"
#include "Window.h"

#include "Physics/PhysicsScene.h"

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

static const glm::vec3 grid_size(3);


void erase_char(std::string& str, char c) {
	auto end_pos = std::remove(str.begin(), str.end(), c);
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

bool read_vec3(c4::csubstr buffer, glm::vec3& vec) {
	size_t pos = c4::unformat(buffer, "glm::vec3({}, {}, {})", vec.x, vec.y, vec.z);
	if (pos == c4::csubstr::npos) {
		float f = 0;
		pos = c4::unformat(buffer, "glm::vec3({})", f);
		vec = glm::vec3(f);
	}

	return pos != c4::csubstr::npos;
}

bool read_string(c4::csubstr buffer, std::string& str) {
	c4::from_chars(buffer, &str);
	return true;
}

void LevelSystem::init() {
	using namespace entt;
	using namespace Component;

	auto& scene = engine.getScene();
	scene.loadTEX("dungeon_pallet"_hs,"./res/models/dungeon_assets/dungeon-texture.png");
	
	loadAssets();
	loadTiles();


	auto& registry = engine.getRegistry();
	auto create_builder = [&](glm::vec3 pos) {
		ID id = scene.addInstance(assets_map[0], Graphics::TranslucentBasicMaterial{ "dungeon_pallet"_hs, .75f });
		entt::entity builder = registry.create();
		registry.emplace<Transformation>(builder, pos);
		registry.emplace<Renderable>(builder, id);
		registry.emplace<LevelBuilder>(builder, 0);
	};
	create_builder(glm::vec3(0));
	//create_builder(glm::vec3(3, 0, 0));
	//create_builder(glm::vec3(0, 0, 3));
	//create_builder(glm::vec3(3, 0, 3));
	

}

void LevelSystem::update(float delta) {
	using namespace entt;

	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	auto destroy_tiles = [&]() {
		assets_map.clear();
		auto view = registry.view<Component::LevelTile>();
		registry.destroy(view.begin(), view.end());
	};

	auto replace_instance = [&](entt::entity entity, entt::id_type new_mesh_id) {
		ID id = scene.addInstance(new_mesh_id, Graphics::TranslucentBasicMaterial{ "dungeon_pallet"_hs, .75f });
		auto& renderable = registry.get<Component::Renderable>(entity);
		scene.getInstance(id).transform = scene.getInstance(renderable.instance_id).transform;
		scene.removeInstance(renderable.instance_id);
		renderable.instance_id = id;
	};

	auto last_write = Util::last_write(asset);
	if (last_write.has_value() && last_write.value() > last_time) {
		destroy_tiles();
		loadAssets();
		loadTiles();
	}
	
	last_write = Util::last_write(tiles);
	if (last_write.has_value() && last_write.value() > last_time_tiles) {
		destroy_tiles();
		loadTiles();
	}

	auto& window = Window::getInstance();
	auto builder_view = registry.view<Component::LevelBuilder, Component::Transformation>();

	int scroll_delta = window.getScrollDelta();

	if (scroll_delta != 0) {
		model_index += scroll_delta;
		model_index = ((model_index + assets_map.size()) % assets_map.size());
	}

	for (auto entity : builder_view) {
		auto& transform = registry.get<Component::Transformation>(entity);

		glm::vec3 move(0);
		move.x -= window.isPressed(Window::Keys::RIGHT_ARROW) ? 1 : 0;
		move.x += window.isPressed(Window::Keys::LEFT_ARROW) ? 1 : 0;
		move.z += window.isPressed(Window::Keys::UP_ARROW) ? 1 : 0;
		move.z -= window.isPressed(Window::Keys::DOWN_ARROW) ? 1 : 0;
		
		if (scroll_delta != 0) {
			replace_instance(entity, assets_map[model_index]);
		}
		
		
		if (window.isPressed('e')) {
			transform.rot *= Util::quarter_rot(1);
		}
		if (window.isPressed('q')) {
			transform.rot *= Util::quarter_rot(-1);
		}

		if (window.isPressed('p')) {
			createTileInstance(assets_map[model_index], transform.pos, transform.rot);
		}
		transform.pos += move * grid_size;
	}
	
	auto view = registry.view<Graphics::DissolveMaterial>();
	for (auto entity : view) {
		if (Window::getInstance().isDown('z'))
			view.get<Graphics::DissolveMaterial>(entity).time += delta / 2.0f;
		else if(Window::getInstance().isDown('x'))
			view.get<Graphics::DissolveMaterial>(entity).time -= delta / 2.0f;
	}
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

				entt::hashed_string id(name.c_str());
				scene.loadMesh(id, path, offset, glm::vec3(scale));
				assets_map.push_back(id);
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
				auto& tileSubstring = node["tile"].val();
				std::string tileType;
				int rot = 0;

				read_string(tileSubstring, tileType);
				read_int(node["rot"].val(), rot);

				entt::id_type mesh_id = entt::hashed_string(tileType.c_str());
				for (ryml::NodeRef& loc : node["locations"]) {
					int x, z;
					read_int(loc[0].val(), x);
					read_int(loc[1].val(), z);

					glm::vec3 pos(-grid_size.x * x, 0, -grid_size.z * z);
					createTileInstance(mesh_id, pos, Util::quarter_rot(rot));
				}
			}
		}

		last_time_tiles = Util::last_write(tiles).value();
	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}
}

void LevelSystem::createTileInstance(entt::id_type mesh_id, const glm::vec3& pos, const glm::quat& rot) {
	using namespace entt;

	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	Graphics::DissolveMaterial material;
	material.diffuse = "dungeon_pallet"_hs;
	material.dissolve_color = glm::vec3(.7f, .4f, .1f);
	material.offset = .05f;
	material.time = 0.0f;

	//create instance
	LOA::ID id = scene.addInstance(mesh_id);
	auto& instance = scene.getInstance(id);
	instance.transform = Util::make_transform(pos, rot);

	//TODO: potentially dereferencing null
	glm::vec3 mesh_min = instance.mesh->getMin();
	glm::vec3 mesh_max = instance.mesh->getMax();
	glm::vec3 dim = mesh_max - mesh_min;
	glm::vec3 offset = (mesh_max + mesh_min) * .5f;

	auto& physicsScene = engine.getPhysicsScene();
	btRigidBody* body = physicsScene.createBox(0, dim * .5f, pos + rot * offset, rot);

	auto entity = registry.create();
	registry.emplace<Graphics::DissolveMaterial>(entity, material);
	registry.emplace<Component::Renderable>(entity, id);
	registry.emplace<Component::StaticBody>(entity, body);
	registry.emplace<Component::LevelTile>(entity);
}