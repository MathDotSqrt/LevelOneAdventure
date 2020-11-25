#include "Graphics/BasicRenderer.h"
#include <GL/glew.h>
#include <algorithm>
#include "common.h"

using namespace LOA::Graphics;



BasicRenderer::BasicRenderer() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_MULTISAMPLE);


}

void BasicRenderer::prerender(const Scene& scene) {
	drawList.clear();

	for (u32 i = 0; i < scene.instances.size(); i++) {
		const auto& instance = scene.instances[i];
		const RenderStateKey renderKey{ instance.materialType };
		const RenderStateKeyValue renderCall{renderKey, i};
		
		drawList.push_back(renderCall);
	}

	std::sort(drawList.begin(), drawList.end());
}

void BasicRenderer::setViewPort(const Scene& scene, draw_iterator current) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 1024, 1024);


	const PerspectiveCamera camera = scene.mainCamera;
	projection = glm::perspective(camera.fov, camera.aspect, camera.near, camera.far);
}

void BasicRenderer::render(const Scene &scene) {
	prerender(scene);

	draw_iterator start = drawList.begin();
	draw_iterator end = drawList.end();

	while (start != end) {
		const RenderStateKey key = start->getKey();
		const MaterialType current_material = key.getMaterialType();

		setViewPort(scene, start);

		switch (current_material) {
		case MaterialType::COLOR_MATERIAL_ID:
			start = renderColor(scene, start, end);
			break;
		case MaterialType::NORMAL_MATERIAL_ID:
			start = renderNormal(scene, start, end);
			break;
		case MaterialType::BASIC_LIT_MATERIAL_ID:
			start = renderBasicLit(scene, start, end);
			break;
		}
	}
}

BasicRenderer::draw_iterator 
BasicRenderer::renderColor(const Scene& scene, draw_iterator start, draw_iterator end) {
	if (start == end) {
		//nothing else to render
		return end;
	}
	
	auto shader = shaders.getShader({"basic/basic.vert", "basic/basic.frag"});
	if (!shader) {
		std::cout << "Failed to load Shader\n";
		return end;
	}

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderNormal(const Scene& scene, draw_iterator start, draw_iterator end) {
	if (start == end) {	//Nothing else to render
		return end;
	}

	auto shader = shaders.getShader({ "normal/normal.vert", "normal/normal.frag" });
	if (!shader) {
		return end;
	}

	shader->start();
	shader->setUniformMat4("P", projection);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		auto instance = scene.instances[instance_id];
		auto &mesh = instance.mesh;

		auto transformation = makeTransform(instance.pos, instance.rot, instance.scale);
		glm::mat3 inverse = glm::inverse(glm::mat3(transformation));

		shader->setUniformMat3("inverse_transpose", inverse, true);
		shader->setUniformMat4("M", transformation);

		mesh->vao.bind();
		mesh->ebo.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0);

		start++;
	}
	shader->end();
	glDisableVertexAttribArray(POSITION_ATTRIB_LOCATION);
	glDisableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
	glBindVertexArray(0);
	
	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderBasicLit(const Scene& scene, draw_iterator start, draw_iterator end) {
	return start;
}

glm::mat4 
BasicRenderer::makeTransform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) const{
	constexpr auto identity = glm::identity<glm::mat4>();
	glm::mat4 translation = glm::translate(identity, pos);
	glm::mat4 rotation = translation * glm::toMat4(rot);
	glm::mat4 transformation = glm::scale(rotation, scale);

	return transformation;
}


