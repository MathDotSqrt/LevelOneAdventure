#include "Graphics/Attrib.h"
#include "GL/glew.h"

template<>
GLenum LOA::Graphics::getGLType<u8>() {
	return GL_UNSIGNED_BYTE;
}

template<>
GLenum LOA::Graphics::getGLType<i8>() {
	return GL_BYTE;
}

template<>
GLenum LOA::Graphics::getGLType<u16>() {
	return GL_UNSIGNED_SHORT;
}

template<>
GLenum LOA::Graphics::getGLType<i16>() {
	return GL_SHORT;
}

template<>
GLenum LOA::Graphics::getGLType<u32>() {
	return GL_UNSIGNED_INT;
}

template<>
GLenum LOA::Graphics::getGLType<i32>() {
	return GL_INT;
}


template<>
GLenum LOA::Graphics::getGLType<f32>() {
	return GL_FLOAT;
}

template<>
GLenum LOA::Graphics::getGLType<f64>() {
	return GL_DOUBLE;
}