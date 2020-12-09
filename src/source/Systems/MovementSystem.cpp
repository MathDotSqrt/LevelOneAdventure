#include "Systems/MovementSystem.h"

#include "Engine.h"
#include "Components.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/intersect.hpp>
#include <assert.h>

using namespace LOA;
using namespace LOA::Systems;

glm::quat remove_pitch_rot(glm::quat rot) {
	float yaw = glm::yaw(rot);
	float roll = glm::roll(rot);
	glm::vec3 eular(roll, yaw, roll);

	return glm::quat(eular);
}

glm::quat look_at(glm::vec3 target, glm::vec3 source, glm::vec3 up) {
	glm::vec3 dir = normalize(target - source);
	up = glm::normalize(up);
	assert(glm::abs(glm::dot(dir, up)) < .999);
	return glm::quatLookAt(dir, up);
}	

void spawnFireball(Engine &engine, glm::vec3 pos, glm::vec3 forward) {
	using namespace Component;
	
	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	//ID id = scene.addPointLight();

	entt::entity fireball = registry.create();
	
	registry.emplace<Transformation>(fireball, pos);
	registry.emplace<Velocity>(fireball, glm::normalize(forward) * 20.0f);
	registry.emplace<FireParticle>(fireball, 200.0f);
	//registry.emplace<PointLight>(fireball, id, glm::vec3(1), 10.0f);


}

void MovementSystem::update(Engine& engine, float delta) {
	using namespace Component;
	auto& registry = engine.getRegistry();

	//camera fly controls
	auto fly_camera_view = registry.view<Transformation, MovementState, Direction, Velocity, Camera>();
	for (auto entity : fly_camera_view) {
		auto& transform = fly_camera_view.get<Transformation>(entity);
		auto& movement = fly_camera_view.get<MovementState>(entity);
		auto& dir = fly_camera_view.get<Direction>(entity);
		auto& vel = fly_camera_view.get<Velocity>(entity);
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
		auto& transform = player_view.get<Transformation>(entity);
		auto& movement = player_view.get<MovementState>(entity);
		auto& dir = player_view.get<Direction>(entity);
		auto& vel = player_view.get<Velocity>(entity);


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



	//Camera Follow
	auto camera_follow_view = registry.view<Transformation, Direction, Camera, Input>(entt::exclude<MovementState>);
	for (auto entity : camera_follow_view) {
		auto& transform = camera_follow_view.get<Transformation>(entity);
		auto& dir = camera_follow_view.get<Direction>(entity);
		auto& camera = camera_follow_view.get<Camera>(entity);
		auto& input = camera_follow_view.get<Input>(entity);
		if (!registry.valid(camera.player)) {
			continue;
		}
		auto& player_transform = registry.get<Transformation>(camera.player);
		auto& player_dir = registry.get<Direction>(camera.player);
		

		////Follow Char
		//{
		//	glm::vec3 plane_origin = player_transform.pos;
		//	float distance = 0;
		//	bool intersected = glm::intersectRayPlane(transform.pos, transform.rot * dir.forward, plane_origin, glm::vec3(0, 1, 0), distance);
		//	assert(intersected);

		//	glm::vec3 floor_pos = transform.pos + glm::normalize(transform.rot * dir.forward) * distance;

		//	glm::vec3 delta = player_transform.pos - floor_pos;
		//	transform.pos += delta;
		//}

		//camera.phi += .01f;

		camera.phi = glm::max(glm::min(camera.phi, glm::pi<float>() / 2.5f), 0.1f);
		camera.distance = camera.phi * 20;

		glm::quat pitch = glm::angleAxis(camera.phi, dir.right);
		glm::quat theta = glm::angleAxis(camera.theta, dir.up);

		transform.pos = theta * (pitch * dir.forward * camera.distance) + player_transform.pos;
		transform.rot = look_at(player_transform.pos, transform.pos, dir.up);
	}
}