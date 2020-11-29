#include "Graphics/VAO.h"

#include <GL/glew.h>
#include <algorithm>
#include <iostream>

using namespace LOA::Graphics;

VAO::VAO() {
	glGenVertexArrays(1, &vaoID);
}

VAO::~VAO() {
	dispose();
}

VAO::VAO(VAO&& other) : vaoID(other.vaoID){
	other.vaoID = 0;
}

VAO& VAO::operator=(VAO&& other) {
	if (this != &other) {
		dispose();

		std::swap(vaoID, other.vaoID);
	}

	return *this;
}

void VAO::bind() {
	glBindVertexArray(vaoID);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

void VAO::addVertexAttribPtr(u32 ptr, u8 num_components, size_t stride, size_t offset) {
	glEnableVertexAttribArray(ptr);
	glVertexAttribPointer(ptr, num_components, GL_FLOAT, false, stride, (void*)offset);
}

void VAO::vertexAttribDivisor(u32 ptr, u32 divisor) {
	glVertexAttribDivisor(ptr, divisor);
}

void VAO::dispose() {
	if (vaoID) {
		glDeleteVertexArrays(1, &vaoID);
		vaoID = 0;
	}
}

GLuint VAO::getID() const {
	return vaoID;
}