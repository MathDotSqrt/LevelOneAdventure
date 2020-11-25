#include "Window.h"
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace LOA;

Window* Window::singleton = nullptr;



Window& Window::createInstance(int width, int height, std::string title) {
	assert(Window::singleton == nullptr);

	singleton = new Window(width, height, title);

	return Window::getInstance();
}

Window& Window::getInstance() {
	assert(Window::singleton != nullptr);
	return *Window::singleton;
}

void Window::destroyInstance() {
    glfwDestroyWindow(Window::getInstance().window);
    glfwTerminate();
}

Window::Window(int width, int height, std::string title) {
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    window = glfwCreateWindow(1024, 1024, "OpenGL Boilerplate", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    //glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);
}

void Window::update() const {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}