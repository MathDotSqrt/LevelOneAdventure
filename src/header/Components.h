#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>

#include "common.h"
#include "Util/PackedFreeList.h"

class btRigidBody;
class btKinematicCharacterController;

namespace LOA::Systems {
	class RenderSystem;
}

namespace LOA::Component {
	struct Transformation {
		glm::vec3 pos = glm::vec3(0);
		glm::quat rot = glm::quat(1, 0, 0, 0);
		glm::vec3 scale = glm::vec3(1);

		Transformation() = default;
		Transformation(glm::vec3 pos) : pos(pos) {}
		Transformation(glm::vec3 pos, glm::quat rot) : pos(pos), rot(rot) {}
	};

	struct Velocity : glm::vec3 {
		Velocity(const glm::vec3& vel) : glm::vec3(vel) {}
		operator const glm::vec3& () const { return static_cast<glm::vec3>(*this); }
	};

	struct Renderable {
		LOA::ID instance_id;
	};

	struct Direction {
		glm::vec3 forward = glm::vec3(0, 0, -1);
		glm::vec3 right = glm::vec3(1, 0, 0);
		glm::vec3 up = glm::vec3(0, 1, 0);
	};

	struct MovementState {
		float forward = 0;
		float strafe = 0;
		float fly = 0;
		bool fire = false;
		glm::vec2 rotate = glm::vec2(0);
	};

	struct Input {
		glm::vec2 lastCursorPos = glm::vec2(0);
		bool lastFire = false;
	};

	struct Camera {
		float fov;
		float aspect;
		float near;
		float far;

		entt::entity player;


		float theta = 0;
		float phi = glm::pi<float>() / 4;
		float distance = 10;
	};

	struct PointLight {
		LOA::ID instance_id;
		glm::vec3 color;
		float intensity;
	};

	struct FireParticle {
		float spawn_rate;
		float life_time = 2.0f;
	};

	struct LevelTile {

	};


	//Rigid Bodies that have physics
	struct RigidBody {
		float mass;
		btRigidBody* body;

		RigidBody(float mass=1) : mass(mass), body(nullptr) {}
	};

	//Rigid Bodies that dont have physics: walls, floors
	struct StaticBody {
		glm::vec3 dim;
		btRigidBody* body;
		StaticBody(glm::vec3 dim) : dim(dim), body(nullptr) {}
	};

	//Rigid Bodies for characters
	struct CharacterController {
		btKinematicCharacterController* kinematicCollider = nullptr;
	};
}