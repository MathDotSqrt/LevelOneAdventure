#include "Graphics/VAO.h"

#include <GL/glew.h>
#include <algorithm>

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

void VAO::addVertexAttribPtr(int ptr, int size, size_t stride, size_t offset) {
	glVertexAttribPointer(ptr, size, GL_FLOAT, false, stride, (void*)offset);
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