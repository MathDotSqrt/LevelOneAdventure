#include "Systems/PhysicsSystem.h"
#include "Engine.h"
#include "Components.h"
#include "Physics/PhysicsScene.h"
#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <iostream>

using namespace LOA;
using namespace LOA::Systems;
using namespace LOA::Physics;

void PhysicsSystem::spawnRigidBody(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	
	//Todo dont rely on transformation being a component
	auto& transform = registry.get<Component::Transformation>(entity);
	auto& collision = registry.get<Component::RigidBody>(entity);
	collision.body = scene.createBox(transform.pos, glm::vec3(.5), collision.mass);
}

void PhysicsSystem::freeRigidBody(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	auto& collision = registry.get<Component::RigidBody>(entity);
	scene.freeBox(collision.body);
}

void PhysicsSystem::spawnStaticBody(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	auto& transform = registry.get<Component::Transformation>(entity);
	auto& static_body = registry.get<Component::StaticBody>(entity);
	static_body.body = scene.createBox(transform.pos, static_body.dim, 0);
}

void PhysicsSystem::freeStaticBody(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	auto& static_body = registry.get<Component::StaticBody>(entity);
	scene.freeBox(static_body.body);
}

void PhysicsSystem::spawnCharacterController(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();

	//Todo dont rely on transformation being a component
	auto& transform = registry.get<Component::Transformation>(entity);
	auto& collision = registry.get<Component::CharacterController>(entity);
	collision.kinematicCollider = scene.createCharacterController();
}

void PhysicsSystem::freeCharacterController(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	auto& controller = registry.get<Component::CharacterController>(entity);
	scene.freeCharacterController(controller.kinematicCollider);
}



void PhysicsSystem::init() {
	using namespace entt;

	auto& registry = engine.getRegistry();
	
	registry.on_construct<Component::RigidBody>().connect<&PhysicsSystem::spawnRigidBody>(this);
	registry.on_destroy<Component::RigidBody>().connect<&PhysicsSystem::freeRigidBody>(this);
	
	registry.on_construct<Component::StaticBody>().connect<&PhysicsSystem::spawnStaticBody>(this);
	registry.on_destroy<Component::StaticBody>().connect<&PhysicsSystem::freeStaticBody>(this);


	registry.on_construct<Component::CharacterController>().connect<&PhysicsSystem::spawnCharacterController>(this);
	registry.on_destroy<Component::CharacterController>().connect<&PhysicsSystem::freeCharacterController>(this);

	auto& scene = engine.getPhysicsScene();

	scene.createStaticPlane(glm::vec3(0, 1, 0), -1);
	scene.setGravity(glm::vec3(0, -10, 0));
}

void PhysicsSystem::update(float delta) {
	using namespace Component;
	auto& registry = engine.getRegistry();
	
	auto character_view = registry.view<Transformation, Velocity, CharacterController>();
	for (auto entity : character_view) {
		auto& transform = character_view.get<Transformation>(entity);
		auto& vel = character_view.get<Velocity>(entity);
		auto& controller = character_view.get<CharacterController>(entity);

		//For some reason, to interact with the controller you need to access its 'Ghost Object'
		//More info in PhysicsScene.cpp
		btKinematicCharacterController* kinematic_controller = controller.kinematicCollider;
		btPairCachingGhostObject* ghost_object = kinematic_controller->getGhostObject();

		//btTransform bt_transform;
		//bt_transform.setIdentity();
		//bt_transform.setOrigin(btVector3(transform.pos.x, transform.pos.y, transform.pos.z));
		//bt_transform.setRotation(btQuaternion(transform.rot.x, transform.rot.y, transform.rot.z, transform.rot.w));

		//ghost_object->setWorldTransform(bt_transform);

		kinematic_controller->setLinearVelocity(btVector3(vel.x, vel.y, vel.z) * btScalar(delta));
	}

	auto view = registry.view<Transformation, Velocity, RigidBody>();
	for (auto entity : view) {
		auto& transform = view.get<Transformation>(entity);
		auto& vel = view.get<Velocity>(entity);
		auto& collision = view.get<RigidBody>(entity);

		collision.body->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
	}

	engine.getPhysicsScene().update(delta);

	for (auto entity : character_view) {
		auto& transform = character_view.get<Transformation>(entity);
		auto& vel = character_view.get<Velocity>(entity);
		auto& controller = character_view.get<CharacterController>(entity);

		btKinematicCharacterController* kinematic_controller = controller.kinematicCollider;
		btPairCachingGhostObject* ghost_object = kinematic_controller->getGhostObject();

		auto& bt_transform = ghost_object->getWorldTransform();
		auto& origin = bt_transform.getOrigin();
		auto& quat = bt_transform.getRotation();
		transform.pos = glm::vec3(origin.x(), origin.y(), origin.z());
		transform.rot = glm::quat(quat.w(), quat.x(), quat.y(), quat.z());

		btVector3 btVel = kinematic_controller->getLinearVelocity() / delta;
		vel = glm::vec3(btVel.x(), btVel.y(), btVel.z());
	}

	for (auto entity : view) {
		auto& transform = view.get<Transformation>(entity);
		auto& vel = view.get<Velocity>(entity);
		auto& collision = view.get<RigidBody>(entity);

		auto& bt_transform = collision.body->getWorldTransform();
		auto& origin = bt_transform.getOrigin();
		auto& quat = bt_transform.getRotation();
		transform.pos = glm::vec3(origin.x(), origin.y(), origin.z());
		transform.rot = glm::quat(quat.w(), quat.x(), quat.y(), quat.z());

		btVector3 btVel = collision.body->getLinearVelocity();
		vel = glm::vec3(btVel.x(), btVel.y(), btVel.z());
	}


}