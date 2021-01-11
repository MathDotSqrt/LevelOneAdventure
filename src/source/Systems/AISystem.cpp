#include "Systems/AISystem.h"

#include "Engine.h"
#include "Window.h"
#include "Components.h"
#include "Physics/PhysicsScene.h"
#include "glm/gtx/vector_angle.hpp"
#include "Util/TransformUtil.h"
using namespace LOA::Systems;
using namespace entt;
//static entt::entity dwagon;
void AISystem::init()
{
	Graphics::Scene &scence = engine.getScene();
	scence.loadMesh("Dwagon"_hs,"res/models/dragon/dragon2.stl");
	Graphics::BasicLitMaterial material;
	material.color = glm::vec3(.1,.76,.4);
	
	entt::registry& reg = engine.getRegistry();
	for (int i = 1; i < 2; i++) {//hahaha
		LOA::ID id = scence.addInstance("Dwagon"_hs, material);
		entt::entity dwagon = reg.create();
		reg.emplace<Component::Transformation>(dwagon, glm::vec3(1.0f / i, 1, 1 * i), glm::angleAxis(3.14f / 2 * 3.14f, glm::vec3(1, 0, 0)), glm::vec3(.1, .1, .1));
		reg.emplace<Component::Renderable>(dwagon, id);
		reg.emplace<Component::Velocity>(dwagon, glm::vec3(0, 0, 0));
		reg.emplace<Component::Direction>(dwagon, glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
		reg.emplace<Component::AIComponent>(dwagon, engine.getPlayer(),10.0f);
		reg.emplace<Component::CharacterController>(dwagon); 
		reg.emplace<Component::HitBox>(dwagon,Component::EventType::CHARACTER,glm::vec3(1,1,1));
		reg.emplace<Component::MovementState>(dwagon);
	}

}
void attack(entt::entity ent, LOA::Engine &engine,float delta) {
	using namespace LOA;
	auto& reg = engine.getRegistry();
	auto& aicomp = reg.get<Component::AIComponent>(ent);
	//auto& pscene = engine.getPhysicsScene();
	//glm::vec3& entpos = reg.get<Component::Transformation>(ent).pos;
	//glm::vec3& targpos = reg.get<Component::Transformation>(aicomp.target).pos;
	
	aicomp.cooldown += delta;
	
	
	if (aicomp.cooldown >= 0.3f) {
		printf("FIRE\n");
		reg.get<Component::MovementState>(ent).fire = true;
		reg.get<Component::AIComponent>(ent).cooldown = 0.0f;
	}
}
void AISystem::update(float delta)
{
		chase(delta);
}
void AISystem::chase(float delta) {
	auto& reg = engine.getRegistry();
	auto& AIView = reg.view<Component::AIComponent, Component::Transformation, Component::Velocity, Component::Direction>();
	for (entt::entity ent : AIView) {
		auto& trans = AIView.get<Component::Transformation>(ent);
		auto& targ = AIView.get<Component::AIComponent>(ent).target;
		auto& targtrans = reg.get<Component::Transformation>(targ);
		glm::vec3 path = targtrans.pos - trans.pos;
		auto& aicomp = reg.get<Component::AIComponent>(ent);
		auto& pscene = engine.getPhysicsScene();
		glm::vec3& entpos = reg.get<Component::Transformation>(ent).pos;
		glm::vec3& targpos = reg.get<Component::Transformation>(targ).pos;
		auto pair = pscene.castRay(entpos, targpos, true);
		if(glm::length(path) > AIView.get<Component::AIComponent>(ent).attackrange)
			AIView.get<Component::Velocity>(ent) = path;
		else {
			AIView.get<Component::Velocity>(ent) = glm::vec3(0, 0, 0);
			if(pair.first)
				AIView.get<Component::Velocity>(ent) = 3.0f*path;
			else
				attack(ent,engine,delta);
		}
		auto& dir = AIView.get<Component::Direction>(ent);
		glm::vec3 dyndir = trans.rot * dir.forward;
		trans.rot = LOA::Util::turn_towards(glm::vec2(dyndir.x, dyndir.z), glm::vec2(targtrans.pos.x - trans.pos.x, targtrans.pos.z - trans.pos.z)) * trans.rot;
	}	
}

