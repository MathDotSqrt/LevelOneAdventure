#include "Graphics/RenderStateKey.h"

using namespace LOA::Graphics;

RenderStateKey::RenderStateKey(
	MaterialType id) : key(0) {

	setMaterialType(id);
	setBlendType(BlendType::OPAQUE);
	setViewPortLayer(ViewPortLayer::DEFAULT);
	setViewPort(ViewPort::FINAL);
}

RenderStateKey::RenderStateKey(
	BlendType type,
	MaterialType id) : key(0) {

	setMaterialType(id);
	setBlendType(type);
	setViewPortLayer(ViewPortLayer::FORWARD);
	setViewPort(ViewPort::FINAL);
}

RenderStateKey::RenderStateKey(
	ViewPortLayer layer,
	BlendType type,
	MaterialType id) : key(0) {

	setMaterialType(id);
	setBlendType(type);
	setViewPortLayer(layer);
	setViewPort(ViewPort::FINAL);
}

RenderStateKey::RenderStateKey(
	ViewPort port,
	ViewPortLayer layer,
	BlendType type,
	MaterialType id) : key(0) {

	setMaterialType(id);
	setBlendType(type);
	setViewPortLayer(layer);
	setViewPort(port);
}

bool RenderStateKey::operator<(const RenderStateKey& other) const {
	return key < other.key;
}

bool RenderStateKey::operator==(const RenderStateKey& other) const {
	return key == other.key;
}

void RenderStateKey::setMaterialType(MaterialType id) {
	KeyType v = static_cast<KeyType>(id);
	KeyType bits = (1 << MATERIAL_TYPE_SIZE) - 1;
	KeyType mask = ~(bits << MATERIAL_SHIFT);
	key = (key & mask) | (v << MATERIAL_SHIFT);
}

void RenderStateKey::setBlendType(BlendType type) {
	KeyType v = static_cast<KeyType>(type);
	KeyType bits = (1 << BLEND_TYPE_SIZE) - 1;
	KeyType mask = ~(bits << BLEND_SHIFT);
	key = (key & mask) | (v << BLEND_SHIFT);
}

void RenderStateKey::setViewPortLayer(ViewPortLayer layer) {
	KeyType v = static_cast<KeyType>(layer);
	KeyType bits = (1 << VIEW_PORT_LAYER_SIZE) - 1;
	KeyType mask = ~(bits << LAYER_SHIFT);
	key = (key & mask) | (v << LAYER_SHIFT);
}

void RenderStateKey::setViewPort(ViewPort port) {
	KeyType v = static_cast<KeyType>(port);
	KeyType bits = (1 << VIEW_PORT_SIZE) - 1;
	KeyType mask = ~(bits << PORT_SHIFT);
	key = (key & mask) | (v << PORT_SHIFT);
}

MaterialType RenderStateKey::getMaterialType() const {
	constexpr u32 mask = (1 << MATERIAL_TYPE_SIZE) - 1;
	return static_cast<MaterialType>((key >> MATERIAL_SHIFT) & mask);
}

BlendType RenderStateKey::getBlendType() const {
	constexpr u32 mask = (1 << BLEND_TYPE_SIZE) - 1;
	return static_cast<BlendType>((key >> BLEND_SHIFT) & mask);
}

ViewPortLayer RenderStateKey::getViewPortLayer() const {
	constexpr u32 mask = (1 << VIEW_PORT_LAYER_SIZE) - 1;
	return static_cast<ViewPortLayer>((key >> LAYER_SHIFT) & mask);
}

ViewPort RenderStateKey::getViewPort() const {
	constexpr u32 mask = (1 << VIEW_PORT_SIZE) - 1;
	return static_cast<ViewPort>((key >> PORT_SHIFT) & mask);
}


RenderStateKeyValue::RenderStateKeyValue(RenderStateKey key, RenderStateKeyValue::ValueType value) 
	: key(key), value(value){}

bool RenderStateKeyValue::operator<(const RenderStateKeyValue& other) const {
	return key == other.key ? value < other.value : key < other.key;
}

RenderStateKey RenderStateKeyValue::getKey() const {
	return key;
}

RenderStateKeyValue::ValueType RenderStateKeyValue::getValue() const {
	return value;
}