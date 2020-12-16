#pragma once

#include "Systems/System.h"

namespace LOA::Systems {
	class LevelSystem : public BaseSystem{
	public:
		LevelSystem(LOA::Engine& engine) : BaseSystem(engine) {}

		void init();
		void update(float delta);
	};
}