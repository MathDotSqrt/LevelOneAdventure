#pragma once

#include "Systems/System.h"
namespace LOA::Systems {
	class AISystem : public BaseSystem {
	public:
		AISystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
		void chase(float delta);
		//void attack(entt::entity ent, entt::registry &reg);
	};
}