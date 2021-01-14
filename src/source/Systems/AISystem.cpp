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
float const whiskeroffest = 3.14 / 6;
float const interpolatespeed = .1;
void AISystem::init()
{
	Graphics::Scene &scence = engine.getScene();
	scence.loadMesh("Dwagon"_hs,"res/models/dragon/dragon2.stl");
	Graphics::BasicLitMaterial material;
	material.color = glm::vec3(.1,.76,.4);
	
	entt::registry& reg = engine.getRegistry();
	for (int i = 1; i < 20; i++) {//hahaha
		LOA::ID id = scence.addInstance("cube"_hs, material);
		entt::entity dwagon = reg.create();
		reg.emplace<Component::Transformation>(dwagon); //,glm::vec3(1.0f / i, 1, 1 * i), glm::angleAxis(3.14f / 2 * 3.14f, glm::vec3(1, 0, 0)), glm::vec3(.1, .1, .1));
		reg.emplace<Component::Renderable>(dwagon, id);
		reg.emplace<Component::Velocity>(dwagon, glm::vec3(0, 0, 0));
		reg.emplace<Component::Direction>(dwagon, glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
		reg.emplace<Component::AIComponent>(dwagon, engine.getPlayer(),10.0f);
		reg.emplace<Component::CharacterController>(dwagon); 
		reg.emplace<Component::HealthComponent>(dwagon, 10.0f, 10.0f);
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
		//printf("FIRE\n");
		reg.get<Component::MovementState>(ent).fire = true;
		reg.get<Component::AIComponent>(ent).cooldown = 0.0f;
	}
}
void AISystem::update(float delta)
{
	auto& reg = engine.getRegistry();
	auto& AIView = reg.view<Component::AIComponent, Component::Transformation, Component::MovementState, Component::Velocity, Component::Direction>();
	for (entt::entity ent : AIView) {
		auto& trans = AIView.get<Component::Transformation>(ent);
		auto& targ = AIView.get<Component::AIComponent>(ent).target;
		auto& targtrans = reg.get<Component::Transformation>(targ);
		auto& dir = AIView.get<Component::Direction>(ent);
		glm::vec3 path = targtrans.pos - trans.pos;
		auto& aicomp = reg.get<Component::AIComponent>(ent);
		auto& pscene = engine.getPhysicsScene();
		glm::vec3& entpos = reg.get<Component::Transformation>(ent).pos;
		glm::vec3& targpos = reg.get<Component::Transformation>(targ).pos;
		auto pair = pscene.castRay(entpos, targpos);//LOS Cast
		if (!pair.first) {//nothing in the way
			glm::vec3 dyndir = trans.rot * dir.forward;
			//trans.rot = LOA::Util::turn_towards(glm::vec2(dyndir.x, dyndir.z), glm::vec2(targtrans.pos.x - trans.pos.x, targtrans.pos.z - trans.pos.z)) * trans.rot;
			glm::quat target = LOA::Util::turn_towards(glm::vec2(dyndir.x, dyndir.z), glm::vec2(targtrans.pos.x - trans.pos.x, targtrans.pos.z - trans.pos.z)) * trans.rot;
			glm::quat current = trans.rot;

			trans.rot = glm::slerp(current, target, interpolatespeed);
			attack(ent, engine, delta);
		}
		else {
			AIView.get<Component::MovementState>(ent).forward = -.4f;
			chase(trans,dir,targtrans,delta);
		}

		
		
	}
		//chase(delta);
}
using namespace LOA::Component;
void AISystem::chase(Transformation &trans, Direction dir,Transformation& targtrans,float delta) {
	auto& reg = engine.getRegistry();
	auto& pscene = engine.getPhysicsScene();
	glm::quat angleoffset = glm::angleAxis(whiskeroffest, glm::vec3(0, 1, 0));
	glm::quat angleoffset2 = glm::angleAxis(-whiskeroffest, glm::vec3(0, 1, 0));
	auto left = pscene.castRay(trans.pos, trans.pos + angleoffset * (trans.rot * dir.forward) * 5.0f, true);
	auto right = pscene.castRay(trans.pos, trans.pos + angleoffset2 * (trans.rot * dir.forward) * 5.0f, true);
	auto mid = pscene.castRay(trans.pos, trans.pos + trans.rot * dir.forward * 8.0f, true);
	float theta = 0;
	//printf("Left: %f,%f,%f\n", left.second.x, left.second.y, left.second.z);
	//printf("Right: %f,%f,%f\n", right.second.x, right.second.y, right.second.z);
	//printf("Mid: %f,%f,%f\n", mid.second.x, mid.second.y, mid.second.z);

	if (!left.first && !right.first && !mid.first) {
		glm::vec3 dyndir = trans.rot * dir.forward;
		glm::quat target = LOA::Util::turn_towards(glm::vec2(dyndir.x, dyndir.z), glm::vec2(targtrans.pos.x - trans.pos.x, targtrans.pos.z - trans.pos.z)) * trans.rot;
		glm::quat current = trans.rot;
		
		trans.rot = glm::slerp(current, target, interpolatespeed);
	}
	if(left.first){
		theta += -.001;
	}
	if(right.first){
		theta += .001;
	}
	if(mid.first){
	}
	glm::quat offset = glm::angleAxis(theta, glm::vec3(0, 1, 0));
	trans.rot = offset * trans.rot;
}
//out of range
		/*if(glm::length(path) > AIView.get<Component::AIComponent>(ent).attackrange)
			if(!pair.first)
				AIView.get<Component::MovementState>(ent).forward = -.4f;
			else
				AIView.get<Component::MovementState>(ent).strafe = 3.0f;
		else {//in range
			AIView.get<Component::MovementState>(ent).forward = 0.0f;
			if(pair.first)
				AIView.get<Component::MovementState>(ent).forward = -1.0f;
			else
				attack(ent,engine,delta);
		}*/
