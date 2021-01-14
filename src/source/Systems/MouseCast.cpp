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
	LOA::Window &window = Window::getInstance();
	glm::vec2 mouse = window.getMousePos();// Gets viewport space mouse coords
	LOA::Physics::PhysicsScene &pscene = engine.getPhysicsScene();
	entt::entity player = engine.getPlayer();
	entt::registry& reg = engine.getRegistry();
	Graphics::Scene &scence = engine.getScene();
	LOA::Graphics::PerspectiveCamera cam = scence.getMainCamera();
	LOA::Graphics::BasicRenderer &render = engine.getRenderer();

	float x = (2.0f*mouse.x)/render.getCurrentWidth()-1.0f;//Normalized Device space conversion
	float y = (2.0f*mouse.y)/render.getCurrentHeight()-1.0f;
	
	glm::vec4 clipcoords = glm::vec4(x, -y, -1.0f, 1.0f);//Clip coords
	glm::mat4 invertprojection = glm::inverse(render.getProjection());
	glm::vec4 eyeCoords = invertprojection * clipcoords;
	eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);//Eye coords, -1 is so it goes "into" the screen
	glm::mat4 invertview = glm::inverse(cam.transform);
	glm::vec4 rayworld = invertview * eyeCoords;
	glm::vec3 mouseray = glm::vec3(rayworld.x, rayworld.y, rayworld.z);
	mouseray = glm::normalize(mouseray);
	
	/*Get camera position and cast a ray using bullet 3 from the camera to the world coords of the mouse location.*/
	glm::vec3 campos = reg.get<Component::Transformation>(engine.getMainCamera()).pos;
	//The pair is a bool and a vec3. The bool is if it collided and the vec3 is where the closest collision was. Can use to click on terrain.
	auto pair = pscene.castRay(campos, campos + mouseray * 600.0f, false);

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