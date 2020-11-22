#pragma once

#include <glm/glm.hpp>
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Util/PackedFreeList.h"

namespace LOA::Graphics {
	struct PerspectiveCamera {
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

	class Scene() {

	private:
		Util::PackedFreeList<Mesh> meshs;

		Util::PackedFreeList<Mesh> meshs;
		Util::PackedFreeList<ColorMaterial> colorMaterials;
		Util::PackedFreeList<NormalMaterial> normalMaterials;
		Util::PackedFreeList<BasicLitMaterial> basicLitMaterials;
	}
}