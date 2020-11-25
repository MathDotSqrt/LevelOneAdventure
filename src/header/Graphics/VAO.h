#pragma once

#include "common.h"
#include "Attrib.h"

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

		void addVertexAttribPtr(u32 ptr, u8 num_components, size_t stride, size_t offset);

		template<typename ...ATTRIBS>
		void addVertexAttribPtr() {
			const auto stride = getAttribsStride<ATTRIBS...>();
			setInterleavedAttribPointers<ATTRIBS...>(stride, 0);
		}

		GLuint getID() const;

	private:
		GLuint vaoID = 0;

		void dispose();

		template<typename ATTRIB, typename ...U>
		constexpr size_t getAttribsStride() {
			if constexpr (!sizeof...(U)) {
				return ATTRIB::size();
			}
			else {
				return ATTRIB::size() + getAttribsStride<U...>();
			}
		}

		template<typename ATTRIB, typename ...U>
		void setInterleavedAttribPointers(size_t stride, size_t offset) {
			addVertexAttribPtr(ATTRIB::Location, ATTRIB::NumComponents, stride, offset);
			if constexpr (sizeof...(U)) {
				setInterleavedAttribPointers<U...>(stride, offset + ATTRIB::size());
			}
		}

	};
}