#pragma once

#include "common.h"

namespace LOA::Graphics {
	class VAO {
	public:
		VAO();
		~VAO();
		VAO(VAO&& other);

		VAO(const VAO&) = delete;
		VAO& operator=(const VAO&) = delete;

		VAO& operator=(VAO&& other);

		void bind();
		void unbind();

		void addVertexAttribPtr(int ptr, int size, size_t stride, size_t offset);

		GLuint getID() const;

	private:
		GLuint vaoID = 0;

		void dispose();

	};
}