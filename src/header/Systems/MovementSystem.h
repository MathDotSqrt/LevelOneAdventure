#pragma once
#include "Systems/System.h"

namespace LOA::Systems {
	class MovementSystem : public BaseSystem {
	public:
		void update(Engine& engine, float delta);
	};
}