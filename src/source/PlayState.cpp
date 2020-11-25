#include "PlayState.h"
#include <iostream>
using namespace LOA;

PlayState::PlayState() {
	using namespace entt;

	scene.loadMesh("knight"_hs, "./res/models/demo_girl/chr_knight.xobj");
	scene.loadMesh("dragon_room"_hs, "./res/models/demo_room/room.xobj");

	Graphics::NormalMaterial material;
	scene.addInstance("knight"_hs, material);
	entity = scene.addInstance("dragon_room"_hs, material);

	Graphics::PerspectiveCamera camera(70, 1, .01f, 1000.0f);
	scene.setMainCamera(camera);

}

void PlayState::update(float dt) {
	static float time = 0;
	time += dt / 10;
	scene.getInstance(entity).pos = glm::vec3(0, time - 3, -10);

	scene.getInstance(entity).rot = glm::angleAxis(time, glm::vec3(0, 1, 0));

}

void PlayState::render() {
	renderer.render(scene);
}