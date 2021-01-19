#include "systems/MouseCast.h"

#include "Engine.h"
#include "Window.h"
#include "Components.h"
#include "Physics/PhysicsScene.h"
#include "glm/gtx/vector_angle.hpp"
#include "Util/TransformUtil.h"
using namespace LOA::Systems;
using namespace entt;
//static entt::entity dwagon;
void MouseCast::init() {
	//printf("We got printf out here.");
	/*Graphics::Scene &scence = engine.getScene();
	scence.loadMesh("Dwagon"_hs,"res/models/dragon/dragon2.stl");
	Graphics::BasicLitMaterial material;
	material.color = glm::vec3(.1,.76,.4);
	LOA::ID id = scence.addInstance("Dwagon"_hs,material);
	entt::registry& reg = engine.getRegistry();
	dwagon = reg.create();
	reg.emplace<Component::Transformation>(dwagon, glm::vec3(1, 1, 1), glm::angleAxis(3.14f/2*3.14f,glm::vec3(1,0,0)), glm::vec3(.1, .1, .1));
	reg.emplace<Component::Renderable>(dwagon,id);*/
	//reg.emplace<Component::Velocity>(ents,glm::vec3(6,2,4));
	
}
/*https://www.youtube.com/watch?v=DLKN0jExRIM&t=302s Used this video to figure out how to convert the coords of the mouse to world coordinate.
Basically it does the reverse of rendering a cube but stops at world space. Goes from viewport->normalized device-> clip space->eye space -> world space.*/
void MouseCast::update(float delta) {
	
	auto& pair = engine.getMouseCast();
	auto& reg = engine.getRegistry();
	/*Found the angle between the forward vector of the player and the vector between the player and the mouse. Creates a quaternian using that angle 
	and sets the player rotation to that new rotation. Use vec2 since we dont care about the y-axis and it repsects the sign of the angle. */
	auto &playertrans = reg.get<Component::Transformation>(engine.getPlayer());
	auto &playerdir = reg.get<Component::Direction>(engine.getPlayer());
	glm::vec2 playervec = glm::vec2(pair.second.x - playertrans.pos.x, pair.second.z - playertrans.pos.z);
	glm::vec2 forvec = glm::vec2(playerdir.forward.x , playerdir.forward.z );
	/*float angle = glm::orientedAngle(glm::normalize(forvec), glm::normalize(playervec));
	glm::quat newrot = glm::angleAxis(-angle, glm::vec3(0, 1, 0));*/
	playertrans.rot = LOA::Util::turn_towards(forvec,playervec);
}
