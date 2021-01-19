#include "Systems/AISystem.h"

#include "Engine.h"
#include "Window.h"
#include "Components.h"
#include "Physics/PhysicsScene.h"
#include "glm/gtx/vector_angle.hpp"
#include "Util/TransformUtil.h"
using namespace LOA;
using namespace LOA::Systems;
using namespace entt;

//static entt::entity dwagon;
//Need to update slerp and avoidance rates if speed is updated
float const ENEMY_FORWARD_SPEED = .6;

float const MID_LENGTH = 3;
float const SIDE_LENGTH = 2;
float const WHISKER_OFFSET = 3.14 / 4;
float const SHOULDER_LENGTH = 1;


float const ATTACK_SLERP_SPEED = .1 / .6;
float const SLERP_SPEED = .08 / .6;

float const AVOID_RATE = .03f / .6;
float const FAST_AVOID_MULTIPLIER = 2;

static entt::entity debug_cube;

void AISystem::init()
{
	Graphics::Scene &scence = engine.getScene();
	scence.loadMesh("Dwagon"_hs,"res/models/dragon/dragon2.stl");
	Graphics::BasicLitMaterial material;
	material.color = glm::vec3(.1,.76,.4);
	
	entt::registry& reg = engine.getRegistry();
	for (int i = 1; i < 2; i++) {//hahaha
		LOA::ID id = scence.addInstance("cube"_hs, material);
		entt::entity dwagon = reg.create();
		reg.emplace<Component::Transformation>(dwagon); //,glm::vec3(1.0f / i, 1, 1 * i), glm::angleAxis(3.14f / 2 * 3.14f, glm::vec3(1, 0, 0)), glm::vec3(.1, .1, .1));
		reg.emplace<Component::Renderable>(dwagon, id);
		reg.emplace<Component::Velocity>(dwagon, glm::vec3(0, 0, 0));
		reg.emplace<Component::Direction>(dwagon, glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
		reg.emplace<Component::AIComponent>(dwagon, engine.getPlayer(), 10.0f, ENEMY_FORWARD_SPEED);
		reg.emplace<Component::CharacterController>(dwagon); 
		reg.emplace<Component::HealthComponent>(dwagon, 10.0f, 10.0f);
		reg.emplace<Component::HitBox>(dwagon,Component::EventType::CHARACTER,glm::vec3(1,1,1));
		reg.emplace<Component::MovementState>(dwagon);
	}


	Graphics::BasicMaterial debug_material;
	debug_material.setAlpha(.5f);
	debug_material.setColor(glm::vec3(1.5f));
	LOA::ID id = scence.addInstance("cube"_hs, debug_material);
	debug_cube = reg.create();
	reg.emplace<Component::Transformation>(debug_cube, glm::vec3(0), glm::quat(1, 0, 0, 0), glm::vec3(.2f)); //,glm::vec3(1.0f / i, 1, 1 * i), glm::angleAxis(3.14f / 2 * 3.14f, glm::vec3(1, 0, 0)), glm::vec3(.1, .1, .1));
	reg.emplace<Component::Renderable>(debug_cube, id);
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

glm::quat turn_ai(const glm::quat &current_rot, const glm::vec3 &target_dir, const Component::Direction &dir, float SPEED) {
	glm::vec3 dyndir = current_rot * dir.forward;
	glm::vec2 current_dir2(dyndir.x, dyndir.z);
	glm::vec2 target_dir2(target_dir.x, target_dir.z);

	glm::quat target = LOA::Util::turn_towards(current_dir2, target_dir2) * current_rot;
	return glm::slerp(current_rot, target, glm::min(SPEED, 1.f));
}

void AISystem::update(float delta)
{
	using namespace Component;

	auto& pscene = engine.getPhysicsScene();
	auto& reg = engine.getRegistry();
	

	//How AI State changes for enemy AI
	auto EnemyAIView = reg.view<AIComponent, Transformation>(entt::exclude<PartyMember>);
	for (entt::entity ent : EnemyAIView) {
		
		auto& trans = EnemyAIView.get<Component::Transformation>(ent);
		auto& aicomp = EnemyAIView.get<Component::AIComponent>(ent);
		auto& targ = aicomp.target;

		if (targ == entt::null) {
			aicomp.currentstate = AIState::IDLE;
			continue;
		}

		auto& targtrans = reg.get<Component::Transformation>(targ);
		glm::vec3 path = targtrans.pos - trans.pos;

		auto pair = pscene.castRay(trans.pos, targtrans.pos, true);//LOS Cast

		if (pair.first == false) { //Has line of sight
			aicomp.currentstate = AIState::ATTACK;
			aicomp.lastspot = targtrans.pos;

		}
		else if (aicomp.currentstate == AIState::ATTACK) {	//Was attacking but lost LOS; Chase
			aicomp.currentstate = AIState::CHASE;
		}
		else if (aicomp.currentstate == AIState::CHASE) {
			//If AI was chasing and arrived at target position but did not gain line of sight of target search
			if (glm::distance(aicomp.lastspot, trans.pos) < 1.f) {
				aicomp.currentstate = AIState::SEARCH;
			}
		}
	}


	auto PartyMemberAIView = reg.view<AIComponent, Transformation, PartyMember>();
	for (entt::entity ent : PartyMemberAIView) {
		auto& trans = PartyMemberAIView.get<Component::Transformation>(ent);
		auto& aicomp = PartyMemberAIView.get<Component::AIComponent>(ent);
		auto& targ = aicomp.target;

		//No valid target
		if (targ == entt::null) {
			aicomp.currentstate = AIState::IDLE;
			continue;
		}

		auto& targtrans = reg.get<Component::Transformation>(targ);
		glm::vec3 path = targtrans.pos - trans.pos;

		auto pair = pscene.castRay(trans.pos, targtrans.pos, true);//LOS Cast
		if(pair.first == false) { //Has line of sight
			aicomp.lastspot = targtrans.pos;
		}

		if (glm::distance(trans.pos, targtrans.pos) < 2) {
			aicomp.currentstate = AIState::IDLE;
		}
		else{
			aicomp.currentstate = AIState::CHASE;
		}

	}

	//All AI actions per state
	auto& AIView = reg.view<Component::AIComponent, Component::Transformation, Component::MovementState, Component::Velocity, Component::Direction>();
	for (entt::entity ent : AIView) {
		auto& trans = AIView.get<Component::Transformation>(ent);
		auto& dir = AIView.get<Component::Direction>(ent);
		auto& movement = AIView.get<Component::MovementState>(ent);
		auto& aicomp = AIView.get<Component::AIComponent>(ent);
		auto& targ = aicomp.target;
		
		//Something wrong here...
		//There is no entity to target
		//Automatically idle
		if (targ == entt::null) {
			aicomp.currentstate = AIState::IDLE;
		}
		
		switch (aicomp.currentstate) {
		case AIState::IDLE:
			movement.forward = 0;
			break;
		case AIState::ATTACK:
		{
			auto& targtrans = reg.get<Component::Transformation>(targ);
			glm::vec3 path = targtrans.pos - trans.pos;
			trans.rot = turn_ai(trans.rot, path, dir, ATTACK_SLERP_SPEED * aicomp.speed);

			if (glm::length(path) < aicomp.attackrange) {
				attack(ent, engine, delta);

				//if too close move back a lil
				if (glm::length(path) < aicomp.attackrange / 2.0f) {
					movement.forward = aicomp.speed;
				}
			}
			else {
				movement.forward = -aicomp.speed;
			}
		}
			break;
		case AIState::CHASE:
			//Moves forward and performs chase 
			movement.forward = -aicomp.speed;
			chase(trans, dir, aicomp.lastspot, aicomp.speed, delta);
			break;
		case AIState::SEARCH:
			//Blindly moves forward
			movement.forward = -aicomp.speed;
			break;
		}

		//debug info
		reg.get<Component::Transformation>(debug_cube).pos = aicomp.lastspot;
		reg.get<Component::Transformation>(debug_cube).pos.y += 1.f;
	}
		//chase(delta);
}
using namespace LOA::Component;
void AISystem::chase(Transformation &trans, Direction dir, glm::vec3 &targtrans, float speed, float delta) {
	auto& reg = engine.getRegistry();
	auto& pscene = engine.getPhysicsScene();
	glm::quat angleoffset = glm::angleAxis(WHISKER_OFFSET, glm::vec3(0, 1, 0));
	glm::quat angleoffset2 = glm::angleAxis(-WHISKER_OFFSET, glm::vec3(0, 1, 0));
	/*glm::quat rs = glm::angleAxis(-3.14f / 2.0f, glm::vec3(0, 1, 0));
	glm::quat ls = glm::angleAxis(3.14f / 2.0f, glm::vec3(0, 1, 0));
	auto lside = pscene.castRay(trans.pos, trans.pos + ls * (trans.rot * dir.forward) * SHOULDER_LENGTH, true);
	auto rside = pscene.castRay(trans.pos, trans.pos + rs * (trans.rot * dir.forward) * SHOULDER_LENGTH, true);*/
	auto left = pscene.castRay(trans.pos, trans.pos + angleoffset * (trans.rot * dir.forward) * SIDE_LENGTH, true);
	auto right = pscene.castRay(trans.pos, trans.pos + angleoffset2 * (trans.rot * dir.forward) * SIDE_LENGTH, true);
	auto mid = pscene.castRay(trans.pos, trans.pos + trans.rot * dir.forward * MID_LENGTH, true);
	float theta = 0;
	//printf("Left: %f,%f,%f\n", left.second.x, left.second.y, left.second.z);
	//printf("Right: %f,%f,%f\n", right.second.x, right.second.y, right.second.z);
	//printf("Mid: %f,%f,%f\n", mid.second.x, mid.second.y, mid.second.z);

	//TODO: if all three are true the AI is LOST
	if (left.first && right.first && mid.first) {
		//Go Home, pray that the AI wont get lost on the way home
		glm::vec3 path = targtrans - trans.pos;
		trans.rot = turn_ai(trans.rot, path, dir, 10 * SLERP_SPEED * speed);
	}
	
	if (!left.first && !right.first && !mid.first) {
		glm::vec3 path = targtrans - trans.pos;
		trans.rot = turn_ai(trans.rot, path, dir, SLERP_SPEED * speed);
	}
	if(left.first){
		theta += -AVOID_RATE * speed;
	}
	if(right.first){
		theta += AVOID_RATE * speed;
	}
	if(mid.first){
		theta *= FAST_AVOID_MULTIPLIER;
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
