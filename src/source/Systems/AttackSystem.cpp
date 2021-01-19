#include "Systems/MovementSystem.h"

#include "Engine.h"
#include "Components.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/intersect.hpp>
#include <assert.h>
#include "..\..\header\Systems\AttackSystem.h"
using namespace LOA;
using namespace LOA::Systems;
glm::vec3 const offset = glm::vec3(0, 40, 0);
float const FMAGE_BCD = .5;
float const FMAGE_PCD = .5;
void firebolt(Engine& engine, glm::vec3 pos, glm::vec3 forward);
void MeteorShower(Engine& engine, glm::vec3 pos);

void AttackSystem::init()
{
}

void AttackSystem::update(float delta)
{
	using namespace Component;
	auto& reg = engine.getRegistry();
	auto& FireMageView = reg.view<FireMage,MovementState,Transformation,Direction>();
	for (entt::entity ent : FireMageView) {
		auto& movementstate = FireMageView.get<MovementState>(ent);
		auto& trans = FireMageView.get<Transformation>(ent);
		auto& dir = FireMageView.get<Direction>(ent);
		auto& mage = FireMageView.get<FireMage>(ent);
		if (movementstate.basicattack && mage.cooldown >= FMAGE_BCD) {
			firebolt(engine, trans.pos, trans.rot * dir.forward);
			mage.cooldown = 0;
			mage.pcooldown += delta;
		}
		else if (movementstate.powerfulattack && mage .pcooldown >= FMAGE_PCD){
			MeteorShower(engine,trans.pos);
			mage.cooldown += delta;
		}
		else {
			mage.cooldown += delta;
			mage.pcooldown += delta;
		}
		movementstate.basicattack = false;
	}
}
void MeteorShower(Engine& engine, glm::vec3 pos) {
	using namespace Component;
	using namespace entt;
	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();
	auto& cam = engine.getMainCamera();
	auto& camtrans = registry.get<Transformation>(cam);
	auto& mousepair = engine.getMouseCast();
	for (int i = 0; i < 70; i++) {
		ID point_id = scene.addPointLight();
		entt::entity fireball = registry.create();
		glm::vec3 randvec = glm::vec3(rand() % 80-40, 0, rand() % 80-40);
		glm::vec3 mrandvec = glm::vec3(rand() % 8 - 4, 0, rand() % 8 - 4);
		registry.emplace<Transformation>(fireball, offset+randvec);
		registry.emplace<Velocity>(fireball, glm::normalize((mousepair.second+mrandvec) - (offset+randvec)) * 20.0f);
		registry.emplace<FireParticle>(fireball, 200.0f, 10.0f);
		//registry.emplace<Renderable>(fireball, id);
		//registry.emplace<RigidBody>(fireball, glm::vec3(.5), glm::vec3(0), .1f);
		registry.emplace<HitBox>(fireball, EventType::FIRE_BOLT, glm::vec3(.1), glm::vec3(0));
		registry.emplace<ProjectileComponent>(fireball);

		registry.emplace<PointLight>(fireball, point_id, glm::vec3(1, .5, .3), 2.0f, 10.0f);
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
