#include "Systems/HealthSystem.h"
#include "Engine.h"
#include "Components.h"
#include <assert.h>

using namespace LOA;
using namespace LOA::Systems;

void damageEntity(Component::HealthComponent& health, float damage) {
	health.current_health -= damage;
	printf("DAMAGE %f \n", health.current_health);
}

void HealthSystem::init() {

}

void HealthSystem::update(float delta) {
	using namespace Component;

	auto& registry = engine.getRegistry();

	//All entities that have hitboxes test if they need to take damage here
	auto view = registry.view<HealthComponent, HitBox>();
	for (entt::entity entity : view) {
		auto& health = view.get<HealthComponent>(entity);
		auto& hitbox = view.get<HitBox>(entity);

		for (const auto& event : hitbox.events) {
			if (event.other_event_type == EventType::FIRE_BOLT) {
				assert(registry.has(event.other_entity));
				auto& projectile = registry.get<ProjectileComponent>(event.other_entity);
				damageEntity(health, projectile.damage);
			}
		}
	}

	auto health_view = registry.view<HealthComponent>();
	for (entt::entity entity : health_view) {
		auto& health = health_view.get<HealthComponent>(entity);
		
		health.current_health = glm::min(health.current_health, health.max_health);
		
		if (health.current_health <= 0) {
			engine.deleteEntity(entity);
		}
	}
}