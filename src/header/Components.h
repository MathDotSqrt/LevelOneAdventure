#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>

#include "common.h"
#include "Util/PackedFreeList.h"

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
		glm::vec2 rotate = glm::vec2(0);
	};

	struct Input {
		glm::vec2 lastCursorPos = glm::vec2(0);
	};

	struct Camera {
		float fov;
		float aspect;
		float near;
		float far;
	};

	struct Dissolve {
		float time = 0;
		float offset = 0;
		glm::vec3 dissolve_color = glm::vec3(1);
	};

	struct PointLight {
		LOA::ID instance_id;
		glm::vec3 color;
		float intensity;
	};
}