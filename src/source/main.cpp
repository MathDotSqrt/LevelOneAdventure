#pragma once

#include <iostream>
#include "header.h"
#include <GLFW/glfw3.h>

int main() {
	glfwInit();
	std::cout << "Hello World: " << LOA::getReturn() << '\n';
	return 0;
}