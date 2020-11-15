#pragma once
#include <vector>

#include "common.h"
namespace LOA::Graphics {

	class VBO {
	public:
		VBO(GLenum bufferType);
		VBO(VBO &&other) noexcept;
		~VBO();

		VBO(const VBO&) = delete;
		VBO& operator =(const VBO&) = delete;
		VBO& operator=(VBO&& other) noexcept;

		void bind();
		void unbind();

		void bufferData(size_t bytes, void* data, GLenum drawType);

		template<typename T>
		void bufferData(const std::vector<T>& data, GLenum drawType) {
			void* data_ptr = (void*)data.data();
			bufferData(sizeof(T) * data.size(), data_ptr, drawType);
		}

		GLuint getID() const;
	private:
		GLuint vboID;
		GLenum bufferType;

		void dispose();
	};
}