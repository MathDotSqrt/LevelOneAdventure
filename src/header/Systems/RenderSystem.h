#pragma once

#include "Systems/System.h"
#include <entt/entt.hpp>

namespace LOA::Systems {
	class RenderSystem : public BaseSystem {
	public:
		RenderSystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
	private:
		void deleteInstance(entt::registry& registry, entt::entity entity);
		void deletePointLight(entt::registry& registry, entt::entity entity);

	};
}