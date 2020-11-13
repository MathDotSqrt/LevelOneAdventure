#pragma once

#include <iostream>
#include "header.h"
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <glm/glm.hpp>

int main() {
	glm::vec3 test(1, 2, 3);
	glfwInit();
	std::cout << "Hello World: " << LOA::getReturn() << '\n';
	return 0;
}