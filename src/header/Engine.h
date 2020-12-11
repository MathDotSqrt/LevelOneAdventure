#pragma once

#include <entt/entt.hpp>
#include <vector>
#include <memory>

#include "Graphics/Scene.h"
#include "Graphics/BasicRenderer.h"
#include "Systems/System.h"
#include "Physics/PhysicsScene.h"

namespace reactphysics3d {
	class PhysicsCommon;
	class PhysicsWorld;
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
			systems.push_back(std::make_unique<T>());

			//dereference this pointer
			Engine& reference = *this;
			systems.back()->init(reference);
		}

		entt::registry& getRegistry();
		Graphics::Scene& getScene();
		Graphics::BasicRenderer& getRenderer();
		Physics::PhysicsScene& getPhysicsScene();
	private:
		entt::registry registry;
		Graphics::Scene scene;
		Graphics::BasicRenderer renderer;
		Physics::PhysicsScene physicsScene;

		std::vector<std::unique_ptr<Systems::BaseSystem>> systems;
	};
}