#include "Systems/MovementSystem.h"

#include "Engine.h"
#include "Components.h"

using namespace LOA;
using namespace LOA::Systems;

glm::quat remove_pitch_rot(glm::quat rot) {
	float yaw = glm::yaw(rot);
	float roll = glm::roll(rot);
	glm::vec3 eular(roll, yaw, roll);

	return glm::quat(eular);
}

void spawnFireball(Engine &engine, glm::vec3 pos, glm::vec3 forward) {
	using namespace Component;
	
	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	ID id = scene.addPointLight();

	entt::entity fireball = registry.create();
	
	registry.emplace<Transformation>(fireball, pos);
	registry.emplace<Velocity>(fireball, glm::normalize(forward) * 20.0f);
	registry.emplace<FireParticle>(fireball, 200.0f);


}

void MovementSystem::update(Engine& engine, float delta) {
	using namespace Component;

	auto& registry = engine.getRegistry();
	auto view = registry.view<Transformation, MovementState, Direction, Velocity>();
	for (auto entity : view) {
		auto& transform = view.get<Transformation>(entity);
		auto& movement = view.get<MovementState>(entity);
		auto& dir = view.get<Direction>(entity);
		auto& vel = view.get<Velocity>(entity);

		

		glm::quat pitch = glm::angleAxis(movement.rotate.y / 500.0f, dir.right);
		glm::quat yaw = glm::angleAxis(movement.rotate.x / 500.0f, dir.up);

		glm::quat new_rot = (yaw * transform.rot) * pitch;
		glm::quat move_rot = remove_pitch_rot(new_rot);

		transform.rot = new_rot;

		glm::vec3 move_vector(movement.strafe * 10, movement.fly * 10, movement.forward * 10);
		vel = move_rot * move_vector;


		if (movement.fire) {
			spawnFireball(engine, transform.pos, new_rot * dir.forward);
		}
	}
}