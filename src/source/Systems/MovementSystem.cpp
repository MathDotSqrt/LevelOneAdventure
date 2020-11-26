#include "Systems/MovementSystem.h"

#include "Engine.h"
#include "Components.h"

using namespace LOA::Systems;

glm::quat remove_pitch_rot(glm::quat rot) {
	float yaw = glm::yaw(rot);
	float roll = glm::roll(rot);
	glm::vec3 eular(roll, yaw, roll);

	return glm::quat(eular);
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
	}
}