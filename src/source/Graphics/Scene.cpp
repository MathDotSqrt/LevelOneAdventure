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

entt::resource_handle<Mesh> Scene::loadMesh(entt::id_type id, std::string filename, glm::vec3 offset, glm::vec3 scale) {
	return meshCache.reload<Graphics::MeshLoader>(id, filename, offset, scale);
}

entt::resource_handle<TEX> Scene::loadTEX(entt::id_type id, std::string filename) {
	return texCache.reload<Graphics::TextureLoader>(id, filename);
}

entt::resource_handle<TEX> Scene::loadTEX(entt::id_type id, TEX::Builder settings, std::string filename) {
	return texCache.reload<Graphics::TextureLoader>(id, settings, filename);
}

LOA::ID Scene::addInstance(entt::id_type meshID) {
	return instances.insert(Instance{meshCache.handle(meshID), MaterialType::NUM_MATERIAL_ID});
}

void Scene::removeInstance(ID id) {
	assert(instances.has(id));

	Instance& instance = instances[id];
	ID materialID = instance.materialID;
	MaterialType type = instance.materialType;

	switch (type) {
	case MaterialType::BASIC_LIT_MATERIAL_ID:
		basicLitMaterials.remove(materialID);
		break;
	case MaterialType::DISSOLVE_MATERIAL_ID:
		dissolveMaterials.remove(materialID);
		break;
	default:
		break;
	}
}

void Scene::newMaterial(LOA::ID id, NormalMaterial material) {
	auto& instance = instances[id];
	removeMaterial(instance.materialID, instance.materialType);
	instance.materialID = LOA::NullID;
	instance.materialType = MaterialType::NORMAL_MATERIAL_ID;
}

void Scene::newMaterial(LOA::ID id, BasicLitMaterial material) {
	auto& instance = instances[id];
	removeMaterial(instance.materialID, instance.materialType);
	instance.materialID = basicLitMaterials.insert(material);
	instance.materialType = MaterialType::BASIC_LIT_MATERIAL_ID;
}

void Scene::newMaterial(LOA::ID id, DissolveMaterial material) {
	auto& instance = instances[id];
	removeMaterial(instance.materialID, instance.materialType);
	instance.materialID = dissolveMaterials.insert(material);
	instance.materialType = MaterialType::DISSOLVE_MATERIAL_ID;
}

void Scene::removeMaterial(LOA::ID id, MaterialType type) {
	switch (type) {
	case MaterialType::NORMAL_MATERIAL_ID:
		break;
	case MaterialType::BASIC_LIT_MATERIAL_ID:
		basicLitMaterials.remove(id);
		break;
	case MaterialType::DISSOLVE_MATERIAL_ID:
		dissolveMaterials.remove(id);
		break;
	default:
		break;
	}
}

LOA::ID Scene::addPointLight() {
	return addPointLight(PointLight{});
}

LOA::ID Scene::addPointLight(PointLight light) {
	if (pointLights.size() < MAX_POINT_LIGHTS)
		return pointLights.insert(light);
	else 
		return NullID;
}

void Scene::removePointLight(ID id) {
	pointLights.remove(id);
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