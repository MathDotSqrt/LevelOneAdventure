#pragma once

#include <entt/entt.hpp>
#include <vector>
#include <unordered_set>
#include <memory>

#include "Graphics/Scene.h"
#include "Graphics/BasicRenderer.h"
#include "Systems/System.h"

namespace LOA::Physics {
	class PhysicsScene;
}

namespace LOA {
	class Engine {
	public:
		Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void update(float delta);
		void render();

		template<typename T>
		void addSystem() {
			systems.push_back(std::make_unique<T>(*this));
		}

		void initSystems();

		void deleteEntity(entt::entity entity);
		void batchDelete();

		entt::registry& getRegistry();
		Graphics::Scene& getScene();
		Graphics::BasicRenderer& getRenderer();
		Physics::PhysicsScene& getPhysicsScene();
		std::pair<bool, glm::vec3> getMouseCast();

		const entt::entity& getPlayer() const;
		const entt::entity& getMainCamera() const;
	private:
		void computeMouseCast();
		entt::entity player;
		entt::entity mainCamera;
		
		entt::registry registry;
		Graphics::Scene scene;
		Graphics::BasicRenderer renderer;

		std::vector<std::unique_ptr<Systems::BaseSystem>> systems;
		std::unordered_set<entt::entity> to_delete;

		std::pair<bool,glm::vec3> mousecastposition;
	};
}