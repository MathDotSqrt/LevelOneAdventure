#include "Util/TransformUtil.h"
#include <glm/gtx/transform.hpp>
#include "glm/gtx/vector_angle.hpp"
using namespace LOA::Util;

glm::mat4 LOA::Util::make_transform(const Component::Transformation& transform) {
	return make_transform(transform.pos, transform.rot, transform.scale);
}

glm::mat4 LOA::Util::make_transform(glm::vec3 pos, glm::quat rot, glm::vec3 scale) {
	const auto identity = glm::identity<glm::mat4>();
	glm::mat4 translation = glm::translate(identity, pos);
	glm::mat4 rotation = translation * glm::toMat4(rot);
	glm::mat4 transformation = glm::scale(rotation, scale);
	return transformation;
}

glm::mat4 LOA::Util::rotate_around_center(glm::vec3 offset, glm::quat rot) {
	const auto identity = glm::identity<glm::mat4>();

	glm::mat4 translation = glm::translate(identity, -offset);
	glm::mat4 rotation = glm::toMat4(rot);
	glm::mat4 back = glm::translate(identity, offset);

	return identity;//glm::translate(glm::translate(rotation, offset), -offset);
}

glm::mat4 LOA::Util::make_transform_offset(glm::vec3 offset, glm::vec3 pos, glm::quat rot, glm::vec3 scale) {
	const auto identity = glm::identity<glm::mat4>();

	glm::mat4 rotation_m = glm::toMat4(rot);

	glm::mat4 translation = glm::translate(identity, pos);
	glm::mat4 rotation = translation * glm::toMat4(rot);
	glm::mat4 offset_m = glm::translate(rotation, -offset);
	glm::mat4 transformation = glm::scale(offset_m, scale);
	return transformation;
}
glm::quat LOA::Util::turn_towards(glm::vec2 forward, glm::vec2 targetDir) {
	float angle = glm::orientedAngle(glm::normalize(forward), glm::normalize(targetDir));
	glm::quat newrot = glm::angleAxis(-angle, glm::vec3(0, 1, 0));
	return newrot;
}


glm::quat LOA::Util::quarter_rot(int r) {
	return glm::angleAxis(glm::pi<float>() * r / 2.0f, glm::vec3(0, 1, 0));
}