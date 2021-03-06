#include "Systems/RenderSystem.h"
#include "Engine.h"
#include "Components.h"
#include "Util/Timer.h"
#include "Window.h"

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

void RenderSystem::deleteInstance(entt::registry& registry, entt::entity entity) {
	using namespace Component;
	Renderable& renderable = registry.get<Renderable>(entity);
	if (renderable.instance_id != LOA::NullID) {
		Graphics::Scene& scene = engine.getScene();

		scene.removeInstance(renderable.instance_id);
	}
}

void RenderSystem::deletePointLight(entt::registry& registry, entt::entity entity) {
	using namespace Component;
	PointLight& light = registry.get<PointLight>(entity);
	if (light.instance_id != LOA::NullID) {
		Graphics::Scene& scene = engine.getScene();

		scene.removePointLight(light.instance_id);
	}
}


void RenderSystem::init() {
	auto& registry = engine.getRegistry();
	
	registry.on_destroy<Component::Renderable>().connect<&RenderSystem::deleteInstance>(this);
	registry.on_destroy<Component::PointLight>().connect<&RenderSystem::deletePointLight>(this);
}

void RenderSystem::update(float delta) {
	using namespace Component;

	Util::Timer timer("RenderSystem");

	auto& scene = engine.getScene();
	auto& registry = engine.getRegistry();

	//Copy mesh state to the renderer
	{
		auto mesh_view = registry.view<Transformation, Renderable>();
		for (auto entity : mesh_view) {
			auto& trans = mesh_view.get<Transformation>(entity);
			auto& renderable = mesh_view.get<Renderable>(entity);

			auto& instance = scene.getInstance(renderable.instance_id);
			instance.transform = makeTransform(trans);;
		}
	}

	//Copy point light state to renderer
	{
		auto point_view = registry.view<Transformation, PointLight>();
		for (auto entity : point_view) {
			auto& trans = point_view.get<Transformation>(entity);
			auto& point = point_view.get<PointLight>(entity);

			auto& instance = scene.getPointLight(point.instance_id);
			instance.position = trans.pos;
			instance.color = point.color;
			instance.intensity = point.intensity;
			instance.radius = point.radius;
		}
	}

	//Copy camera positions to renderer
	{
		auto camera_view = registry.view<Transformation, Camera>();
		for (auto entity : camera_view) {
			auto& trans = camera_view.get<Transformation>(entity);
			auto& camera = camera_view.get<Camera>(entity);

			auto& scene_camera = scene.getMainCamera();
			scene_camera.transform = makeViewTransform(trans);
			scene_camera.fov = camera.fov;
		}
	}



	auto view = registry.view<Graphics::DissolveMaterial>();
	for (auto entity : view) {
		if (Window::getInstance().isDown('x'))
			view.get<Graphics::DissolveMaterial>(entity).time += delta / 2.0f;
		else if (Window::getInstance().isDown('z'))
			view.get<Graphics::DissolveMaterial>(entity).time -= delta / 2.0f;
	}

	//Copy material state to renderer
	{
		copyMaterial<Graphics::BasicMaterial>();
		copyMaterial<Graphics::BasicLitMaterial>();
		copyMaterial<Graphics::DissolveMaterial>();
		copyMaterial<Graphics::FireParticleMaterial>();
	}


}
