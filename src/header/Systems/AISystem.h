#pragma once

#include "Systems/System.h"
namespace LOA::Systems {
	class AISystem : public BaseSystem {
	public:
		AISystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
	};
}