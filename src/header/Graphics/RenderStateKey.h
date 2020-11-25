#pragma once
#include "common.h"
#include "Graphics/Material.h"

namespace LOA::Graphics {
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
}