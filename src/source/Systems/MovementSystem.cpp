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

	//camera fly controls
	auto camera_view = registry.view<Transformation, MovementState, Direction, Velocity, Camera>();
	for (auto entity : camera_view) {
		auto& transform = camera_view.get<Transformation>(entity);
		auto& movement = camera_view.get<MovementState>(entity);
		auto& dir = camera_view.get<Direction>(entity);
		auto& vel = camera_view.get<Velocity>(entity);
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

	auto camera = registry.view<Transformation, Direction, Camera>().front();
	auto& camera_transform = registry.get<Transformation>(camera);
	auto& camera_dir = registry.get<Direction>(camera);

	//player movement controls
	auto player_view = registry.view<Transformation, MovementState, Direction, Velocity>(entt::exclude<Camera>);
	for (auto entity : player_view) {
		auto& transform = camera_view.get<Transformation>(entity);
		auto& movement = camera_view.get<MovementState>(entity);
		auto& dir = camera_view.get<Direction>(entity);
		auto& vel = camera_view.get<Velocity>(entity);


		glm::vec3 forward = camera_transform.rot * camera_dir.up;
		forward.y = 0;
		forward = glm::normalize(forward);

		glm::vec3 right = camera_transform.rot * camera_dir.right;
		right = glm::normalize(right);

		vel = -10.0f * forward * movement.forward + 10.0f * right * movement.strafe;


		if (movement.fire) {
			spawnFireball(engine, transform.pos, transform.rot * dir.forward);
		}
	}

}