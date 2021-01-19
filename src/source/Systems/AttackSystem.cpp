#include "Systems/MovementSystem.h"

#include "Engine.h"
#include "Components.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/intersect.hpp>
#include <assert.h>
#include "..\..\header\Systems\AttackSystem.h"
using namespace LOA;
using namespace LOA::Systems;

void firebolt(Engine& engine, glm::vec3 pos, glm::vec3 forward);

void AttackSystem::init()
{
}

void AttackSystem::update(float delta)
{
	using namespace Component;
	auto& reg = engine.getRegistry();
	auto& FireMageView = reg.view<FireMage,MovementState>();
	for (entt::entity ent : FireMageView) {
		auto& movementstate = FireMageView.get<MovementState>(ent);
		auto& trans = FireMageView.get<Transformation>(ent);
		auto& dir = FireMageView.get<Direction>(ent);
		if (movementstate.basicattack)
			firebolt(engine,trans.pos,dir.forward);
	}
}
void firebolt(Engine& engine, glm::vec3 pos, glm::vec3 forward) {
	using namespace Component;
	using namespace entt;
	
	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	//ID id = scene.addInstance("cube"_hs, Graphics::NormalMaterial{});
	ID point_id = scene.addPointLight();

	entt::entity fireball = registry.create();

	registry.emplace<Transformation>(fireball, pos + glm::normalize(forward));
	registry.emplace<Velocity>(fireball, glm::normalize(forward) * 20.0f);
	registry.emplace<FireParticle>(fireball, 200.0f, 10.0f);
	//registry.emplace<Renderable>(fireball, id);
	//registry.emplace<RigidBody>(fireball, glm::vec3(.5), glm::vec3(0), .1f);
	registry.emplace<HitBox>(fireball, EventType::FIRE_BOLT, glm::vec3(.1), glm::vec3(0));
	registry.emplace<ProjectileComponent>(fireball);

	registry.emplace<PointLight>(fireball, point_id, glm::vec3(1, .5, .3), 2.0f, 10.0f);


}
