#include "Graphics/BasicRenderer.h"
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include "common.h"
#include "Graphics/preamble.h"
#include "Graphics/Attrib.h"

using namespace LOA::Graphics;

BasicRenderer::BasicRenderer() : 
	vbo(VBO::BufferType::ARRAY_BUFFER),
	vbo_color(VBO::BufferType::ARRAY_BUFFER),
	ebo(VBO::BufferType::ELEMENT_ARRAY_BUFFER) {
	
	std::vector<glm::vec3> verticies = {
		glm::vec3(-.5, .5, 0),
		glm::vec3(-.5, -.5, 0),
		glm::vec3(.5, .5, 0),
		glm::vec3(.5, -.5, 0),
	};

	std::vector<glm::vec3> color = {
		glm::vec3(1, 1, 1),
		glm::vec3(0, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 0, 1),
	};
	
	std::vector<u32> indices = {
		0, 1, 2,
		2, 1, 3
	};

	vao.bind();
		vbo.bind();
			vao.addVertexAttribPtr<PositionAttrib>();
			vbo.bufferData(verticies);
		vbo.unbind();
		vbo_color.bind();
			vao.addVertexAttribPtr<ColorAttrib>();
			vbo_color.bufferData(color);
		vbo_color.unbind();
		ebo.bind();
			ebo.bufferData(indices);
		ebo.unbind();
	vao.unbind();


	projection = glm::perspective<float>(70, 1, .1f, 1000.0f);
}

void BasicRenderer::update(float delta) {
	rotation = glm::rotate(rotation, .01f, glm::vec3(1, 0, 1));
	position.z -= delta;
}

void BasicRenderer::render(float time) {
	glm::mat4 transform = glm::identity<glm::mat4>();
	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, glm::angle(rotation), glm::axis(rotation));
	transform = glm::scale(transform, scale);
	


	glClearColor(sin(time), cos(time), 1-sin(time), 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto basic_shader = shaders.getShader({"basic/basic.vert", "basic/basic.frag"});

	assert(basic_shader, "SHADER IS NULL");

	basic_shader->start();
	basic_shader->setUniform1f("u_time", time);
	basic_shader->setUniformMat4("MP", projection * transform);
	vao.bind();
	ebo.bind();
	glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
	glEnableVertexAttribArray(COLOR_ATTRIB_LOCATION);
	glDrawElements(GL_TRIANGLES, ebo.getNumBytes()/sizeof(u32), GL_UNSIGNED_INT, (void*)0);
	ebo.unbind();
	vao.unbind();
	basic_shader->end();
}