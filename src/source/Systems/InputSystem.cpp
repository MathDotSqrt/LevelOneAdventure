#include "Systems/InputSystem.h"

#include "Engine.h"
#include "Window.h"
#include "Components.h"

using namespace LOA::Systems;

void InputSystem::init() {
	using namespace Component;

	auto pos = Window::getInstance().getMousePos();

	auto& registry = engine.getRegistry();
	auto view = registry.view<Input>();
	view.each([pos](auto& input) {
		input.lastCursorPos = pos;
	});
}

void InputSystem::update(float delta) {
	using namespace Component;
	auto& window = Window::getInstance();
	auto& registry = engine.getRegistry();
	glm::vec2 pos = window.getMousePos();
	
	if (window.isPressed(Window::Mouse::RIGHT_CLICK)) {
		auto input_view = registry.view<Input>();
		input_view.each([pos](auto& input) {
			input.lastCursorPos = pos;
		});
	}

	auto view = registry.view<Input, MovementState>();
	for (auto entity : view) {
		auto& input = view.get<Input>(entity);
		auto& movement = view.get<MovementState>(entity);

		movement.forward = 0;
		movement.strafe = 0;
		movement.fly = 0;

		if (window.isDown('w'))
			movement.forward -= 1;
		if (window.isDown('s'))
			movement.forward += 1;

		if (window.isDown('d'))
			movement.strafe += 1;
		if (window.isDown('a'))
			movement.strafe -= 1;

		if (window.isDown(' '))
			movement.fly += 1;
		if (window.isDown(Window::Keys::LEFT_SHIFT))
			movement.fly -= 1;

		movement.fire = window.isPressed('o');

		glm::vec2 delta = pos - input.lastCursorPos;

		movement.rotate = -delta;

		input.lastCursorPos = pos;
	}

	auto camera_view = registry.view<Input, Camera>();
	for (auto entity : camera_view) {
		auto& input = camera_view.get<Input>(entity);
		auto& camera = camera_view.get<Camera>(entity);
		
		camera.distance -= window.getScrollDelta() / 1.0f;
		camera.phi -= window.getScrollDelta() / 7.0f;
		camera.theta -= window.isDown(Window::Mouse::RIGHT_CLICK) ? (pos.x - input.lastCursorPos.x) / 200 : 0;
		
		if(window.isDown(Window::Mouse::RIGHT_CLICK))
			input.lastCursorPos = pos;
	}
}