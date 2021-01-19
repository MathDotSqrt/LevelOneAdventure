#include "Systems/PartySystem.h"

#include "Engine.h"
#include "Window.h"
#include "Components.h"
#include "entt/core/hashed_string.hpp"
#include "Graphics/Scene.h"
#include "Physics/PhysicsScene.h"

using namespace LOA::Systems;
using namespace LOA::Component;
using namespace entt;

void PartySystem::init() {

	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	Graphics::DissolveMaterial material;
	material.color = glm::vec3(1, 1, 1);
	material.texture = "uv_debug_grid"_hs;
	material.offset = .01f;
	material.dissolve_color = glm::vec3(1, .8, .4);

	for (int i = 0; i < 4; i++) {
		ID cubeID = scene.addInstance("cube"_hs, material);
		ID point_light = scene.addPointLight(Graphics::PointLight{});
		entt::entity npc = registry.create();
		registry.emplace<Transformation>(npc, glm::vec3(-10 + 10 * i, 0, 12));
		registry.emplace<Velocity>(npc, glm::vec3(0, 0, 0));
		registry.emplace<Direction>(npc, glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		registry.emplace<MovementState>(npc);
		registry.emplace<Renderable>(npc, cubeID);
		registry.emplace<PointLight>(npc, point_light, glm::vec3(.7, .6, .5), 1.f, 10.0f);
		registry.emplace<CharacterController>(npc);
		registry.emplace<HitBox>(npc, EventType::CHARACTER, glm::vec3(.5));
		registry.emplace<HealthComponent>(npc, 10.f, 10.f);
		registry.emplace<AIComponent>(npc, entt::null, .8f);
		registry.emplace<PartyMember>(npc, entt::null);
		registry.emplace<Graphics::DissolveMaterial>(npc, material);
	}

}

void PartySystem::update(float delta) {
	using namespace Component;

	auto& registry = engine.getRegistry();

	auto MemberView = registry.view<PartyMember, AIComponent, Transformation>();
	for (entt::entity entity : MemberView) {
		auto& member = MemberView.get<PartyMember>(entity);
		auto& ai = MemberView.get<AIComponent>(entity);

		if (member.leader == entt::null) {
			auto& transform = MemberView.get<Transformation>(entity);

			auto& player_trans = registry.get<Transformation>(engine.getPlayer());
			if (glm::distance(player_trans.pos, transform.pos) < 10 && engine.getPhysicsScene().castRay(transform.pos, player_trans.pos).first == false) {
				member.leader = engine.getPlayer();
			}
		}

		ai.target = member.leader;
	}
}