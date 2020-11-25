#include "PlayState.h"
#include "Systems/MovementSystem.h"
using namespace LOA;

PlayState::PlayState() {
	using namespace entt;

	scene.loadMesh("knight"_hs, "./res/models/demo_girl/chr_knight.xobj");
	scene.loadMesh("dragon_room"_hs, "./res/models/demo_room/room.xobj");
	scene.loadTEX("room"_hs, "./res/models/demo_room/room.png");
	scene.loadTEX("char"_hs, "./res/models/demo_girl/chr_knight.png");

	Graphics::BasicLitMaterial material;
	material.diffuse = "room"_hs;
	entity = scene.addInstance("dragon_room"_hs, material);

	Graphics::NormalMaterial char_material;
	//char_material.diffuse = "char"_hs;
	char_entity = scene.addInstance("knight"_hs, char_material);

	Graphics::PerspectiveCamera camera(glm::radians(70.0f), 1, .01f, 1000.0f);
	scene.setMainCamera(camera);

	Graphics::PointLight point;
	point.position = glm::vec3(0, 0, 0);
	point.color = glm::vec3(1, 1, 1);
	point.intensity = 7;

	scene.addPointLight(point);

	Graphics::PointLight point2;
	point2.position = glm::vec3(0, 5, 0);
	point2.color = glm::vec3(1, .2, 1);
	point2.intensity = 30;

	scene.addPointLight(point2);

	engine.addSystem<Systems::MovementSystem>();

}

void PlayState::update(float dt) {
	static float time = 0;
	time += dt / 5;
	scene.getInstance(entity).pos = glm::vec3(0, time - 5, -10);
	scene.getInstance(char_entity).pos = glm::vec3(2 * time, time - 5, -9.9);


	scene.getInstance(entity).rot = glm::angleAxis(time, glm::vec3(0, 1, 0));
	scene.getInstance(char_entity).rot = glm::angleAxis(time, glm::vec3(0, 1, 0));

}

void PlayState::render() {
	renderer.render(scene);
}