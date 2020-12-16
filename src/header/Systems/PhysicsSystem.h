#pragma once
#include "System.h"
#include <entt/entt.hpp>

namespace LOA::Systems {
	class PhysicsSystem : public BaseSystem {
	public:
		PhysicsSystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float dt);

	private:
		void spawnRigidBody(entt::registry& registry, entt::entity entity);
	};
}