#pragma once

#include <glm/glm.hpp>
#include <entt/resource/handle.hpp>
#include <entt/core/hashed_string.hpp>

#include "common.h"
#include "TEX.h"

namespace LOA::Graphics {
	using namespace entt;

	enum class ViewPort : u8 {
		SHADOW = 0,
		FINAL,
		NUM_VIEW_PORTS
	};

	enum class ViewPortLayer : u8 {
		DEFAULT = 0,
		WORLD,
		SKYBOX,
		NUM_VIEW_PORT_LAYERS
	};

	enum class BlendType : u8 {
		OPAQUE = 0,
		ADD,
		SUB,
		MUL,
		NUM_BLEND_TYPES
	};

	enum class MaterialType : u8 {
		COLOR_MATERIAL_ID,
		NORMAL_MATERIAL_ID,
		BASIC_LIT_MATERIAL_ID,
		NUM_MATERIAL_ID
	};

	

	struct ColorMaterial {
		constexpr static MaterialType Type = MaterialType::COLOR_MATERIAL_ID;
		constexpr static id_type ShaderID = "ColorShader"_hs;
		
		glm::vec3 color;
	};

	struct NormalMaterial {
		constexpr static MaterialType Type = MaterialType::NORMAL_MATERIAL_ID;
		constexpr static id_type ShaderID = "NormalShader"_hs;

	};

	struct BasicLitMaterial {
		constexpr static MaterialType Type = MaterialType::BASIC_LIT_MATERIAL_ID;
		constexpr static id_type ShaderID = "BasicLitShader"_hs;

		resource_handle<TEX> diffuse;
	};
}