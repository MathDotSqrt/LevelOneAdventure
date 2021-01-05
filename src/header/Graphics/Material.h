#pragma once

#include <glm/glm.hpp>
#include <entt/core/hashed_string.hpp>

#include "RenderStateKey.h"
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
		DEFERRED = 0,		//For lit objects
		DEFERRED_LIGHT,		//For the light volumes
		FORWARD,			//For non lit or transparent objects
		DEBUG,				//Any object without postprocessing
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
		BASIC_MATERIAL_ID,
		BASIC_LIT_MATERIAL_ID,
		DISSOLVE_MATERIAL_ID,
		NORMAL_MATERIAL_ID,
		LINE_MATERIAL_ID,
		PARTICLE_MATERIAL_ID,
		FIRE_PARTICLE_ID,
		LIGHT_VOLUME_MATERIAL_ID,

		RENDER_COMMAND_ID,
		NUM_MATERIAL_ID
	};

	constexpr u32 calc_enum_size(u32 v) {
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;

		u32 r = 0;
		while (v >>= 1) {
			r++;
		}
		return r;
	}

	class RenderStateKey {
	public:
		typedef u32 KeyType;

		RenderStateKey(
			MaterialType material);

		RenderStateKey(
			BlendType type,
			MaterialType material);

		RenderStateKey(
			ViewPortLayer layer,
			BlendType type,
			MaterialType material);

		RenderStateKey(
			ViewPort port,
			ViewPortLayer layer,
			BlendType blend,
			MaterialType material);

		bool operator<(const RenderStateKey& other) const;
		bool operator==(const RenderStateKey& other) const;

		void setMaterialType(MaterialType id);
		void setBlendType(BlendType type);
		void setViewPortLayer(ViewPortLayer layer);
		void setViewPort(ViewPort port);

		MaterialType getMaterialType() const;
		BlendType getBlendType() const;
		ViewPortLayer getViewPortLayer() const;
		ViewPort getViewPort() const;

	private:
		constexpr static u32 VIEW_PORT_SIZE = calc_enum_size((u32)ViewPort::NUM_VIEW_PORTS);
		constexpr static u32 VIEW_PORT_LAYER_SIZE = calc_enum_size((u32)ViewPortLayer::NUM_VIEW_PORT_LAYERS);
		constexpr static u32 BLEND_TYPE_SIZE = calc_enum_size((u32)BlendType::NUM_BLEND_TYPES);
		constexpr static u32 MATERIAL_TYPE_SIZE = calc_enum_size((u32)MaterialType::NUM_MATERIAL_ID);

		constexpr static u32 MATERIAL_SHIFT = 0;
		constexpr static u32 BLEND_SHIFT = MATERIAL_TYPE_SIZE + MATERIAL_SHIFT;
		constexpr static u32 LAYER_SHIFT = BLEND_TYPE_SIZE + BLEND_SHIFT;
		constexpr static u32 PORT_SHIFT = VIEW_PORT_LAYER_SIZE + LAYER_SHIFT;

		KeyType key;
	};

	class RenderStateKeyValue {
	public:
		typedef u32 ValueType;

		RenderStateKeyValue(RenderStateKey key, ValueType value);

		bool operator<(const RenderStateKeyValue& keyValue) const;

		RenderStateKey getKey() const;
		ValueType getValue() const;

	private:
		RenderStateKey key;
		ValueType value;
	};

	struct BaseMaterial {
		BaseMaterial(RenderStateKey state) : state(state) {}

		~BaseMaterial() = default;
		
		MaterialType getType() const {
			return state.getMaterialType();
		}

		const RenderStateKey& getRenderState() const {
			return state;
		}
	protected:
		RenderStateKey state;
	};

	struct NormalMaterial : public BaseMaterial { 
		constexpr static MaterialType Type = MaterialType::NORMAL_MATERIAL_ID;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;
		constexpr static entt::id_type ShaderID = "NormalShader"_hs;

		NormalMaterial() : 
			BaseMaterial(RenderStateKey{ DefaultLayer, DefaultBlend, Type}) {}

	};

	struct BasicMaterial : public BaseMaterial {
		constexpr static MaterialType Type = MaterialType::BASIC_MATERIAL_ID;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;
		constexpr static entt::id_type ShaderID = "BasicShader"_hs;

		BasicMaterial() :
			BaseMaterial(RenderStateKey{ DefaultLayer, DefaultBlend, Type }) {}



		void setColor(glm::vec3 color) {
			this->color = color;
		}

		const glm::vec3& getColor() const {
			return color;
		}

		void setAlpha(float alpha) {
			alpha = glm::clamp(alpha, 0.0f, 1.0f);
			if (alpha == 1) {
				state.setBlendType(BlendType::OPAQUE);
			}
			else {
				state.setBlendType(BlendType::MUL);
			}
			this->alpha = alpha;
		}

		float getAlpha() const {
			return alpha;
		}

		void setTexture(entt::id_type textureID) {
			this->textureID = textureID;
		}

		entt::id_type getTexture() const {
			return textureID;
		}

	protected:
		glm::vec3 color = glm::vec3(1);
		float alpha = 1;
		entt::id_type textureID = 0;

	};

	struct LineMaterial : public BaseMaterial {
		constexpr static MaterialType Type = MaterialType::LINE_MATERIAL_ID;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::DEBUG;
		constexpr static entt::id_type ShaderID = "LineShader"_hs;

		LineMaterial() :
			BaseMaterial(RenderStateKey{ DefaultLayer, DefaultBlend, Type }) {}

		glm::vec3 color = glm::vec3(1);
	};

	struct BasicLitMaterial : public BaseMaterial {
		constexpr static MaterialType Type = MaterialType::BASIC_LIT_MATERIAL_ID;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::DEFERRED;
		constexpr static entt::id_type ShaderID = "BasicLitShader"_hs;

		BasicLitMaterial() :
			BaseMaterial(RenderStateKey{ DefaultLayer, DefaultBlend, Type }) {}

		glm::vec3 color = glm::vec3(1);
		entt::id_type texture = 0;

	};

	struct DissolveMaterial : public BaseMaterial{
		constexpr static MaterialType Type = MaterialType::DISSOLVE_MATERIAL_ID;
		constexpr static BlendType DefaultBlend = BlendType::OPAQUE;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::DEFERRED;
		constexpr static entt::id_type ShaderID = "DissolveShader"_hs;

		DissolveMaterial() : BaseMaterial(RenderStateKey{ DefaultLayer, DefaultBlend, Type }) {}


		entt::id_type texture = 0;
		glm::vec3 color = glm::vec3(1);
		glm::vec3 dissolve_color = glm::vec3(1);
		float offset = .01;
		float time = 0;
		
		
	};

	struct FireParticleMaterial : public BaseMaterial {
		constexpr static MaterialType Type = MaterialType::FIRE_PARTICLE_ID;
		constexpr static BlendType DefaultBlend = BlendType::MUL;
		constexpr static ViewPortLayer DefaultLayer = ViewPortLayer::FORWARD;
		constexpr static entt::id_type ShaderID = "FireParticleShader"_hs;

		FireParticleMaterial() : BaseMaterial(RenderStateKey{ DefaultLayer, DefaultBlend, Type }) {}

		entt::id_type diffuse_grid = 0;

		glm::vec3 base_color = glm::vec3(1);
		glm::vec3 outer_color = glm::vec3(1);
	};

	
}