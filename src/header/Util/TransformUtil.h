#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Components.h"

namespace LOA::Util {
	glm::mat4 make_transform(const Component::Transformation& transform);
	glm::mat4 make_transform(glm::vec3 pos, glm::quat rot = glm::quat(1, 0, 0, 0), glm::vec3 scale = glm::vec3(1));
	glm::mat4 rotate_around_center(glm::vec3 offset, glm::quat rot);
	glm::mat4 make_transform_offset(glm::vec3 offset, glm::vec3 pos, glm::quat rot=glm::quat(1, 0, 0, 0), glm::vec3 scale=glm::vec3(1));
	glm::quat turn_towards(glm::vec2 forward, glm::vec2 dir);

	glm::quat quarter_rot(int r);
}