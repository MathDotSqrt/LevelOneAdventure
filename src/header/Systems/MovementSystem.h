#pragma once
#include "Systems/System.h"

namespace LOA::Systems {
	class MovementSystem : public BaseSystem {
	public:
		MovementSystem(LOA::Engine& engine) : BaseSystem(engine) {}

		void update(float delta);
	};
}