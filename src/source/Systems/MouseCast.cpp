#include "systems/MouseCast.h"

#include "Engine.h"
#include "Window.h"
#include "Components.h"
#include "Physics/PhysicsScene.h"
using namespace LOA::Systems;
using namespace entt;
void MouseCast::init() {
	//printf("We got printf out here.");
	Graphics::Scene &scence = engine.getScene();
	scence.loadMesh("Dwagon"_hs,"res/models/dragon/dragon2.stl");
	Graphics::BasicLitMaterial material;
	material.color = glm::vec3(.1,.76,.4);
	LOA::ID id = scence.addInstance("Dwagon"_hs,material);
	entt::registry& reg = engine.getRegistry();
	entt::entity ents = reg.create();
	reg.emplace<Component::Transformation>(ents, glm::vec3(1, 1, 1), glm::angleAxis(3.14f/2*3.14f,glm::vec3(1,0,0)), glm::vec3(.1, .1, .1));
	reg.emplace<Component::Renderable>(ents,id);
	reg.emplace<Component::Velocity>(ents,glm::vec3(6,2,4));
	
}

void MouseCast::update(float delta) {
	//printf("Still priunting");
	LOA::Window &window = Window::getInstance();
	glm::vec2 mouse = window.getMousePos();
	LOA::Physics::PhysicsScene &pscene = engine.getPhysicsScene();
	entt::entity player = engine.getPlayer();
	entt::registry& reg = engine.getRegistry();
	Component::Transformation trans = reg.get<Component::Transformation>(player);
	pscene.castRay(glm::vec3(mouse,0),trans.pos,true);
}