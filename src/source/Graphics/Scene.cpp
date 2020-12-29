#include "Graphics/Scene.h"

using namespace LOA::Graphics;

PerspectiveCamera::PerspectiveCamera() 
	: fov(glm::radians(90.0f)), aspect(1), near(.1f), far(1000.0f){}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
	: fov(fov), aspect(aspect), near(near), far(far) {}

Instance::Instance(entt::resource_handle<Mesh> mesh, MaterialType type, ID matID)
	: mesh(mesh), materialType(type), materialID(matID) {}

Instance::Instance(entt::resource_handle<Mesh> mesh, MaterialType type)
	: mesh(mesh), materialType(type), materialID(LOA::NullID) {}

ParticleSystemInstance::ParticleSystemInstance(ParticleRenderData &&data, MaterialType type, ID matID) 
	: data(std::move(data)), materialType(type), materialID(matID){}

ParticleSystemInstance::ParticleSystemInstance(size_t max, MaterialType type, ID matID)
	: data(max), materialType(type), materialID(matID) {}

Scene::Scene() {
	setAmbientLight(AmbientLight{glm::vec3(0), 0});
	setDirLight(DirLight{ glm::vec3(0), glm::vec3(0, -1, 0), 0 });
}

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

	removeMaterial(type, materialID);

	instances.remove(id);
}


LOA::ID Scene::addPointLight() {
	return addPointLight(PointLight{});
}

LOA::ID Scene::addPointLight(PointLight light) {
	return pointLights.insert(light);
}

void Scene::removePointLight(ID id) {
	pointLights.remove(id);
}

LOA::ID Scene::createParticleInstance(size_t max, ParticleMaterial material) {
	const auto id = insertMaterial(material);
	return particleSystemInstances.insert(ParticleSystemInstance{max, MaterialType::PARTICLE_MATERIAL_ID, id});
}

LOA::ID Scene::createParticleInstance(size_t max, FireParticleMaterial material) {
	const auto id = insertMaterial(material);
	return particleSystemInstances.insert(ParticleSystemInstance{ max, MaterialType::FIRE_PARTICLE_ID, id });
}

void Scene::removeMaterial(MaterialType type, ID id){
	if (type != MaterialType::NUM_MATERIAL_ID) {
		freeListReferenceMap[type]->remove(id);
	}
}

BlendType Scene::getMaterialBlendType(MaterialType type) const {
	return blendMap.at(type);
}

ViewPortLayer Scene::getMaterialViewPortLayer(MaterialType type) const {
	return viewPortLayerMap.at(type);
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

void Scene::setMainCamera(PerspectiveCamera camera) {
	mainCamera = camera;
}

PerspectiveCamera& Scene::getMainCamera() {
	return mainCamera;
}

glm::vec3 Scene::getCameraPos() const {
	return glm::vec3(mainCamera.transform[3]);
}

void Scene::setAmbientLight(AmbientLight light) {
	ambient = light;
}

AmbientLight Scene::getAmbientLight() const {
	return ambient;
}

void Scene::setDirLight(DirLight light) {
	dirLight = light;
}

DirLight Scene::getDirLight() const {
	return dirLight;
}

const PerspectiveCamera& Scene::getMainCamera() const {
	return mainCamera;
}