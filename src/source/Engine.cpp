#include "Engine.h"

using namespace LOA;

Engine::Engine() {

}

void Engine::update(float delta) {
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
