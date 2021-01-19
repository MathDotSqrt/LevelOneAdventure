#include "Engine.h"
#include "Physics/PhysicsScene.h"
#include "Physics/PhysicsDebugDrawer.h"
#include "Window.h"

using namespace LOA;

Engine::Engine() {
	registry.set<Physics::PhysicsScene>();

	player = registry.create();
	mainCamera = registry.create();
}

void Engine::update(float delta) {

	for (auto& system : systems) {
		system->update(delta);
	}
	if (Window::getInstance().isPressed('m'))
		getPhysicsScene().getDrawer()->setDebugMode(1);
	else if (Window::getInstance().isPressed('n'))
		getPhysicsScene().getDrawer()->setDebugMode(0);
	computeMouseCast();
}

void Engine::render() {
	Physics::PhysicsScene& physicsScene = getPhysicsScene();

	//we pass the physics scene to the renderer if we want to debug draw
	Physics::PhysicsScene* physics = physicsScene.getDrawer()->getDebugMode() == 0 ? nullptr : &physicsScene;
	renderer.render(scene, physics);

	//We prerender for the next frame because game physics is one frame behind the renderer
	physicsScene.prerender();
}

void Engine::initSystems() {
	for (auto& system : systems) {
		system->init();
	}
}

void Engine::deleteEntity(entt::entity entity) {
	to_delete.insert(entity);
}

void Engine::batchDelete() {
	getRegistry().destroy(to_delete.begin(), to_delete.end());
	to_delete.clear();
}

entt::registry& Engine::getRegistry() {
	return registry;
}

Graphics::Scene& Engine::getScene() {
	return scene;
}

Graphics::BasicRenderer& Engine::getRenderer() {
	return renderer;
}

Physics::PhysicsScene& Engine::getPhysicsScene() {
	return 	registry.ctx<Physics::PhysicsScene>();
}

const entt::entity& Engine::getPlayer() const {
	return player;
}

const entt::entity& Engine::getMainCamera() const {
	return mainCamera;
}
std::pair<bool, glm::vec3> Engine::getMouseCast() {
	return mousecastposition;
}
void Engine::computeMouseCast() {
	LOA::Window& window = Window::getInstance();
	glm::vec2 mouse = window.getMousePos();// Gets viewport space mouse coords
	LOA::Physics::PhysicsScene& pscene = this->getPhysicsScene();
	entt::entity player = this->getPlayer();
	entt::registry& reg = this->getRegistry();
	Graphics::Scene& scence = this->getScene();
	LOA::Graphics::PerspectiveCamera cam = scence.getMainCamera();
	LOA::Graphics::BasicRenderer& render = this->getRenderer();

	float x = (2.0f * mouse.x) / render.getCurrentWidth() - 1.0f;//Normalized Device space conversion
	float y = (2.0f * mouse.y) / render.getCurrentHeight() - 1.0f;

	glm::vec4 clipcoords = glm::vec4(x, -y, -1.0f, 1.0f);//Clip coords
	glm::mat4 invertprojection = glm::inverse(render.getProjection());
	glm::vec4 eyeCoords = invertprojection * clipcoords;
	eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);//Eye coords, -1 is so it goes "into" the screen
	glm::mat4 invertview = glm::inverse(cam.transform);
	glm::vec4 rayworld = invertview * eyeCoords;
	glm::vec3 mouseray = glm::vec3(rayworld.x, rayworld.y, rayworld.z);
	mouseray = glm::normalize(mouseray);

	/*Get camera position and cast a ray using bullet 3 from the camera to the world coords of the mouse location.*/
	glm::vec3 campos = reg.get<Component::Transformation>(this->getMainCamera()).pos;
	//The pair is a bool and a vec3. The bool is if it collided and the vec3 is where the closest collision was. Can use to click on terrain.
	mousecastposition = pscene.castRay(campos, campos + mouseray * 600.0f, false);

}
