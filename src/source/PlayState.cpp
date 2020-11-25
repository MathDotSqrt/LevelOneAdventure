#include "PlayState.h"
using namespace LOA;

PlayState::PlayState() {
	using namespace entt;

	scene.loadMesh("knight"_hs, "./res/models/demo_girl/chr_knight.xobj");
	scene.loadMesh("dragon_room"_hs, "./res/models/demo_room/room.xobj");
	scene.loadTEX("UV"_hs, "./res/models/demo_room/room.png");

	Graphics::BasicLitMaterial material;
	material.diffuse = "UV"_hs;

	scene.addInstance("knight"_hs, material);
	entity = scene.addInstance("dragon_room"_hs, material);

	Graphics::PerspectiveCamera camera(70, 1, .01f, 1000.0f);
	scene.setMainCamera(camera);

	Graphics::PointLight point;
	point.position = glm::vec3(0, 0, 0);
	point.color = glm::vec3(1, 1, 1);
	point.intensity = 20;

	scene.addPointLight(point);

	Graphics::PointLight point2;
	point2.position = glm::vec3(0, 5, 0);
	point2.color = glm::vec3(1, 0, 1);
	point2.intensity = 20;

	scene.addPointLight(point2);

}

void PlayState::update(float dt) {
	static float time = 0;
	time += dt / 5;
	scene.getInstance(entity).pos = glm::vec3(0, time - 5, -10);

	scene.getInstance(entity).rot = glm::angleAxis(time, glm::vec3(0, 1, 0));

}

void PlayState::render() {
	renderer.render(scene);
}