#pragma once

#include "common.h"
#include "Graphics/TEX.h"

namespace LOA::Graphics {
	class FBO {
	public:
		FBO(int width, int height);
		FBO(const FBO &) = delete;
		FBO(FBO&& other);
		~FBO();

		FBO& operator=(const FBO& other) = delete;
		FBO& operator=(FBO&& other);

		void bind() const;
		void bind(int width, int height) const;
		void unbind() const;

		int getWidth() const;
		int getHeight() const;

		const TEX& getColorAttachment() const;
		const TEX& getDepthAttachement() const;

	private:
		void dispose();

		GLuint fboID = 0;
		int width, height;

		TEX color;
		TEX depth;
	};
}