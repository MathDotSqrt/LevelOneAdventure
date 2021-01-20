#pragma once

#include "Systems/System.h"
#include "Components.h"
#include "entt/entt.hpp"
namespace LOA::Systems {
	class AISystem : public BaseSystem {
	public:
		AISystem(LOA::Engine& engine);
		void init();
		void update(float delta);

	private:
		void createDebugCube(entt::registry& registry, entt::entity entity);
		void chase(Component::Transformation& trans,Component::Direction dir, glm::vec3& targtrans, float speed, float delta);
		//void attack(entt::entity ent, entt::registry &reg);
	};
}