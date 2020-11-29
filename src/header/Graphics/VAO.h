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
		void vertexAttribDivisor(u32 ptr, u32 divisor);

		template<typename ...ATTRIBS>
		void addVertexAttribPtr(u32 divisor=0) {
			const auto stride = getAttribsStride<ATTRIBS...>();
			setInterleavedAttribPointers<ATTRIBS...>(stride, 0, divisor);
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
		void setInterleavedAttribPointers(size_t stride, size_t offset, u32 divisor) {
			addVertexAttribPtr(ATTRIB::Location, ATTRIB::NumComponents, stride, offset);
			vertexAttribDivisor(ATTRIB::Location, divisor);
			if constexpr (sizeof...(U) > 0) {
				setInterleavedAttribPointers<U...>(stride, offset + ATTRIB::size(), divisor);
			}
		}

	};
}