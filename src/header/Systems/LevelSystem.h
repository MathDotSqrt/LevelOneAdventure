#pragma once

#include "Systems/System.h"
#include <ryml.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/core/hashed_string.hpp>
#include <vector>

namespace LOA::Systems {
	class LevelSystem : public BaseSystem{
	public:
		LevelSystem(LOA::Engine& engine) : BaseSystem(engine) {}

		void init();
		void update(float delta);
	private:
		size_t model_index = 0;
		std::vector<entt::id_type> assets_map;
		ryml::Tree asset_tree;

		void loadAssets();
		void loadTiles();

		void createTileInstance(entt::id_type mesh_id, const glm::vec3& pos, const glm::quat &rot);
	};
}