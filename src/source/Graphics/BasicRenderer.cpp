#include "Graphics/BasicRenderer.h"
#include <GL/glew.h>
#include <algorithm>
#include "common.h"
#include "Window.h"
#include "Util/Noise.h"


using namespace LOA::Graphics;



BasicRenderer::BasicRenderer() : 
	//noise3D(TEX::Builder().floatType().r().linear().mirrorRepeat().buildTexture3D(Util::gen_perlin_3D_texture(64, .1)))
	noise3D(TEX::Builder().floatType().r().linear().mirrorRepeat().mipmapLinear().buildTexture3D(Util::gen_perlin_3D_texture(64, .1f)))

{
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
	auto& window = Window::getInstance();
	int width = window.getWidth();
	int height = window.getHeight();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

	const PerspectiveCamera camera = scene.mainCamera;
	projection = glm::perspective(camera.fov, width / (float)height, camera.near, camera.far);
}

void BasicRenderer::render(const Scene &scene) {
	prerender(scene);

	draw_iterator start = drawList.begin();
	draw_iterator end = drawList.end();

	setViewPort(scene, start);

	while (start != end) {
		const RenderStateKey key = start->getKey();
		const MaterialType current_material = key.getMaterialType();

		switch (current_material) {
		case MaterialType::NORMAL_MATERIAL_ID:
			start = renderNormal(scene, start, end);
			break;
		case MaterialType::BASIC_LIT_MATERIAL_ID:
			start = renderBasicLit(scene, start, end);
			break;
		case MaterialType::DISSOLVE_MATERIAL_ID:
			start = renderDissolve(scene, start, end);
			break;
		default:
			start++;
		}

		clearOpenGLState();
	}

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
	shader->setUniformMat4("VP", projection * scene.mainCamera.transform);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		auto instance = scene.instances[instance_id];
		auto &mesh = instance.mesh;

		auto &transform = instance.transform;
		glm::mat3 inverse = glm::inverse(glm::mat3(transform));

		shader->setUniformMat3("inverse_transpose", inverse, true);
		shader->setUniformMat4("M", transform);

		mesh->vao.bind();
		mesh->ebo.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0);

		start++;
	}
	shader->end();
	
	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderBasicLit(const Scene& scene, draw_iterator start, draw_iterator end) {
	if (start == end) {	//Nothing else to render
		return end;
	}

	auto shader = shaders.getShader({ "basic_lit/basic_lit.vert", "basic_lit/basic_lit.frag" });
	if (!shader) {
		return end;
	}

	shader->start();
	shader->setUniformMat4("VP", projection * scene.mainCamera.transform);

	loadPointLights(scene, *shader);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		auto instance = scene.instances[instance_id];
		auto& mesh = instance.mesh;
		auto& material = scene.basicLitMaterials[instance.materialID];
		auto& diffuse = scene.texCache.handle(material.diffuse);


		auto &transform= instance.transform;
		glm::mat3 inverse = glm::inverse(glm::mat3(transform));

		shader->setUniformMat3("inverse_transpose", inverse, true);
		shader->setUniformMat4("M", transform);
		
		shader->setUniform1i("diffuse", 0);
		
		diffuse->bindActiveTexture(0);

		mesh->vao.bind();
		mesh->ebo.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glEnableVertexAttribArray(TEXCOORD_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0);
		diffuse->unbind();

		start++;
	}

	return end;
}

BasicRenderer::draw_iterator
BasicRenderer::renderDissolve(const Scene& scene, draw_iterator start, draw_iterator end) {
	if (start == end) {
		//nothing else to render
		return end;
	}

	auto shader = shaders.getShader({ "dissolve_lit/dissolve_lit.vert", "dissolve_lit/dissolve_lit.frag" });
	if (!shader) {
		return end;
	}

	shader->start();
	shader->setUniformMat4("VP", projection * scene.mainCamera.transform);

	loadPointLights(scene, *shader);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		auto instance = scene.instances[instance_id];
		auto& mesh = instance.mesh;
		auto& material = scene.dissolveMaterials[instance.materialID];
		auto& diffuse = scene.texCache.handle(material.diffuse);


		auto& transform = instance.transform;
		glm::mat3 inverse = glm::inverse(glm::mat3(transform));

		shader->setUniformMat3("inverse_transpose", inverse, true);
		shader->setUniformMat4("M", transform);

		shader->setUniform1f("u_dissolve", material.time);
		shader->setUniform1f("u_offset", material.offset);
		shader->setUniform3f("u_dissolve_color", material.dissolve_color);

		shader->setUniform1i("diffuse", 0);
		shader->setUniform1i("noise", 1);

		diffuse->bindActiveTexture(0);
		noise3D.bindActiveTexture(1);

		mesh->vao.bind();
		mesh->ebo.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glEnableVertexAttribArray(TEXCOORD_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0);
		diffuse->unbind();

		start++;
	}

	return end;
}

void BasicRenderer::clearOpenGLState() {
	glDisableVertexAttribArray(POSITION_ATTRIB_LOCATION);
	glDisableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
	glDisableVertexAttribArray(TEXCOORD_ATTRIB_LOCATION);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void BasicRenderer::loadPointLights(const Scene& scene, GLSLProgram &shader) {
	shader.setUniform1i("u_num_point_lights", scene.pointLights.size());
	for (int i = 0; i < scene.pointLights.size(); i++) {
		const auto& point = scene.pointLights[i];
		const auto index = std::to_string(i);
		shader.setUniform3f("u_point_lights[" + index + "].pos", point.position);
		shader.setUniform3f("u_point_lights[" + index + "].color", point.color);
		shader.setUniform1f("u_point_lights[" + index + "].intensity", point.intensity);
	}
}

glm::mat4 
BasicRenderer::makeTransform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) const{
	constexpr auto identity = glm::identity<glm::mat4>();
	glm::mat4 translation = glm::translate(identity, pos);
	glm::mat4 rotation = translation * glm::toMat4(rot);
	glm::mat4 transformation = glm::scale(rotation, scale);

	return transformation;
}


