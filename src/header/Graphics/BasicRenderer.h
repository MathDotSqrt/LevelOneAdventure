#pragma once

#include "Graphics/VAO.h"
#include "Graphics/VBO.h"

namespace LOA::Graphics {
	class BasicRenderer {
	public:
		BasicRenderer();
		void render(float dt);

	private:
		VAO vao;
		VBO vbo;
		VBO ebo;
	};
}