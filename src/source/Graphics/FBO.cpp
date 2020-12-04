#include "Graphics/FBO.h"
#include "Graphics/TEX.h"
#include "GL/glew.h"
#include "Window.h"

#include <assert.h>

using namespace LOA::Graphics;

FBO::FBO(int width, int height) : 
	width(width), 
	height(height), 
	color(TEX::Builder().rgb().clampToEdge().linear().buildTexture(width, height)),
	depth(TEX::Builder().depth24().clampToEdge().linear().buildTexture(width, height)){

	glGenFramebuffers(1, &fboID);
	assert(fboID);

	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.getTexID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth.getTexID(), 0);
	unbind();
}

FBO::FBO(FBO&& other) : 
	fboID(other.fboID),
	color(std::move(other.color)),
	depth(std::move(other.depth)){
	
	other.fboID = 0;
}

FBO::~FBO() {
	if (fboID) {
		dispose();
	}
}

FBO& FBO::operator=(FBO&& other) {
	if (this != &other) {
		dispose();
		std::swap(fboID, other.fboID);
		std::swap(color, other.color);
		std::swap(depth, other.depth);
	}

	return *this;
}

void FBO::dispose() {
	if (fboID) {
		glDeleteFramebuffers(1, &fboID);
		fboID = 0;
	}
}

void FBO::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glViewport(0, 0, width, height);
}

void FBO::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	auto& window = Window::getInstance();
	glViewport(0, 0, window.getWidth(), window.getHeight());
}

int FBO::getWidth() const {
	return width;
}

int FBO::getHeight() const {
	return height;
}

const TEX& FBO::getColorAttachment() const {
	return color;
}

const TEX& FBO::getDepthAttachement() const {
	return depth;
}
