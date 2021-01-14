#pragma once

#include "System.h"

namespace LOA::Systems {
	class HealthSystem : public BaseSystem {
	public:
		HealthSystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
	};
}