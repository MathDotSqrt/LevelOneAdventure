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

ParticleSystemInstance::ParticleSystemInstance(ParticleRenderData &&data, MaterialType type, ID matID) 
	: data(std::move(data)), materialType(type), materialID(matID){}

ParticleSystemInstance::ParticleSystemInstance(size_t max, MaterialType type, ID matID)
	: data(max), materialType(type), materialID(matID) {}

entt::resource_handle<Mesh> Scene::loadMesh(entt::id_type id, std::string filename, glm::vec3 offset) {
	return meshCache.load<Graphics::MeshLoader>(id, filename, offset);
}

entt::resource_handle<TEX> Scene::loadTEX(entt::id_type id, std::string filename) {
	return texCache.load<Graphics::TextureLoader>(id, filename);
}

entt::resource_handle<TEX> Scene::loadTEX(entt::id_type id, TEX::Builder settings, std::string filename) {
	return texCache.load<Graphics::TextureLoader>(id, settings, filename);
}

LOA::ID Scene::addInstance(entt::id_type meshID, NormalMaterial material) {
	return instances.insert(Instance{ meshCache.handle(meshID), MaterialType::NORMAL_MATERIAL_ID});
}

LOA::ID Scene::addInstance(entt::id_type meshID, BasicLitMaterial material) {
	const auto id = basicLitMaterials.insert(material);
	return instances.insert(Instance{ meshCache.handle(meshID), MaterialType::BASIC_LIT_MATERIAL_ID, id });
}

LOA::ID Scene::addInstance(entt::id_type meshID, DissolveMaterial material) {
	const auto id = dissolveMaterials.insert(material);
	return instances.insert(Instance(meshCache.handle(meshID), MaterialType::DISSOLVE_MATERIAL_ID, id));
}

LOA::ID Scene::addPointLight(PointLight light) {
	if (pointLights.size() < MAX_POINT_LIGHTS)
		return pointLights.insert(light);
	else 
		return NullID;
}

LOA::ID Scene::createParticleInstance(size_t max, ParticleMaterial material) {
	const auto id = particleMaterials.insert(material);
	return particleSystemInstances.insert(ParticleSystemInstance{max, MaterialType::PARTICLE_MATERIAL_ID, id});
}

LOA::ID Scene::createParticleInstance(size_t max, FireParticleMaterial material) {
	const auto id = fireParticleMaterials.insert(material);
	return particleSystemInstances.insert(ParticleSystemInstance{ max, MaterialType::FIRE_PARTICLE_ID, id });
}

Instance& Scene::getInstance(ID id) {
	return instances[id];
}

PointLight& Scene::getPointLight(ID id) {
	return pointLights[id];
}

ParticleSystemInstance& Scene::getParticleSystemInstance(ID id) {
	return particleSystemInstances[id];
}

DissolveMaterial& Scene::getDissolveMaterial(ID id) {
	return dissolveMaterials[id];
}

void Scene::setMainCamera(PerspectiveCamera camera) {
	mainCamera = camera;
}

PerspectiveCamera& Scene::getMainCamera() {
	return mainCamera;
}