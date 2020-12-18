#include "Systems/PhysicsSystem.h"
#include "Engine.h"
#include "Components.h"
#include "Physics/PhysicsScene.h"
#include "util/Timer.h"

#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <iostream>

using namespace LOA;
using namespace LOA::Systems;
using namespace LOA::Physics;

btVector3 to_bt(const glm::vec3 &vec) {
	return btVector3(vec.x, vec.y, vec.z);
}

btQuaternion to_bt(const glm::quat& quat) { 
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

glm::vec3 to_glm(const btVector3& vec) {
	return glm::vec3(vec.x(), vec.y(), vec.z());
}

glm::quat to_glm(const btQuaternion& quat) {
	return glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
}

void PhysicsSystem::spawnRigidBody(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	
	auto& collision = registry.get<Component::RigidBody>(entity);

	if (collision.body == nullptr) {
		collision.body = scene.createBox(collision.mass, collision.dim);
	}

}

void PhysicsSystem::freeRigidBody(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	auto& collision = registry.get<Component::RigidBody>(entity);
	scene.freeBox(collision.body);
}

void PhysicsSystem::spawnStaticBody(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	auto& static_body = registry.get<Component::StaticBody>(entity);
	if (static_body.body == nullptr) {
		static_body.body = scene.createBox(0, static_body.dim);
	}
}

void PhysicsSystem::freeStaticBody(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	auto& static_body = registry.get<Component::StaticBody>(entity);
	scene.freeBox(static_body.body);
}

void PhysicsSystem::spawnCharacterController(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();

	auto& collision = registry.get<Component::CharacterController>(entity);
	collision.kinematicCollider = scene.createCharacterController();
}

void PhysicsSystem::freeCharacterController(entt::registry& registry, entt::entity entity) {
	auto& scene = engine.getPhysicsScene();
	auto& controller = registry.get<Component::CharacterController>(entity);
	scene.freeCharacterController(controller.kinematicCollider);
}

PhysicsSystem::PhysicsSystem(Engine& engine) : BaseSystem(engine) {
	
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

	scene.createStaticPlane(glm::vec3(0, 1, 0), -10);
	scene.setGravity(glm::vec3(0, -10, 0));
}

void PhysicsSystem::update(float delta) {
	LOA::Util::Timer timer("Physics");

	using namespace Component;
	auto& registry = engine.getRegistry();
	
	auto static_view = registry.view<Transformation, StaticBody>();
	for (auto entity : static_view) {
		auto& transform = static_view.get<Transformation>(entity);
		auto& static_body = static_view.get<StaticBody>(entity);

		glm::vec3 rigid_body_origin = (transform.rot * static_body.offset) + transform.pos;

		btTransform bt_transform;
		bt_transform.setIdentity();
		bt_transform.setOrigin(to_bt(rigid_body_origin));
		bt_transform.setRotation(to_bt(transform.rot));

		static_body.body->setWorldTransform(bt_transform);
	}

	auto character_view = registry.view<Transformation, Velocity, CharacterController>();
	for (auto entity : character_view) {
		auto& transform = character_view.get<Transformation>(entity);
		auto& vel = character_view.get<Velocity>(entity);
		auto& controller = character_view.get<CharacterController>(entity);

		//For some reason, to interact with the controller you need to access its 'Ghost Object'
		//More info in PhysicsScene.cpp
		btKinematicCharacterController* kinematic_controller = controller.kinematicCollider;
		btPairCachingGhostObject* ghost_object = kinematic_controller->getGhostObject();

		btTransform bt_transform;
		bt_transform.setIdentity();
		bt_transform.setOrigin(to_bt(transform.pos));
		bt_transform.setRotation(to_bt(transform.rot));

		ghost_object->setWorldTransform(bt_transform);

		kinematic_controller->setLinearVelocity(to_bt(vel * delta));
	}

	auto view = registry.view<Transformation, Velocity, RigidBody>();
	for (auto entity : view) {
		auto& transform = view.get<Transformation>(entity);
		auto& vel = view.get<Velocity>(entity);
		auto& collision = view.get<RigidBody>(entity);

		//
		glm::vec3 rigid_body_origin = (transform.rot * collision.offset) + transform.pos;

		btTransform bt_transform;
		bt_transform.setIdentity();
		bt_transform.setOrigin(to_bt(rigid_body_origin));
		bt_transform.setRotation(to_bt(transform.rot));

		collision.body->setWorldTransform(bt_transform);
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
		transform.pos = to_glm(origin);
		transform.rot = to_glm(quat);

		btVector3 btVel = kinematic_controller->getLinearVelocity() / delta;
		vel = to_glm(btVel);
	}

	for (auto entity : view) {
		auto& transform = view.get<Transformation>(entity);
		auto& vel = view.get<Velocity>(entity);
		auto& collision = view.get<RigidBody>(entity);

		auto& bt_transform = collision.body->getWorldTransform();
		auto& origin = bt_transform.getOrigin();
		auto& quat = bt_transform.getRotation();
		

		transform.rot = to_glm(quat);
		
		transform.pos = to_glm(origin) - (transform.rot * collision.offset);

		//TODO: invetigate if i need to divide by delta?
		//seems to work without dividing, but character controller needs to be divided
		btVector3 btVel = collision.body->getLinearVelocity();
		vel = to_glm(btVel);
	}


}