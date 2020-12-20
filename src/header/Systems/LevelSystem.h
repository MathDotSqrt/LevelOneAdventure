#pragma once

#include "Systems/System.h"
#include <ryml.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/core/hashed_string.hpp>
#include <vector>
#include <string>

namespace LOA::Systems {

	//This entire hoe will likley be rewritten
	//Just wanted a quick level editor/loader
	class LevelSystem : public BaseSystem{
	public:
		LevelSystem(LOA::Engine& engine) : BaseSystem(engine) {}

		void init();
		void update(float delta);
	private:
		size_t model_index = 0;
		std::vector<entt::id_type> assets_map;

		//todo find better way to aquire name
		std::vector<std::string> assets_names;

		ryml::Tree asset_tree;

		void saveScene();
		void loadAssets();
		void loadTiles();

		void createTileInstance(entt::hashed_string mesh_id, const glm::ivec3& pos, int rot);
	};
}