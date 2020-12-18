#include "Engine.h"
#include "Physics/PhysicsScene.h"
#include "Physics/PhysicsDebugDrawer.h"
#include "Window.h"

using namespace LOA;

Engine::Engine() {
	registry.set<Physics::PhysicsScene>();
}

void Engine::update(float delta){
	for (auto& system : systems) {
		system->update(delta);
	}
	if(Window::getInstance().isPressed('m'))
		getPhysicsScene().getDrawer()->setDebugMode(1);
	else if(Window::getInstance().isPressed('n'))
		getPhysicsScene().getDrawer()->setDebugMode(0);
}

void Engine::render() {
	Physics::PhysicsScene& physicsScene = getPhysicsScene();
	physicsScene.render();
	renderer.render(scene, &physicsScene);
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
