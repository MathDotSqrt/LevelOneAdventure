#pragma once

#include <string>
#include <glm/glm.hpp>
#include <bitset>
struct GLFWwindow;

namespace LOA {

	//free functions for glfw's c style callbacks
	void key_callback(GLFWwindow*, int, int, int, int);
	void internal_focus_callback(GLFWwindow*, int);
	void internal_mouse_callback(GLFWwindow*, int, int, int);
	void internal_scroll_callback(GLFWwindow*, double, double);

	class Window {
	public:
		enum class Keys {
			RIGHT_ARROW,
			LEFT_ARROW,
			UP_ARROW,
			DOWN_ARROW,
			LEFT_SHIFT,
			LEFT_CTRL,
			ESC
		};

		enum class Mouse {
			RIGHT_CLICK,
			LEFT_CLICK
		};


		static Window& createInstance(int width, int height, std::string title);
		static Window& getInstance();
		static void destroyInstance();

		void update();

		bool isDown(char c) const;
		bool isDown(int i) const;
		bool isDown(Keys keys) const;
		bool isDown(Mouse mouse) const;

		bool isPressed(char c) const;
		bool isPressed(Keys keys) const;
		bool isPressed(Mouse mouse) const;

		bool isClick(Mouse mouse) const;
		bool shouldClose() const;

		glm::vec2 getMousePos() const;
		float getScrollDelta() const;

		int getWidth() const;
		int getHeight() const;

		//allow access to window private variables
		friend void key_callback(GLFWwindow*, int, int, int, int);
		friend void internal_focus_callback(GLFWwindow*, int);
		friend void internal_mouse_callback(GLFWwindow*, int, int, int);
		friend void internal_scroll_callback(GLFWwindow*, double, double);

	private:
		static Window* singleton;
		GLFWwindow* window;
		int width;
		int height;
		float scrollDelta = 0;
		bool hasFocus = true;
		bool isMouseDisabled;
		std::bitset<348> prev_keys;
		bool prev_left_mouse = false;
		bool prev_right_mouse = false;

		int toGLFW(Keys key) const;
		int toGLFW(Mouse mouse) const;

		Window(int width, int height, std::string title);
	
	};
}