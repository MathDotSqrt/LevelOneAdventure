#pragma once

#include <glm/glm.hpp>

#include "Graphics/VAO.h"
#include "Graphics/VBO.h"
#include "Graphics/ShaderSet.h"


namespace LOA::Graphics {
	class BasicRenderer {
	public:
		BasicRenderer();
		void update(float dt);
		void render(float time);

	private:
		VAO vao;
		VBO vbo;
		VBO ebo;

		ShaderSet shaders;

		glm::vec3 position = glm::vec3();
		glm::vec3 rotation = glm::vec3();
		glm::vec3 scale = glm::vec3(1);
		glm::mat4 projection;
	};
}