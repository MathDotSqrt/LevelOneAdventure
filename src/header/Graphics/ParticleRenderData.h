#pragma once

#include <vector>
#include "Graphics/VAO.h"
#include "Graphics/VBO.h"

namespace LOA::Graphics {

	struct RenderData {
		glm::vec4 pos_size;
		glm::u8vec4 color;
	};

	class ParticleRenderData {
		ParticleRenderData(int max_size);
		void streamData(const std::vector<RenderData>& data);
	private:
		VAO vao;
		VBO quad;
		VBO instance_data;
		VBO ebo;
		int max_size;
	};
}