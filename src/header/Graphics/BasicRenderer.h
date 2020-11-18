#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Graphics/VAO.h"
#include "Graphics/VBO.h"
#include "Graphics/TEX.h"
#include "Graphics/ShaderSet.h"


namespace LOA::Graphics {
	class BasicRenderer {
	public:
		BasicRenderer();
		void update(float dt);
		void render(float time);

	private:
		TEX tex;
		VAO vao;
		VBO vbo;
		VBO vbo_color;
		VBO ebo;

		ShaderSet shaders;

		glm::vec3 position = glm::vec3();
		glm::quat rotation = glm::quat(0, 0, 0, 1);
		glm::vec3 scale = glm::vec3(1);
		glm::mat4 projection;
	};
}