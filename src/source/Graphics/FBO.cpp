#include "Graphics/FBO.h"
#include "Graphics/TEX.h"
#include "GL/glew.h"
#include "Window.h"

#include <assert.h>

using namespace LOA::Graphics;

FBO::FBO(int width, int height) : 
	width(width), 
	height(height), 
	color(TEX::Builder().rgb16f().clampToEdge().linear().buildTexture(width, height)),
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
	bind(this->width, this->height);
}

void FBO::bind(int width, int height) const {
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	glm::vec2 size = getActualSize(glm::vec2(width, height));
	glViewport(0, 0, size.x, size.y);
}

void FBO::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	auto& window = Window::getInstance();
	glViewport(0, 0, window.getWidth(), window.getHeight());
}

glm::vec2 FBO::getActualSize(glm::vec2 window_size) const {
	const float width = window_size.x;
	const float height = window_size.y;

	const float fbo_aspect = (float)getWidth() / getHeight();
	const float aspect = width / (float)height;

	if (width <= getWidth() && height <= getHeight()) {
		return glm::vec2(window_size);
	}
	else if (aspect > fbo_aspect) {	//align via width
		return glm::vec2(getWidth(), getWidth() / aspect);
	}
	else {	//align via height
		float a = getHeight() * aspect;
		float b = getHeight();
		return glm::vec2(getHeight() * aspect, getHeight());
	}
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
