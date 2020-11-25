#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>

#include "common.h"
#include "Util/PackedFreeList.h"

namespace LOA::Component {
	struct Transformation {
		glm::vec3 pos;
		glm::quat rot;
		glm::vec3 scale;
	};

	struct Velocity {
		glm::vec3 vel;

		operator glm::vec3() const { return vel; }
	};

	struct Renderable {
		LOA::ID instance_id;
	};

	struct Camera {
		glm::vec3 eye;
		glm::vec3 target;
		glm::vec3 up;

		float fov;
		float aspect;
		float near;
		float far;
	};

	struct PointLight {
		glm::vec3 position;
		glm::vec3 color;
		float intensity;
	};
}