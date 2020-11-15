#include "Graphics/VBO.h"

#include <assert.h>
#include <GL/glew.h>

using namespace LOA::Graphics;

enum class VBO::BufferType : GLenum {
	VERTEX_ARRAY = GL_VERTEX_ARRAY,
	ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER
};

VBO::VBO(VBO::BufferType type) : vboID(0), type(type){
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
	glBindBuffer((GLenum)type, vboID);
}

void VBO::unbind() {
	glBindBuffer((GLenum)type, 0);
}

void VBO::bufferData(size_t bytes, void* data, GLenum drawType) {
	glBufferData((GLenum)type, bytes, data, drawType);
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