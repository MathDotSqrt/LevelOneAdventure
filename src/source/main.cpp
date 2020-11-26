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



int main(void) {

    LOA::Window &window = LOA::Window::createInstance(1280, 1024, "Level One Adventure");

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