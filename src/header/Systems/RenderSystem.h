#pragma once

#include "Systems/System.h"
#include <entt/entt.hpp>
#include "Components.h"

namespace LOA::Systems {
	class RenderSystem : public BaseSystem {
	public:
		RenderSystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
	private:
		void deleteInstance(entt::registry& registry, entt::entity entity);
		void deletePointLight(entt::registry& registry, entt::entity entity);

		template<typename MATERIAL>
		void copyMaterial() {
			auto& scene = engine.getScene();
			auto& registry = engine.getRegistry();

			auto view = registry.group<MATERIAL>(entt::get<Component::Renderable>);
			for (auto entity : view) {
				auto& render = view.get<Component::Renderable>(entity);
				auto& component_material = view.get<MATERIAL>(entity);

				auto& instance = scene.getInstance(render.instance_id);

				if (instance.material->getType() != MATERIAL::Type) {
					scene.changeMaterial(render.instance_id, component_material);
				}
				else {
					MATERIAL& material = scene.castMaterial<MATERIAL>(instance.material);
					material = component_material;
				}
			}
		}
	};
}