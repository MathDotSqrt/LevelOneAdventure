#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#include "common.h"
#include "PlayState.h"
#include "Window.h"
#include "Util/Timer.h"


int main(void) {

    LOA::Window &window = LOA::Window::createInstance(1280, 1024, "Level One Adventure");

    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::cout << "FATAL\n";
    }


    int count = 0;
    LOA::PlayState state;
    auto& manager = LOA::Util::TimerManager::getInstance();
    while (!window.shouldClose()) {
        {
            LOA::Util::Timer timer("Main");
            state.update(1 / 60.0f);
            state.render();
        }
        
        window.update();

        if (count >= 60) {
            //manager.display();
            count -= 60;
        }
        manager.clear();

        count++;
    }

    LOA::Window::destroyInstance();
    exit(EXIT_SUCCESS);
}