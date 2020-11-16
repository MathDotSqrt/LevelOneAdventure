#include "Graphics/VBO.h"

#include <assert.h>
#include <GL/glew.h>

#include <iostream>

using namespace LOA::Graphics;

constexpr GLenum toGL(VBO::BufferType type) {
	switch (type)
	{
	case VBO::BufferType::ARRAY_BUFFER:
		return GL_ARRAY_BUFFER;
	case VBO::BufferType::ELEMENT_ARRAY_BUFFER:
		return GL_ELEMENT_ARRAY_BUFFER;
	default:
		assert(false, "TO GL FAILED");
		return 0;
	}
}

VBO::VBO(VBO::BufferType type) : vboID(0), type(type), bytes(0){
	glGenBuffers(1, &vboID);
	assert(vboID, "FATAL: failed to generate VBO");
}

VBO::VBO(VBO&& other) noexcept : vboID(other.vboID), type(other.type){
	other.vboID = 0;
}

VBO::~VBO() {
	dispose();
}

VBO& VBO::operator=(VBO &&other) noexcept {
	if (this != &other) {
		dispose();
		std::swap(vboID, other.vboID);
		std::swap(type, other.type);
	}

	return *this;
}


void VBO::bind() {
	assert(vboID, "Fatal: failed to bind");
	glBindBuffer(toGL(type), vboID);
}

void VBO::unbind() {
	glBindBuffer(toGL(type), 0);
}

void VBO::bufferData(size_t bytes, void* data) {
	glBufferData(toGL(type), bytes, data, GL_STATIC_DRAW);
	this->bytes = bytes;
}

void VBO::dispose() {
	if (this->vboID) {
		glDeleteBuffers(1, &vboID);
		this->vboID = 0;
	}
}

GLuint VBO::getID() const {
	return vboID;
}

VBO::BufferType VBO::getType() const {
	return type;
}

size_t VBO::getNumBytes() const {
	return bytes;
}
