#include "Systems/InputSystem.h"

#include "Engine.h"
#include "Window.h"
#include "Components.h"

using namespace LOA::Systems;

void InputSystem::init(Engine &engine) {
	using namespace Component;

	auto pos = Window::getInstance().getMousePos();

	auto& registry = engine.getRegistry();
	auto view = registry.view<Input>();
	view.each([pos](auto& input) {
		input.lastCursorPos = pos;
	});
}

void InputSystem::update(Engine& engine, float delta) {
	using namespace Component;
	auto& window = Window::getInstance();
	
	glm::vec2 pos = window.getMousePos();
	
	auto& registry = engine.getRegistry();
	auto view = registry.view<Input, MovementState>();
	for (auto entity : view) {
		auto& input = view.get<Input>(entity);
		auto& movement = view.get<MovementState>(entity);

		movement.forward = 0;
		movement.strafe = 0;
		movement.fly = 0;

		if (window.isPressed('w'))
			movement.forward -= 1;
		if (window.isPressed('s'))
			movement.forward += 1;

		if (window.isPressed('d'))
			movement.strafe += 1;
		if (window.isPressed('a'))
			movement.strafe -= 1;

		if (window.isPressed(' '))
			movement.fly += 1;
		if (window.isPressed(Window::Keys::LEFT_SHIFT))
			movement.fly -= 1;

		if (window.isPressed('p')) {
			movement.fire = !input.lastFire;
		}
		input.lastFire = window.isPressed('p');

		glm::vec2 delta = pos - input.lastCursorPos;

		movement.rotate = -delta;

		input.lastCursorPos = pos;
	}

	auto camera_view = registry.view<Input, Camera>();
	for (auto entity : camera_view) {
		auto& input = camera_view.get<Input>(entity);
		auto& camera = camera_view.get<Camera>(entity);
		
		camera.distance -= window.getScrollDelta() / 1.0f;
		camera.phi -= window.getScrollDelta() / 10;
		camera.theta -= window.isClick(Window::Mouse::RIGHT_CLICK) ? (pos.x - input.lastCursorPos.x) / 200 : 0;
		input.lastCursorPos = pos;
	}
}