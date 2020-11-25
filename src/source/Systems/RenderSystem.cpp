#include "Systems/RenderSystem.h"
#include "Engine.h"
#include "Components.h"

using namespace LOA::Systems;

void RenderSystem::update(Engine &engine, float delta) {
	using namespace Component;

	auto& scene = engine.getScene();
	auto& registry = engine.getRegistry();

	auto view = registry.view<Transformation, Renderable>();

	for (auto entity : view) {
		auto& trans = view.get<Transformation>(entity);
		auto& renderable = view.get<Renderable>(entity);


		auto& instance = scene.getInstance(renderable.instance_id);
		instance.pos = trans.pos;
		instance.rot = trans.rot;
		instance.scale = trans.scale;
	}
}
