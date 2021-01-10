#include "Systems/AISystem.h"

#include "Engine.h"
#include "Window.h"
#include "Components.h"
#include "Physics/PhysicsScene.h"
#include "glm/gtx/vector_angle.hpp"
using namespace LOA::Systems;
using namespace entt;
static entt::entity dwagon;
void AISystem::init()
{
	Graphics::Scene &scence = engine.getScene();
	scence.loadMesh("Dwagon"_hs,"res/models/dragon/dragon2.stl");
	Graphics::BasicLitMaterial material;
	material.color = glm::vec3(.1,.76,.4);
	LOA::ID id = scence.addInstance("Dwagon"_hs,material);
	entt::registry& reg = engine.getRegistry();
	dwagon = reg.create();
	reg.emplace<Component::Transformation>(dwagon, glm::vec3(1, 1, 1), glm::angleAxis(3.14f/2*3.14f,glm::vec3(1,0,0)), glm::vec3(.1, .1, .1));
	reg.emplace<Component::Renderable>(dwagon,id);
	reg.emplace<Component::Velocity>(dwagon,glm::vec3(0,0,0));
	reg.emplace<Component::AIComponent>(dwagon,engine.getPlayer());
}

void AISystem::update(float delta)
{
	auto &reg = engine.getRegistry();
	auto &AIView = reg.view<Component::AIComponent>();
	for (entt::entity ent : AIView) {
		auto trans = reg.get<Component::Transformation>(ent);
		auto targ = reg.get<Component::AIComponent>(ent).target;
		auto targtrans = reg.get<Component::Transformation>(targ);
		glm::vec3 path = targtrans.pos - trans.pos;
		reg.get<Component::Velocity>(ent) = path;
	}
}
