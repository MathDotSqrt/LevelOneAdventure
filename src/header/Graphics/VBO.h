#pragma once
#include <vector>

#include "common.h"
namespace LOA::Graphics {
	

	class VBO {
	public:
		
		enum class BufferType : GLenum;

		VBO(BufferType type);
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
		BufferType getType();
	private:
		GLuint vboID;
		BufferType type;

		void dispose();
	};
}