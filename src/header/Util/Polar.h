#pragma once

#include <glm/glm.hpp>

namespace LOA::Util {
	template<typename T, glm::qualifier Q>
	glm::vec<3, T, Q> polar(const glm::vec<3, T, Q>& cartesian)
	{
		T r = glm::sqrt(glm::dot(cartesian, cartesian));
		T theta = glm::atan(cartesian.y, cartesian.x);
		T phi = glm::acos(cartesian.z / r);

		return glm::vec<3, T, Q>(phi, theta, r);
	}

	template<typename T, glm::qualifier Q>
	glm::vec<3, T, Q> cartesian(const glm::vec<3, T, Q>& polar)
	{
		T sin_phi = glm::sin(polar.x);
		T x = polar.z * sin_phi * glm::cos(polar.y);
		T y = polar.z * sin_phi * glm::sin(polar.y);
		T z = polar.z * glm::cos(polar.x);
		return glm::vec<3, T, Q>(x, z, -y);
	}
}