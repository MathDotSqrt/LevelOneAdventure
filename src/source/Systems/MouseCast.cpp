#include "systems/MouseCast.h"

#include "Engine.h"
#include "Window.h"
#include "Components.h"
#include "Physics/PhysicsScene.h"
using namespace LOA::Systems;
using namespace entt;
static entt::entity dwagon;
void MouseCast::init() {
	//printf("We got printf out here.");
	Graphics::Scene &scence = engine.getScene();
	scence.loadMesh("Dwagon"_hs,"res/models/dragon/dragon2.stl");
	Graphics::BasicLitMaterial material;
	material.color = glm::vec3(.1,.76,.4);
	LOA::ID id = scence.addInstance("Dwagon"_hs,material);
	entt::registry& reg = engine.getRegistry();
	dwagon = reg.create();
	reg.emplace<Component::Transformation>(dwagon, glm::vec3(1, 1, 1), glm::angleAxis(3.14f/2*3.14f,glm::vec3(1,0,0)), glm::vec3(.1, .1, .1));
	reg.emplace<Component::Renderable>(dwagon,id);
	//reg.emplace<Component::Velocity>(ents,glm::vec3(6,2,4));
	
}

void MouseCast::update(float delta) {
	//printf("Still priunting");
	LOA::Window &window = Window::getInstance();
	glm::vec2 mouse = window.getMousePos();
	LOA::Physics::PhysicsScene &pscene = engine.getPhysicsScene();
	entt::entity player = engine.getPlayer();
	entt::registry& reg = engine.getRegistry();
	//Component::Transformation trans = reg.get<Component::Transformation>(player);
	Graphics::Scene &scence = engine.getScene();
	LOA::Graphics::PerspectiveCamera cam = scence.getMainCamera();
	LOA::Graphics::BasicRenderer &render = engine.getRenderer();
	float x = (2.0f*mouse.x)/render.getCurrentWidth()-1.0f;
	float y = (2.0f*mouse.y)/render.getCurrentHeight()-1.0f;
	//mouse = glm::vec2(x, -1.0f*y);
	glm::vec4 clipcoords = glm::vec4(x, -y, -1.0f, 1.0f);
	glm::mat4 invertprojection = glm::inverse(render.getProjection());
	glm::vec4 eyeCoords = invertprojection * clipcoords;
	eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
	glm::mat4 invertview = glm::inverse(cam.transform);
	glm::vec4 rayworld = invertview * eyeCoords;
	glm::vec3 mouseray = glm::vec3(rayworld.x, rayworld.y, rayworld.z);
	mouseray = glm::normalize(mouseray);
	Component::Transformation &trans = reg.get<Component::Transformation>(dwagon);
	
	//printf("%f|%f|%f\n", mouseray.x, mouseray.y, mouseray.z);
	glm::vec3 campos = reg.get<Component::Transformation>(engine.getMainCamera()).pos;
	auto pair = pscene.castRay(campos, campos + mouseray * 600.0f, true);
	if (pair.first)
		trans.pos = pair.second;
	else
		trans.pos = campos + mouseray * 600.0f;
	//printf("%d",pscene.castRay(campos, campos + mouseray * 600.0f,true));
	
	
	//pscene.castRay(glm::vec3(mouse,0),trans.pos,true);
}