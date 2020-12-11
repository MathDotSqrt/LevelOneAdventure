#pragma once
#include "System.h"

namespace LOA::Systems {
	class PhysicsSystem : public BaseSystem {
	public:
		void init(Engine& engine);
		void update(Engine& engine, float dt);
	};
}