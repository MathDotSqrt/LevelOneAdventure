#pragma once

#include <glm/glm.hpp>

#include "common.h"
#include "TEX.h"

namespace LOA::Graphics {
	enum class MaterialID : u8 {
		COLOR_MATERIAL_ID,
		NORMAL_MATERIAL_ID,
		BASIC_LIT_MATERIAL_ID,
		NUM_MATERIAL_ID
	};

	struct ColorMaterial {
		constexpr static MaterialID TYPE = MaterialID::COLOR_MATERIAL_ID;
		glm::vec3 color;
	};

	struct NormalMaterial {
		constexpr static MaterialID TYPE = MaterialID::NORMAL_MATERIAL_ID;
	};

	struct BasicLitMaterial {
		constexpr static MaterialID TYPE = MaterialID::BASIC_LIT_MATERIAL_ID;
		TEX diffuse;
	};
}