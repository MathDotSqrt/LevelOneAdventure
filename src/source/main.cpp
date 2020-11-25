#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#include "common.h"
#include "PlayState.h"
#include "Window.h"


using namespace glm;

#define GLSL(src) #src

//static void error_callback(int error, const char* description) {
//    fputs(description, stderr);
//}
//
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, GL_TRUE);
//}

int main(void) {

    //glfwSetErrorCallback(error_callback);

    LOA::Window &window = LOA::Window::createInstance(1024, 1024, "Level One Adventure");

    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::cout << "FATAL\n";
    }

    

    float t = 0;

    LOA::PlayState state;

    while (!window.shouldClose()) {
        state.update(1/60.0f);
        state.render();

        window.update();

        t += .01f;
    }

    LOA::Window::destroyInstance();
    exit(EXIT_SUCCESS);
}