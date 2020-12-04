#pragma once

#include <vector>
#include "Graphics/VAO.h"
#include "Graphics/VBO.h"

namespace LOA::Graphics {

	struct alignas(4) RenderData {
		glm::vec4 pos_size;
		glm::u8vec4 color;
		glm::f32 index;
	};

	class ParticleRenderData {
	public:
		ParticleRenderData(size_t max_size);
		void streamData(const std::vector<RenderData>& data);
		VAO vao;
		VBO quad;
		VBO instance_data;
		VBO ebo;
		size_t max_size;
		int instances = 0;
	};
}