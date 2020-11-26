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

		glm::vec2 delta = pos - input.lastCursorPos;

		movement.rotate = -delta;

		input.lastCursorPos = pos;
	}
}