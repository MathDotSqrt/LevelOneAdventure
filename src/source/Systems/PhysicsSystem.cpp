#include "Systems/PhysicsSystem.h"
#include "Engine.h"
#include "q3.h"
#include "Components.h"

using namespace LOA;
using namespace LOA::Systems;

void spawn(entt::registry& registry, entt::entity entity) {
	auto& scene = registry.ctx<q3Scene>();
	
	auto& transform = registry.get<Component::Transformation>(entity);
	auto& collision = registry.get<Component::Collision>(entity);
	
	glm::vec3 pos = transform.pos;
	float angle = glm::angle(transform.rot);
	glm::vec3 axis = glm::axis(transform.rot);

	q3BodyDef def;
	def.position = q3Vec3(pos.x, pos.y, pos.z);
	def.angle = angle;
	def.axis = q3Vec3(axis.x, axis.y, axis.z);

	collision.body = scene.CreateBody(def);
	
	q3BoxDef boxDef; 
	q3Transform localSpace; // Contains position and orientation, see q3Transform.h for details
	q3Identity(localSpace); // Specify the origin, and identity orientation

	// Create a box at the origin with width, height, depth = (1.0, 1.0, 1.0)
	// and add it to a rigid body. The transform is defined relative to the owning body
	boxDef.Set(localSpace, q3Vec3(1.0, 1.0, 1.0));
	collision.body->AddBox(boxDef);

}

void PhysicsSystem::init(Engine& engine) {
	auto& registry = engine.getRegistry();
	registry.on_construct<Component::Collision>().connect<&spawn>();
}

void PhysicsSystem::update(Engine& engine, float delta) {
	q3Scene& scene = engine.getPhysicsScene();

	
}