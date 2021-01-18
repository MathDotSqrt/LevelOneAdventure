#pragma once

#include "Systems/System.h"
#include "Components.h"
namespace LOA::Systems {
	class AISystem : public BaseSystem {
	public:
		AISystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
		void chase(Component::Transformation& trans,Component::Direction dir, glm::vec3& targtrans, float delta);
		//void attack(entt::entity ent, entt::registry &reg);
	};
}