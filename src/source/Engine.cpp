#include "Engine.h"
#include "q3.h"

using namespace LOA;

Engine::Engine() {
	registry.set<q3Scene>(1/60.0f);
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

q3Scene& Engine::getPhysicsScene() {
	return registry.ctx<q3Scene>();
}

Graphics::BasicRenderer& Engine::getRenderer() {
	return renderer;
}
