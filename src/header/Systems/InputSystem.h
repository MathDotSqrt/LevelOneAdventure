#pragma once

#include "Systems/System.h"

namespace LOA::Systems {
	class InputSystem : public BaseSystem {
	public:
		InputSystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
	};
}