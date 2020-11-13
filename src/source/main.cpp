#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glfw/glfw3.h>

int main() {

	glfwInit();

	glm::vec3 test(1, 2, 3);
	std::cout << "Hello World: " << test.x << " " << test.y << " " << test.z;
	return 0;
}