#pragma once

#include <entt/entt.hpp>
#include <vector>
#include <memory>

#include "Systems/System.h"
namespace LOA {
	class Engine {
	public:
		Engine();

		void update(float delta);
		void render();

		template<typename T>
		void addSystem() {
			systems.push_back(std::make_unique<T>());

			//dereference this pointer
			Engine& reference = *this;
			systems.back()->init(reference);
		}

		entt::registry& getRegistry();

	private:
		entt::registry registry;
		std::vector<std::unique_ptr<Systems::BaseSystem>> systems;
	};
}