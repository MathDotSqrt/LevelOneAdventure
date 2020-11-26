#include "Window.h"
#include <assert.h>
#include <cctype>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace LOA;

Window* Window::singleton = nullptr;


void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


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

    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(1024, 1024, "OpenGL Boilerplate", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);
}

void Window::update() const {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Window::isPressed(char c) const {
    return GLFW_PRESS == glfwGetKey(window, toupper(c));
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}