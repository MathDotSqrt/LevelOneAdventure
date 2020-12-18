#include "Graphics/GLError.h"
#include <Gl/glew.h>

const char* LOA::Graphics::GetGLErrorStr(GLenum err) {
	switch (err)
	{
	case GL_NO_ERROR:          return "No error";
	case GL_INVALID_ENUM:      return "Invalid enum";
	case GL_INVALID_VALUE:     return "Invalid value";
	case GL_INVALID_OPERATION: return "Invalid operation";
	case GL_STACK_OVERFLOW:    return "Stack overflow";
	case GL_STACK_UNDERFLOW:   return "Stack underflow";
	case GL_OUT_OF_MEMORY:     return "Out of memory";
	default:                   return "Unknown error";
	}
}

void LOA::Graphics::CheckGLError() {
	while (true)
	{
		const GLenum err = glGetError();
		if (GL_NO_ERROR == err)
			break;

		std::cout << "GL Error: " << GetGLErrorStr(err) << std::endl;
	}
}