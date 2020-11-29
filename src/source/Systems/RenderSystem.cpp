#include "Systems/RenderSystem.h"
#include "Engine.h"
#include "Components.h"

using namespace LOA;
using namespace LOA::Systems;

glm::mat4 makeTransform(const Component::Transformation &transform) {
	constexpr auto identity = glm::identity<glm::mat4>();
	glm::mat4 translation = glm::translate(identity, transform.pos);
	glm::mat4 rotation = translation * glm::toMat4(transform.rot);
	glm::mat4 transformation = glm::scale(rotation, transform.scale);
	return transformation;
}

glm::mat4 makeViewTransform(const Component::Transformation& transform) {
	constexpr auto identity = glm::identity<glm::mat4>();

	glm::mat4 translation = glm::translate(identity, -transform.pos);
	//glm::mat4 transformation = glm::scale(translation, transform.scale);

	//glm::mat4 transformation = glm::inverse(glm::toMat4(transform.rot));
	glm::mat4 transformation = glm::inverse(glm::toMat4(transform.rot)) * translation;

	return transformation;
}

void RenderSystem::update(Engine &engine, float delta) {
	using namespace Component;

	auto& scene = engine.getScene();
	auto& registry = engine.getRegistry();

	auto mesh_view = registry.view<Transformation, Renderable>();
	for (auto entity : mesh_view) {
		auto& trans = mesh_view.get<Transformation>(entity);
		auto& renderable = mesh_view.get<Renderable>(entity);

		auto& instance = scene.getInstance(renderable.instance_id);
		instance.transform = makeTransform(trans);;
	}

	auto point_view = registry.view<Transformation, PointLight>();
	for (auto entity : point_view) {
		auto& trans = point_view.get<Transformation>(entity);
		auto& point = point_view.get<PointLight>(entity);

		auto& instance = scene.getPointLight(point.instance_id);
		instance.position = trans.pos;
		instance.color = point.color;
		instance.intensity = point.intensity;
	}

	auto dissolve_view = registry.view<Renderable, Dissolve>();
	for (auto entity : dissolve_view) {
		auto& render = dissolve_view.get<Renderable>(entity);
		auto& dissolve = dissolve_view.get<Dissolve>(entity);

		auto& instance = scene.getInstance(render.instance_id);
		auto& material = scene.getDissolveMaterial(instance.materialID);

		material.time = dissolve.time;
		material.offset = dissolve.offset;
		material.dissolve_color = dissolve.dissolve_color;
	}

	auto camera_view = registry.view<Transformation, Camera>();
	for (auto entity : camera_view) {
		auto& trans = camera_view.get<Transformation>(entity);
		auto& camera = camera_view.get<Camera>(entity);

		auto& scene_camera = scene.getMainCamera();
		scene_camera.transform = makeViewTransform(trans);
		scene_camera.fov = camera.fov;
	}
}