#pragma once

#include "Systems/System.h"
#include "Components.h"
namespace LOA::Systems {
	class AttackSystem : public BaseSystem {
	public:
		AttackSystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
	};
}