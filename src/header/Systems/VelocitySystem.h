#pragma once
#include "Systems/System.h"

namespace LOA::Systems {
	class VelocitySystem : public BaseSystem {
	public:
		void update(Engine& engine, float delta);
	};
}