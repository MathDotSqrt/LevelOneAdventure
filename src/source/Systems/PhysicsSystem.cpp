#include "Systems/PhysicsSystem.h"
#include "Engine.h"
#include "Components.h"
#include "Physics/PhysicsScene.h"
#include "BulletDynamics/btBulletDynamicsCommon.h"

#include <iostream>

using namespace LOA;
using namespace LOA::Systems;
using namespace LOA::Physics;

void spawn(entt::registry& registry, entt::entity entity) {
	auto& scene = registry.ctx<Physics::PhysicsScene>();
	auto& transform = registry.get<Component::Transformation>(entity);
	auto& collision = registry.get<Component::Collision>(entity);
	collision.body = scene.createBox(transform.pos, glm::vec3(.5));
}


void PhysicsSystem::init(Engine& engine) {
	using namespace entt;

	auto& registry = engine.getRegistry();
	registry.on_construct<Component::Collision>().connect<&spawn>();


	auto& scene = engine.getPhysicsScene();

	scene.createStaticPlane(glm::vec3(0, 1, 0), -1);
	scene.setGravity(glm::vec3(0, -10, 0));
}

void PhysicsSystem::update(Engine& engine, float delta) {
	using namespace Component;
	auto& registry = engine.getRegistry();
	
	auto view2 = registry.view<Velocity, Collision>();
	for (auto entity : view2) {
		auto& vel = view2.get<Velocity>(entity);
		auto& collision = view2.get<Collision>(entity);

		collision.body->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
	}

	engine.getPhysicsScene().update(delta);

	auto view = registry.view<Transformation, Velocity, Collision>();
	for (auto entity : view) {
		auto& transform = view.get<Transformation>(entity);
		auto& vel = view.get<Velocity>(entity);
		auto& collision = view.get<Collision>(entity);

		auto& bt_transform = collision.body->getWorldTransform();
		auto& origin = bt_transform.getOrigin();
		auto& quat = bt_transform.getRotation();
		transform.pos = glm::vec3(origin.x(), origin.y(), origin.z());
		transform.rot = glm::quat(quat.w(), quat.x(), quat.y(), quat.z());

		btVector3 btVel = collision.body->getLinearVelocity();
		vel = glm::vec3(btVel.x(), btVel.y(), btVel.z());
	}
}