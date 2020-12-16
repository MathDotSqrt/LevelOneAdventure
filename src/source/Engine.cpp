#include "Engine.h"
#include "Physics/PhysicsScene.h"

using namespace LOA;

Engine::Engine() {
	registry.set<Physics::PhysicsScene>();
}

void Engine::update(float delta){
	for (auto& system : systems) {
		system->update(delta);
	}
}

void Engine::render() {
	renderer.render(scene);
}

entt::registry& Engine::getRegistry() {
	return registry;
}

Graphics::Scene& Engine::getScene() {
	return scene;
}

Graphics::BasicRenderer& Engine::getRenderer() {
	return renderer;
}

Physics::PhysicsScene& Engine::getPhysicsScene() {
	return 	registry.ctx<Physics::PhysicsScene>();
}
