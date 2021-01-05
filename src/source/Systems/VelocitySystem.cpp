#include "Systems/VelocitySystem.h"
#include "Engine.h"
#include "Components.h"

using namespace LOA::Systems;

void VelocitySystem::update(float delta) {
	using namespace Component;

	auto& registry = engine.getRegistry();
	
}