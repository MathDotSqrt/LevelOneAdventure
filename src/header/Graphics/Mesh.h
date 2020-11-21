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
		Mesh(const Geometry<T...> &geometry, TEX &&tex) : 
			vbo(VBO::BufferType::ARRAY_BUFFER),
			ebo(VBO::BufferType::ELEMENT_ARRAY_BUFFER),
			tex(std::move(tex)){
			
			vao.bind();
			
			vbo.bind();
			vao.addVertexAttribPtr<PositionAttrib, NormalAttrib, TexcoordAttrib>();
			vbo.bufferData(geometry.getVerticies());
			vbo.unbind();

			ebo.bind();
			ebo.bufferData(geometry.getIndices());
			ebo.unbind();

			vao.unbind();

		}

		VAO vao;
		VBO vbo;
		VBO ebo;
		TEX tex;
	};
}