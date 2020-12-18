#pragma once

#include "Graphics/Attrib.h"
#include "Graphics/Geometry.h"
#include "Graphics/VAO.h"
#include "Graphics/VBO.h"
#include "Graphics/TEX.h"

#include <iostream>
namespace LOA::Graphics {
	class Mesh {
	public:
		template<typename ...T>
		Mesh(const Geometry<T...> &geometry) : 
			vbo(VBO::BufferType::ARRAY_BUFFER),
			ebo(VBO::BufferType::ELEMENT_ARRAY_BUFFER) {
			
			vao.bind();
			
			vbo.bind();
			vao.addVertexAttribPtr<T...>();
			vbo.bufferData(geometry.getVerticies());
			vbo.unbind();

			ebo.bind();
			ebo.bufferData(geometry.getIndices());
			ebo.unbind();

			vao.unbind();

			//if geometry stores 3d position
			if constexpr (Geometry<T...>::PositionType::length() == 3) {
				min = glm::vec3(geometry.getMin());
				max = glm::vec3(geometry.getMax());
			}
			else {
				min = glm::vec3(geometry.getMin(), 0);
				max = glm::vec3(geometry.getMax(), 0);
			}
		}

		VAO vao;
		VBO vbo;
		VBO ebo;

		const glm::vec3& getMin() const{
			return min;
		}

		const glm::vec3& getMax() const {
			return max;
		}
	private:
		glm::vec3 min;
		glm::vec3 max;
	};
}