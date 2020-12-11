#include "Systems/PhysicsSystem.h"
#include "Engine.h"
#include "Components.h"

#include "reactphysics3d/reactphysics3d.h"


using namespace LOA;
using namespace LOA::Systems;

void spawn(entt::registry& registry, entt::entity entity) {
	reactphysics3d::PhysicsCommon physicsCommon;


}

void PhysicsSystem::init(Engine& engine) {
	auto& registry = engine.getRegistry();
	registry.on_construct<Component::Collision>().connect<&spawn>();
}

void PhysicsSystem::update(Engine& engine, float delta) {

	
}