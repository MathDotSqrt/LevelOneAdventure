#include "Window.h"
#include <assert.h>
#include <cctype>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace LOA;

Window* Window::singleton = nullptr;

void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

static void LOA::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        Window::getInstance().hasFocus = false;
    }
}

static void LOA::internal_focus_callback(GLFWwindow* window, int focused) {
    //if (focused) {
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //    has_focus = true;
    //}
    //else {
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //    has_focus = false;
    //}
}

static void LOA::internal_mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Window::getInstance().hasFocus = true;
    }
}

static void LOA::internal_scroll_callback(GLFWwindow* window, double x, double y) {
    Window::getInstance().scrollDelta = y;
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

Window::Window(int width, int height, std::string title) : width(width), height(height){
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(width, height, "OpenGL Boilerplate", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowFocusCallback(window, internal_focus_callback);
    glfwSetMouseButtonCallback(window, internal_mouse_callback);
    glfwSetScrollCallback(window, LOA::internal_scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);
    isMouseDisabled = true;

    glfwSwapInterval(1);

}

void Window::update() {
    scrollDelta = 0;
    glfwSwapBuffers(window);
    
    //The first valid char is 32
    for (int i = 32; i < 348; i++) {
        prev_keys[i] = isDown(i);
    }

    glfwPollEvents();
}

bool Window::isDown(char c) const {
    return GLFW_PRESS == glfwGetKey(window, toupper(c));
}

bool Window::isDown(int i) const {
    return GLFW_PRESS == glfwGetKey(window, i);
}

bool Window::isDown(Keys key) const {

    switch (key) {
    case Keys::LEFT_SHIFT:
        return GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    case Keys::LEFT_CTRL:
        return GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    case Keys::ESC:
        return GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE);
    default:
        return false;
    }
}

bool Window::isPressed(char c) const {
    return isDown(c) && !prev_keys[toupper(c)];
}

bool Window::isPressed(Keys key) const {
    return isDown(key) && !prev_keys[GLFW_KEY_LEFT_SHIFT];
}

bool Window::isClick(Mouse mouse) const {
    switch (mouse) {
    case Mouse::LEFT_CLICK:
        return GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    case Mouse::RIGHT_CLICK:
        return GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    default:
        return false;
    }
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

glm::vec2 Window::getMousePos() const {
    static double x, y;
    if(hasFocus)
        glfwGetCursorPos(window, &x, &y);
    return glm::vec2(x, y);
}

float Window::getScrollDelta() const {
    return scrollDelta;
}

int Window::getWidth() const {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return width;
}

int Window::getHeight() const {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return height;
}