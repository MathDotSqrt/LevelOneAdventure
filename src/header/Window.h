#pragma once

#include <string>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace LOA {
	class Window {
	public:
		enum class Keys {
			LEFT_SHIFT,
			LEFT_CTRL,
			ESC
		};

		static Window& createInstance(int width, int height, std::string title);
		static Window& getInstance();
		static void destroyInstance();

		void update() const;

		bool isPressed(char c) const;
		bool isPressed(Keys keys) const;
		bool shouldClose() const;

		glm::vec2 getMousePos() const;

		int getWidth() const;
		int getHeight() const;

	private:
		static Window* singleton;
		GLFWwindow* window;
		int width;
		int height;
		bool isMouseDisabled;

		Window(int width, int height, std::string title);
	};
}