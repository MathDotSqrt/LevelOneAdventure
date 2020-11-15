#include "Graphics/BasicRenderer.h"
#include <GL/glew.h>
#include <iostream>
using namespace LOA::Graphics;

BasicRenderer::BasicRenderer() {
	glewExperimental = GL_TRUE;
	glewInit();
	std::cout << "TEST\n";
}

void BasicRenderer::render(float dt) {
	glClearColor(sin(dt), cos(dt), 1-sin(dt), 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}