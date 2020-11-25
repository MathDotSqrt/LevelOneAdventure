#pragma once

#include <string>

struct GLFWwindow;

namespace LOA {
	class Window {
	public:
		static Window& createInstance(int width, int height, std::string title);
		static Window& getInstance();
		static void destroyInstance();

		void update() const;
		bool shouldClose() const;

	private:
		static Window* singleton;
		GLFWwindow* window;

		Window(int width, int height, std::string title);
	};
}