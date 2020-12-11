#include "Systems/PhysicsSystem.h"
#include "Engine.h"
#include "Components.h"

#include <iostream>

#include "reactphysics3d/reactphysics3d.h"


using namespace LOA;
using namespace LOA::Systems;

void spawn(entt::registry& registry, entt::entity entity) {
	using namespace reactphysics3d;

	auto& common = registry.ctx<PhysicsCommon>();
	auto& world = registry.ctx<PhysicsWorld*>();
	
	auto& collision = registry.get<Component::Collision>(entity);
	auto& transform = registry.get<Component::Transformation>(entity);
	auto& vel = registry.get<Component::Velocity>(entity);

	glm::vec3 pos = transform.pos;
	glm::quat rot = transform.rot;

	Vector3 physics_pos(pos.x, pos.y, pos.z);
	Quaternion physics_quat(rot.x, rot.y, rot.z, rot.w);
	Transform physics_transform(physics_pos, physics_quat);

	BoxShape* shape = common.createBoxShape(Vector3(.5f, .5f, .5f));

	RigidBody* body = world->createRigidBody(physics_transform);
	body->setType(BodyType::DYNAMIC);
	body->setLinearVelocity(Vector3(vel.x, vel.y, vel.z));
	Collider* collider = body->addCollider(shape, Transform::identity());
	collision.body = body;
}

static reactphysics3d::RigidBody* plane;

void PhysicsSystem::init(Engine& engine) {
	using namespace reactphysics3d;

	auto& registry = engine.getRegistry();
	registry.on_construct<Component::Collision>().connect<&spawn>();

	auto& common = engine.getPhysicsCommon();
	auto& world = engine.getPhysicsWorld();

	BoxShape* shape = common.createBoxShape(Vector3(100, 1.0f, 100.0f));

	Transform t = Transform::identity();
	t.setPosition(Vector3(0, -2.2, 0));

	plane = world.createRigidBody(t);
	plane->setType(BodyType::STATIC);
	plane->setIsActive(true);
	Collider* collider = plane->addCollider(shape, Transform::identity());

}

void PhysicsSystem::update(Engine& engine, float delta) {
	using namespace reactphysics3d;

	auto& registry = engine.getRegistry();
	auto& world = engine.getPhysicsWorld();

	auto view = registry.view<Component::Transformation, Component::Collision>();

	world.update(1 / 60.0f);
	for(entt::entity e : view){
		auto& transform = view.get<Component::Transformation>(e);
		auto& collision = view.get<Component::Collision>(e);

		auto& physics_transform = collision.body->getTransform();
		Vector3 pos = physics_transform.getPosition();
		Quaternion quat = physics_transform.getOrientation();


		transform.pos = glm::vec3(pos.x, pos.y, pos.z);
		transform.rot = glm::quat(quat.w, quat.x, quat.y, quat.z);
	}

}