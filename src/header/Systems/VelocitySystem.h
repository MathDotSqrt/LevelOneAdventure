#pragma once
#include "Systems/System.h"

namespace LOA::Systems {
	class VelocitySystem : public BaseSystem {
	public:
		VelocitySystem(LOA::Engine& engine) : BaseSystem(engine) {}

		void update(float delta);
	};
}