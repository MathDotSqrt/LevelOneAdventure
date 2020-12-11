#include "Systems/PhysicsSystem.h"
#include "Engine.h"
#include "Components.h"

#include <iostream>

using namespace LOA;
using namespace LOA::Systems;

void spawn(entt::registry& registry, entt::entity entity) {
	using namespace reactphysics3d;
}


void PhysicsSystem::init(Engine& engine) {
	engine.getPhysicsScene().setGravity(glm::vec3(0, -10, 0));
}

void PhysicsSystem::update(Engine& engine, float delta) {
	using namespace reactphysics3d;

	auto& registry = engine.getRegistry();


}