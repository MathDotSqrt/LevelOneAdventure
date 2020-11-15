#include "Graphics/VBO.h"

#include <assert.h>
#include <GL/glew.h>

using namespace LOA::Graphics;

VBO::VBO(GLenum bufferType) : vboID(0), bufferType(bufferType){
	glGenBuffers(1, &this->vboID);
	assert(vboID, "FATAL: failed to generate VBO");
}

VBO::VBO(VBO&& other) noexcept : vboID(other.vboID), bufferType(other.bufferType){
	other.vboID = 0;
}

VBO::~VBO() {
	dispose();
}

VBO& VBO::operator=(VBO &&other) noexcept {
	if (this != &other) {
		dispose();
		std::swap(vboID, other.vboID);
		std::swap(bufferType, other.bufferType);
	}

	return *this;
}


void VBO::bind() {
	assert(vboID, "Fatal: failed to bind");
	glBindBuffer(bufferType, vboID);
}

void VBO::unbind() {
	glBindBuffer(bufferType, 0);
}

void VBO::bufferData(size_t bytes, void* data, GLenum drawType) {
	glBufferData(this->bufferType, bytes, data, drawType);
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