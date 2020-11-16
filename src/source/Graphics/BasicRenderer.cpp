#include "Graphics/BasicRenderer.h"
#include <GL/glew.h>
#include <iostream>

#include <vector>
#include <glm/glm.hpp>

#include "common.h"
using namespace LOA::Graphics;

BasicRenderer::BasicRenderer() : 
	vbo(VBO::BufferType::ARRAY_BUFFER), 
	ebo(VBO::BufferType::ELEMENT_ARRAY_BUFFER) {
	
	std::vector<glm::vec3> verticies = {
		glm::vec3(-.5, .5, 0),
		glm::vec3(-.5, -.5, 0),
		glm::vec3(.5, .5, 0),
		glm::vec3(.5, -.5, 0),
	};
	
	std::vector<u32> indices = {
		0, 1, 2,
		2, 1, 3
	};

	vao.bind();
		vbo.bind();
			vao.addVertexAttribPtr(0, 3, 0, 0);
			vbo.bufferData(verticies);
		vbo.unbind();
		ebo.bind();
		ebo.bufferData(indices);
	vao.unbind();


	std::cout << ebo.getNumBytes()/sizeof(u32);
}

void BasicRenderer::render(float dt) {
	glClearColor(sin(dt), cos(dt), 1-sin(dt), 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vao.bind();
	glEnableVertexAttribArray(0);
	glDrawElements(GL_TRIANGLES, ebo.getNumBytes()/sizeof(u32), GL_UNSIGNED_INT, (void*)0);
	vao.unbind();
}