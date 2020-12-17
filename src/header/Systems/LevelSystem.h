#pragma once

#include "Systems/System.h"
#include <ryml.hpp>

namespace LOA::Systems {
	class LevelSystem : public BaseSystem{
	public:
		LevelSystem(LOA::Engine& engine) : BaseSystem(engine) {}

		void init();
		void update(float delta);
	private:
		ryml::Tree asset_tree;

		void loadAssets();
		void loadTiles();
	};
}