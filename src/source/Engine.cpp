#include "Engine.h"
#include "Physics/PhysicsScene.h"
#include "Physics/PhysicsDebugDrawer.h"
#include "Window.h"

using namespace LOA;

Engine::Engine() {
	registry.set<Physics::PhysicsScene>();

	player = registry.create();
	mainCamera = registry.create();
}

void Engine::update(float delta) {

	for (auto& system : systems) {
		system->update(delta);
	}
	if (Window::getInstance().isPressed('m'))
		getPhysicsScene().getDrawer()->setDebugMode(1);
	else if (Window::getInstance().isPressed('n'))
		getPhysicsScene().getDrawer()->setDebugMode(0);

	getRegistry().destroy(to_delete.begin(), to_delete.end());
	to_delete.clear();

}

void Engine::render() {
	Physics::PhysicsScene& physicsScene = getPhysicsScene();

	//we pass the physics scene to the renderer if we want to debug draw
	Physics::PhysicsScene* physics = physicsScene.getDrawer()->getDebugMode() == 0 ? nullptr : &physicsScene;
	renderer.render(scene, physics);

	//We prerender for the next frame because game physics is one frame behind the renderer
	physicsScene.prerender();
}

void Engine::deleteEntity(entt::entity entity) {
	to_delete.insert(entity);
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

const entt::entity& Engine::getPlayer() const {
	return player;
}

const entt::entity& Engine::getMainCamera() const {
	return mainCamera;
}
