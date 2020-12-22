#pragma once

#include <glm/glm.hpp>
#include <entt/core/hashed_string.hpp>

#include "common.h"
#include "TEX.h"

namespace LOA::Graphics {
	using namespace entt;

	enum class ViewPort : u8 {
		SHADOW = 0,			//For the sub camera to create the shadow texture
		FINAL,				//For everything else
		NUM_VIEW_PORTS
	};

	enum class ViewPortLayer : u8 {
		DEFAULT = 0,		//Not really sure
		DEFERRED,			//For lit objects
		FORWARD,			//For non lit or transparent objects
		SKYBOX,				//For the skybox
		NUM_VIEW_PORT_LAYERS
	};

	enum class BlendType : u8 {
		OPAQUE = 0,			//Non transparent objects
		ADD,
		SUB,
		MUL,				//For transparent objects like particles
		NUM_BLEND_TYPES
	};

	enum class MaterialType : u8 {
		BASIC_DEFERRED_ID,
		TRANSLUCENT_BASIC_MATERIAL_ID,
		NORMAL_MATERIAL_ID,
		BASIC_LIT_MATERIAL_ID,
		DISSOLVE_MATERIAL_ID,

		PARTICLE_MATERIAL_ID,
		FIRE_PARTICLE_ID,
		LINE_MATERIAL_ID,

		NUM_MATERIAL_ID
	};

	struct BasicDeferred {
		constexpr static MaterialType Type = MaterialType::BASIC_DEFERRED_ID;
		constexpr static entt::id_type ShaderID = "BasicDeferredShader"_hs;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::DEFERRED;

		entt::id_type diffuse;
	};

	struct TranslucentBasicMaterial {
		constexpr static MaterialType Type = MaterialType::TRANSLUCENT_BASIC_MATERIAL_ID;
		constexpr static entt::id_type ShaderID = "TranslucentShader"_hs;
		constexpr static BlendType DefaultBlend = BlendType::MUL;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;

		entt::id_type diffuse;
		float alpha = .5f;
	};

	struct NormalMaterial {
		constexpr static MaterialType Type = MaterialType::NORMAL_MATERIAL_ID;
		constexpr static entt::id_type ShaderID = "NormalShader"_hs;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;


	};

	struct BasicLitMaterial {
		constexpr static MaterialType Type = MaterialType::BASIC_LIT_MATERIAL_ID;
		constexpr static entt::id_type ShaderID = "BasicLitShader"_hs;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;


		entt::id_type diffuse;
	};

	struct DissolveMaterial {
		constexpr static MaterialType Type = MaterialType::DISSOLVE_MATERIAL_ID;
		constexpr static entt::id_type ShaderID = "DissolveShader"_hs;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;


		entt::id_type diffuse;
		glm::vec3 dissolve_color;
		float offset;
		float time;
	};

	struct ParticleMaterial {
		constexpr static MaterialType Type = MaterialType::PARTICLE_MATERIAL_ID;
		constexpr static entt::id_type ShaderID = "ParticleShader"_hs;
		constexpr static BlendType DefaultBlend = BlendType::MUL;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;


		entt::id_type diffuse;
	};

	struct FireParticleMaterial {
		constexpr static MaterialType Type = MaterialType::FIRE_PARTICLE_ID;
		constexpr static entt::id_type ShaderID = "FireParticleShader"_hs;
		constexpr static BlendType DefaultBlend = BlendType::MUL;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;


		entt::id_type diffuse_grid;
		glm::vec3 base_color;
		glm::vec3 outer_color;
	};

	struct LineMaterial {
		constexpr static MaterialType Type = MaterialType::LINE_MATERIAL_ID;
		constexpr static entt::id_type ShaderID = "LineShader"_hs;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;

	};
}