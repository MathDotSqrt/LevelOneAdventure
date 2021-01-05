#include "Graphics/Scene.h"

using namespace LOA::Graphics;

PerspectiveCamera::PerspectiveCamera() 
	: fov(glm::radians(90.0f)), aspect(1), near(.1f), far(1000.0f){}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
	: fov(fov), aspect(aspect), near(near), far(far) {}

Instance::Instance(entt::resource_handle<Mesh> mesh) : mesh(mesh), material(nullptr){}

Scene::Scene() {
	setAmbientLight(AmbientLight{glm::vec3(0), 0});
	setDirLight(DirLight{ glm::vec3(0), glm::vec3(0, -1, 0), 0 });
}

entt::resource_handle<Mesh> Scene::loadTexturedMesh(entt::id_type id, std::string filename, glm::vec3 offset, glm::vec3 scale) {
	return meshCache.reload<Graphics::TexturedMeshLoader>(id, filename, offset, scale);
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
	return instances.insert(Instance{meshCache.handle(meshID)});
}

void Scene::removeInstance(ID id) {
	assert(instances.has(id));

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