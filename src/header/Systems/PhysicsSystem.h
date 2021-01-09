#pragma once
#include "System.h"
#include <entt/entt.hpp>

namespace LOA::Systems {
	class PhysicsSystem : public BaseSystem {
	public:
		PhysicsSystem(LOA::Engine& engine);
		void init();
		void update(float dt);

	private:
		void spawnHitBox(entt::registry& registry, entt::entity entity);
		void freeHitBox(entt::registry& registry, entt::entity entity);

		void spawnRigidBody(entt::registry& registry, entt::entity entity);
		void freeRigidBody(entt::registry& registry, entt::entity entity);
		
		void spawnStaticBody(entt::registry& registry, entt::entity entity);
		void freeStaticBody(entt::registry& registry, entt::entity entity);

		void spawnCharacterController(entt::registry& registry, entt::entity entity);
		void freeCharacterController(entt::registry& registry, entt::entity entity);
	};
}