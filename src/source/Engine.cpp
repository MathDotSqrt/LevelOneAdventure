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

}

entt::registry& Engine::getRegistry() {
	return registry;
}

