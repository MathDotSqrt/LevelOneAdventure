
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
static std::string tiles = "./res/scene/scene.yaml";
static i64 last_time = 0;
static i64 last_time_tiles = 0;

static const glm::vec3 grid_size(3, 1.5, 3);


void erase_char(std::string& str, char c) {
	auto end_pos = std::remove(str.begin(), str.end(), c);
	str.erase(end_pos, str.end());
}

bool read_int(c4::csubstr buffer, int& f) {
	size_t pos = c4::unformat(buffer, "{}", f);
	return pos != c4::csubstr::npos;
}

bool read_int(c4::csubstr buffer, entt::id_type& id) {
	size_t pos = c4::unformat(buffer, "{}", id);
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

bool read_vec3(c4::csubstr buffer, glm::ivec3& vec) {
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

bool read_string(c4::substr buffer, std::string& str) {
	c4::from_chars(buffer, &str);
	return true;
}

std::string to_chars(glm::ivec3 v)
{
	return std::string("glm::vec3( ") + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
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
		registry.emplace<Direction>(builder);
	};
	create_builder(glm::vec3(0));
	//create_builder(glm::vec3(3, 0, 0));
	//create_builder(glm::vec3(0, 0, 3));
	//create_builder(glm::vec3(3, 0, 3));
	

}

void LevelSystem::update(float delta) {
	using namespace entt;
	using namespace Component;

	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();
	auto& window = Window::getInstance();

	auto destroy_tiles = [&]() {
		
		auto view = registry.view<Component::LevelTile>();
		registry.destroy(view.begin(), view.end());
	};

	auto replace_instance = [&](entt::entity entity, entt::id_type new_mesh_id) {
		ID id = scene.addInstance(new_mesh_id, Graphics::TranslucentBasicMaterial{ "dungeon_pallet"_hs, .75f });
		auto& renderable = registry.get<Renderable>(entity);
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
	if ((last_write.has_value() && last_write.value() > last_time_tiles) || window.isPressed('r')) {
		destroy_tiles();
		loadTiles();
	}


	auto camera_view = registry.view<Camera, Transformation>();
	auto camera = camera_view.front();
	glm::quat camera_rot = camera_view.get<Transformation>(camera).rot;
	
	glm::vec3 camera_forward = camera_rot * glm::vec3(0, 0, -1);
	camera_forward.y = 0;
	camera_forward = glm::normalize(camera_forward);

	glm::vec3 camera_right = camera_rot * glm::vec3(1, 0, 0);
	
	auto builder_view = registry.view<LevelBuilder, Transformation>();
	
	int scroll_delta = 0;
	scroll_delta += window.isPressed('=');
	scroll_delta -= window.isPressed('-');
	if (scroll_delta != 0) {
		model_index += scroll_delta;
		model_index = ((model_index + assets_map.size()) % assets_map.size());
	}

	for (auto entity : builder_view) {
		auto& transform = registry.get<Transformation>(entity);
		auto& builder = registry.get<LevelBuilder>(entity);

		float right = 0;
		float forward = 0;
		float up = 0;
		right += window.isPressed(Window::Keys::RIGHT_ARROW) ? 1 : 0;
		right -= window.isPressed(Window::Keys::LEFT_ARROW) ? 1 : 0;
		forward += window.isPressed(Window::Keys::UP_ARROW) ? 1 : 0;
		forward -= window.isPressed(Window::Keys::DOWN_ARROW) ? 1 : 0;
		up += window.isPressed(']') ? 1 : 0;
		up -= window.isPressed('[') ? 1 : 0;
		
		if (scroll_delta != 0) {
			replace_instance(entity, assets_map[model_index]);
		}
		
		
		if (window.isPressed('e')) {
			builder.rot += 1;
		}
		if (window.isPressed('q')) {
			builder.rot -= 1;
		}

		transform.rot = Util::quarter_rot(builder.rot);

		if (window.isPressed('p')) {
			//TODO: rewrite level system to not have random string copies everwhere
			entt::hashed_string terrible_code = entt::hashed_string(assets_names[model_index].c_str());
			glm::ivec3 loc = glm::floor(transform.pos / grid_size);	//more terribleness
			createTileInstance(terrible_code, loc, builder.rot);
		}
		
		glm::vec3 move_forward = glm::round(camera_forward * forward);
		glm::vec3 move_right = glm::round(camera_right * right);
		glm::vec3 move_up = glm::vec3(0, up, 0);
		glm::vec3 move = move_forward + move_right + move_up;
		transform.pos += move * grid_size;
	}
	
	if (window.isPressed('y')) {
		saveScene();
	}
	
	auto view = registry.view<Graphics::DissolveMaterial>();
	for (auto entity : view) {
		if (Window::getInstance().isDown('x'))
			view.get<Graphics::DissolveMaterial>(entity).time += delta / 2.0f;
		else if(Window::getInstance().isDown('z'))
			view.get<Graphics::DissolveMaterial>(entity).time -= delta / 2.0f;
	}

}

void LevelSystem::saveScene() {
	using namespace Component;

	auto& registry = engine.getRegistry();
	auto view = registry.view<LevelTile>();

	ryml::Tree tree;
	ryml::NodeRef r = tree.rootref();

	r |= ryml::MAP;

	ryml::NodeRef tiles = r["tiles"];
	tiles |= ryml::SEQ;

	for (auto entity : view) {
		auto& level_tile = view.get<LevelTile>(entity);
		
		ryml::NodeRef tile = tiles.append_child();
		tile |= ryml::MAP;

		std::string loc = to_chars(level_tile.loc);

		tile.append_child() << ryml::key("tile") << level_tile.model_name;
		tile.append_child() << ryml::key("location") << loc;
		tile.append_child() << ryml::key("rot") << level_tile.rot;
	}

	FILE* pfile = fopen("./res/scene/scene.yaml", "w");
	assert(pfile);
	ryml::emit(r, pfile);
	fclose(pfile);
}

void LevelSystem::loadAssets() {
	try {
		assets_map.clear();
		assets_names.clear();

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
				assets_names.push_back(name);
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
			root |= ryml::MAP;
			if (!root.has_child("tiles")) {
				throw std::exception("Empty yaml");
			}

			ryml::NodeRef tiles = root["tiles"];
			for (ryml::NodeRef& node : tiles) {
				auto& tileSubstring = node["tile"].val();
				std::string tileType;
				int rot = 0;

				read_string(tileSubstring, tileType);
				read_int(node["rot"].val(), rot);

				entt::hashed_string mesh_id = entt::hashed_string(tileType.c_str());

				glm::ivec3 loc;
				read_vec3(node["location"].val(), loc);
				createTileInstance(mesh_id, loc, rot);
			}
		}

	}
	catch (std::exception& e) {
		std::cerr << e.what();
	}

	last_time_tiles = Util::last_write(tiles).value();

}

void LevelSystem::createTileInstance(entt::hashed_string mesh_id, const glm::ivec3& loc, int rot) {
	using namespace entt;

	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	Graphics::DissolveMaterial material;
	material.diffuse = "dungeon_pallet"_hs;
	material.dissolve_color = glm::vec3(.7f, .4f, .1f);
	material.offset = .05f;
	material.time = 0.0f;

	//create instance
	glm::vec3 pos = glm::vec3(loc) * grid_size;
	LOA::ID id = scene.addInstance(mesh_id);
	auto& instance = scene.getInstance(id);
	glm::quat rotation = Util::quarter_rot(rot);
	instance.transform = Util::make_transform(pos, rotation);

	//TODO: potentially dereferencing null
	glm::vec3 mesh_min = instance.mesh->getMin();
	glm::vec3 mesh_max = instance.mesh->getMax();
	glm::vec3 dim = mesh_max - mesh_min;
	glm::vec3 offset = (mesh_max + mesh_min) * .5f;

	auto& physicsScene = engine.getPhysicsScene();
	btRigidBody* body = physicsScene.createBox(0, dim * .5f, pos + rotation * offset, rotation);

	auto entity = registry.create();
	registry.emplace<Graphics::DissolveMaterial>(entity, material);
	registry.emplace<Component::Renderable>(entity, id);
	registry.emplace<Component::StaticBody>(entity, body);
	registry.emplace<Component::LevelTile>(entity, std::string(mesh_id.data()), loc, rot);
}