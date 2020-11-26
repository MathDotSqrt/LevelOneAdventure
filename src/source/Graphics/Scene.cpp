#include "Graphics/Scene.h"

using namespace LOA::Graphics;

PerspectiveCamera::PerspectiveCamera() 
	: fov(glm::radians(90.0f)), aspect(1), near(.1f), far(1000.0f){}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
	: fov(fov), aspect(aspect), near(near), far(far) {}

Instance::Instance(entt::resource_handle<Mesh> mesh, MaterialType type, ID matID)
	: mesh(mesh), materialType(type), materialID(matID) {}

Instance::Instance(entt::resource_handle<Mesh> mesh, MaterialType type)
	: mesh(mesh), materialType(type), materialID(ID{0, 0}) {}

entt::resource_handle<Mesh> Scene::loadMesh(entt::id_type id, std::string filename) {
	return meshCache.load<Graphics::MeshLoader>(id, filename);
}

entt::resource_handle<TEX> Scene::loadTEX(entt::id_type id, std::string filename) {
	return texCache.load<Graphics::TextureLoader>(id, filename);
}

entt::resource_handle<TEX> Scene::loadTEX(entt::id_type id, TEX::Builder settings, std::string filename) {
	return texCache.load<Graphics::TextureLoader>(id, settings, filename);
}

LOA::ID Scene::addInstance(entt::id_type meshID, ColorMaterial material) {
	const auto id = colorMaterials.insert(material);
	return instances.insert(Instance(meshCache.handle(meshID), MaterialType::COLOR_MATERIAL_ID, id));
}

LOA::ID Scene::addInstance(entt::id_type meshID, NormalMaterial material) {
	return instances.insert(Instance{ meshCache.handle(meshID), MaterialType::NORMAL_MATERIAL_ID});
}

LOA::ID Scene::addInstance(entt::id_type meshID, BasicLitMaterial material) {
	const auto id = basicLitMaterials.insert(material);
	return instances.insert(Instance{ meshCache.handle(meshID), MaterialType::BASIC_LIT_MATERIAL_ID, id });
}

LOA::ID Scene::addPointLight(PointLight light) {
	if (pointLights.size() < MAX_POINT_LIGHTS)
		return pointLights.insert(light);
	else 
		return NullID;
}

Instance& Scene::getInstance(ID id) {
	return instances[id];
}


void Scene::setMainCamera(PerspectiveCamera camera) {
	mainCamera = camera;
}

PerspectiveCamera& Scene::getMainCamera() {
	return mainCamera;
}