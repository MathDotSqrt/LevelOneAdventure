#include "Engine.h"
#include "reactphysics3d/engine/PhysicsCommon.h"
#include "reactphysics3d/engine/PhysicsWorld.h"

using namespace LOA;

Engine::Engine() {
	registry.set<reactphysics3d::PhysicsCommon>();
	auto& physics = registry.ctx<reactphysics3d::PhysicsCommon>();
	registry.set<reactphysics3d::PhysicsWorld*>(physics.createPhysicsWorld());
}

void Engine::update(float delta){
	for (auto& system : systems) {
		system->update(*this, delta);
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

reactphysics3d::PhysicsCommon& Engine::getPhysicsCommon() {
	return registry.ctx<reactphysics3d::PhysicsCommon>();
}

reactphysics3d::PhysicsWorld& Engine::getPhysicsWorld() {
	return *registry.ctx<reactphysics3d::PhysicsWorld*>();
}