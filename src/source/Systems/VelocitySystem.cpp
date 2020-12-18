#include "Systems/VelocitySystem.h"
#include "Engine.h"
#include "Components.h"

using namespace LOA::Systems;

void VelocitySystem::update(float delta) {
	using namespace Component;

	auto& registry = engine.getRegistry();
	auto view = registry.view<Transformation, Velocity>(entt::exclude<RigidBody, CharacterController>);

	//for (auto entity : a_view) {
	//	view.get<Velocity>(entity).y -= 20 * delta * .5f;
	//}

	for (auto entity : view) {
		auto& transformation = view.get<Transformation>(entity);
		auto& velocity = view.get<Velocity>(entity);
		transformation.pos += velocity * delta;
	}
}