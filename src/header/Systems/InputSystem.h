#pragma once

#include "Systems/System.h"

namespace LOA::Systems {
	class InputSystem : public BaseSystem {
	public:
		void init(Engine& engine);
		void update(Engine& engine, float delta);
	};
}