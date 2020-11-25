#include "Systems/MovementSystem.h"
#include "Engine.h"
#include "Components.h"

using namespace LOA::Systems;

void MovementSystem::update(Engine &engine, float delta) {
	using namespace Component;

	auto& registry = engine.getRegistry();

	auto view = registry.view<Transformation, Velocity>();

	for (auto entity : view) {
		auto& transformation = view.get<Transformation>(entity);
		auto& velocity = view.get<Velocity>(entity);
		transformation.pos += velocity * delta;

		transformation.rot *= glm::angleAxis(delta, glm::vec3(0, 1, 0));
	}
}