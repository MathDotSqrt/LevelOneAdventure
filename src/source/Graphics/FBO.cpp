#include "Graphics/FBO.h"
#include "Graphics/TEX.h"
#include "GL/glew.h"
#include "Window.h"

#include <assert.h>

using namespace LOA::Graphics;

FBO::FBO(int width, int height) : 
	width(width), 
	height(height) {

	glGenFramebuffers(1, &fboID);
	assert(fboID);
}

FBO::FBO(FBO&& other) : 
	fboID(other.fboID),
	width(other.width),
	height(other.height),
	color(std::move(other.color)),
	depth(std::move(other.depth)){
	
	//invalidate other
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

void FBO::addColorAttachment(TEX::Builder texSettings) {
	color.push_back(texSettings.buildTexture(width, height));
	size_t index = color.size() - 1;

	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, color[index].getTexID(), 0);

	std::vector<GLenum> attachments;
	for (int i = 0; i < color.size(); i++) {
		attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	glDrawBuffers(attachments.size(), attachments.data());
	unbind();
}

void FBO::addDepthAttachment(TEX::Builder texSettings) {
	assert(depth.size() == 0);

	depth.push_back(texSettings.buildTexture(width, height));
	
	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth[0].getTexID(), 0);
	unbind();
}

void FBO::blitDepthbuffer(const FBO& other) {
	blitDepthbuffer(other, width, height);
}

void FBO::blitDepthbuffer(const FBO& other, int blit_width, int blit_height) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, other.getID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getID());
	glBlitFramebuffer(0, 0, blit_width, blit_height, 0, 0, blit_width, blit_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
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

void FBO::bindAllColorAttachments() const {
	for (int i = 0; i < color.size(); i++) {
		color[i].bindActiveTexture(i);
	}
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

GLuint FBO::getID() const {
	return fboID;
}

const TEX& FBO::getColorAttachment(int i) const {
	assert(i >= 0 && i < color.size());
	return color[i];
}

const TEX& FBO::getDepthAttachement() const {
	assert(depth.size() == 1);
	return depth[0];
}
