#pragma once

#include "common.h"
#include "Graphics/TEX.h"
#include <glm/glm.hpp>
#include <vector>
namespace LOA::Graphics {
	class FBO {
	public:
		FBO(int width, int height);
		FBO(const FBO &) = delete;
		FBO(FBO&& other);
		~FBO();

		FBO& operator=(const FBO& other) = delete;
		FBO& operator=(FBO&& other);

		void addColorAttachment(TEX::Builder texSettings);
		void addDepthAttachment(TEX::Builder texSettings);

		void blitDepthbuffer(const FBO& other);
		void blitDepthbuffer(const FBO& other, int blit_width, int blit_height);

		void bind() const;
		void bind(int width, int height) const;
		void unbind() const;

		void bindAllColorAttachments() const;

		glm::vec2 getActualSize(glm::vec2 window_size) const;

		int getWidth() const;
		int getHeight() const;

		GLuint getID() const;

		const TEX& getColorAttachment(int i=0) const;
		const TEX& getDepthAttachement() const;

	private:
		void dispose();

		GLuint fboID = 0;
		int width, height;

		std::vector<TEX> color;
		std::vector<TEX> depth;
	};
}