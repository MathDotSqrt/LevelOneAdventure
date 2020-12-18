#include "Graphics/BasicRenderer.h"
#include <GL/glew.h>
#include <algorithm>
#include "common.h"
#include "Window.h"
#include "Util/Noise.h"
#include "Graphics/GeometryBuilder.h"
#include "Util/Timer.h"
#include "Physics/PhysicsScene.h"
#include "Physics/PhysicsDebugDrawer.h"

using namespace LOA::Graphics;

BasicRenderer::BasicRenderer() :
	//noise3D(TEX::Builder().floatType().r().linear().mirrorRepeat().buildTexture3D(Util::gen_simplex_3D_texture(64, .05)))
	noise3D(TEX::Builder().floatType().r().linear().mirrorRepeat().mipmapLinear().buildTexture3D(Util::gen_perlin_3D_texture(64, .1f))),
	postProcess(shaders, 3440, 1440) {

	using namespace entt;

	glEnable(GL_CULL_FACE);
	
	shaders.load(NormalMaterial::ShaderID, "normal/normal.vert", "normal/normal.frag");
	shaders.load(BasicLitMaterial::ShaderID, "basic_lit/basic_lit.vert", "basic_lit/basic_lit.frag");
	shaders.load(DissolveMaterial::ShaderID, "dissolve_lit/dissolve_lit.vert", "dissolve_lit/dissolve_lit.frag");
	shaders.load(ParticleMaterial::ShaderID, "color_particle/particle.vert", "color_particle/particle.frag");
	shaders.load(FireParticleMaterial::ShaderID, "fire_particle/fire_particle.vert", "fire_particle/fire_particle.frag");
	shaders.load(LineMaterial::ShaderID, "basic_color/basic_color.vert", "basic_color/basic_color.frag");
	//glEnable(GL_MULTISAMPLE);
}

void BasicRenderer::prerender(const Scene& scene, bool drawPhysicsDebug) {
	Util::Timer timer("Prerender");
	auto& window = Window::getInstance();

	current_width = window.getWidth();
	current_height = window.getHeight();

	drawList.clear();

	for (u32 i = 0; i < scene.instances.size(); i++) {
		const auto& instance = scene.instances[i];
		const RenderStateKey renderKey{ instance.materialType };
		const RenderStateKeyValue renderCall{renderKey, i};
		
		drawList.push_back(renderCall);
	}

	for (u32 i = 0; i < scene.particleSystemInstances.size(); i++) {
		const auto& instance = scene.particleSystemInstances[i];
		const RenderStateKey renderKey{ BlendType::MUL, instance.materialType };
		const RenderStateKeyValue renderCall{ renderKey, i };
		drawList.push_back(renderCall);
	}

	if(drawPhysicsDebug){
		const RenderStateKey renderKey{ BlendType::OPAQUE, MaterialType::LINE_MATERIAL_ID };
		const RenderStateKeyValue renderCall{ renderKey, 0 };
		drawList.push_back(renderCall);
	}

	std::sort(drawList.begin(), drawList.end());
}

void BasicRenderer::setViewPort(const Scene& scene, ViewPort port) {
	postProcess.bindMainViewPort(current_width, current_height);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const PerspectiveCamera camera = scene.mainCamera;
	projection = glm::perspective(camera.fov, current_width / (float)current_height, camera.near, camera.far);
}

void BasicRenderer::setBlendType(const Scene& scene, BlendType blend) {
	switch (blend) {
	case BlendType::OPAQUE:
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		break;
	case BlendType::ADD:
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	case BlendType::MUL:
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	default:
		break;
	}
}

void BasicRenderer::render(const Scene &scene, const Physics::PhysicsScene* physicsScene) {
	bool debugPhysicsDraw = physicsScene != nullptr;
	
	prerender(scene, debugPhysicsDraw);

	Util::Timer timer("Render");

	draw_iterator start = drawList.begin();
	draw_iterator end = drawList.end();

	ViewPort prev_viewport = ViewPort::NUM_VIEW_PORTS;	//dummy
	BlendType prev_blend = BlendType::NUM_BLEND_TYPES;	//dummy

	while (start != end) {
		RenderStateKey key = start->getKey();
		
		ViewPort current_viewport = key.getViewPort();
		if (prev_viewport != current_viewport) {
			setViewPort(scene, current_viewport);
			prev_viewport = current_viewport;
		}

		BlendType current_blend = key.getBlendType();
		if (prev_blend != current_blend) {
			setBlendType(scene, current_blend);
			prev_blend = current_blend;
		}

		MaterialType current_material = key.getMaterialType();
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
		case MaterialType::PARTICLE_MATERIAL_ID:
			start = renderParticle(scene, start, end);
			break;
		case MaterialType::FIRE_PARTICLE_ID:
			start = renderFireParticle(scene, start, end);
			break;
		case MaterialType::LINE_MATERIAL_ID:
			start = renderPhysicsDebug(scene, physicsScene, start, end);
			break;
		default:
			start++;
		}

		clearOpenGLState();
	}

	postProcess.unbind();
	postProcess.renderPostProcess(shaders, current_width, current_height);

}

BasicRenderer::draw_iterator
BasicRenderer::renderNormal(const Scene& scene, draw_iterator start, draw_iterator end) {
	if (start == end) {	//Nothing else to render
		return end;
	}

	auto shader = shaders.get(NormalMaterial::ShaderID);
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

	auto shader = shaders.get(BasicLitMaterial::ShaderID);
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
		glDrawElements(GL_TRIANGLES, mesh->ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0);
		diffuse->unbind();

		start++;
	}

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderDissolve(const Scene& scene, draw_iterator start, draw_iterator end) {
	if (start == end) {
		//nothing else to render
		return end;
	}

	auto shader = shaders.get(DissolveMaterial::ShaderID);
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
		glDrawElements(GL_TRIANGLES, mesh->ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0);
		diffuse->unbind();

		start++;
	}

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderParticle(const Scene &scene, draw_iterator start, draw_iterator end) {
	if (start == end) {
		//nothing else to render
		return end;
	}

	auto shader = shaders.get(ParticleMaterial::ShaderID);
	if (!shader) {
		return end;
	}

	shader->start();
	shader->setUniformMat4("P", projection);
	shader->setUniformMat4("V", scene.mainCamera.transform);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		auto& particle_system_instances = scene.particleSystemInstances[instance_id];
		auto& render_data = particle_system_instances.data;
		auto& transform = particle_system_instances.transform;
		
		shader->setUniformMat4("M", transform);
		
		render_data.vao.bind();
		render_data.ebo.bind();
		auto prim_count = render_data.instances;
		glDrawElementsInstanced(GL_TRIANGLES, render_data.ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0, prim_count);

		start++;
	}

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderFireParticle(const Scene &scene, draw_iterator start, draw_iterator end) {
	
	if (start == end) {
		//nothing else to render
		return end;
	}

	auto shader = shaders.get(FireParticleMaterial::ShaderID);
	if (!shader) {
		return end;
	}

	shader->start();
	shader->setUniformMat4("P", projection);
	shader->setUniformMat4("V", scene.mainCamera.transform);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		auto& particle_system_instances = scene.particleSystemInstances[instance_id];
		auto& fire_material = scene.fireParticleMaterials[particle_system_instances.materialID];
		auto& texture = scene.texCache.handle(fire_material.diffuse_grid);
		auto& render_data = particle_system_instances.data;
		auto& transform = particle_system_instances.transform;

		shader->setUniformMat4("M", transform);
		shader->setUniform1i("diffuse", 0);
		texture->bindActiveTexture(0);

		render_data.vao.bind();
		render_data.ebo.bind();
		auto prim_count = render_data.instances;
		glDrawElementsInstanced(GL_TRIANGLES, render_data.ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0, prim_count);
		texture->unbind();
		start++;
	}

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderPhysicsDebug(const Scene& scene, const Physics::PhysicsScene* physicsScene, draw_iterator start, draw_iterator end) {
	if (start == end) {
		//nothing else to render
		return end;
	}

	const VAO& vao = physicsScene->getDrawer()->getVAO();
	size_t num_elements = physicsScene->getDrawer()->getNumElements();
	
	//nothing to draw
	if (num_elements == 0)
		return ++start;

	//Shader failed to compile
	auto shader = shaders.get(LineMaterial::ShaderID);
	if (!shader) {
		return end;
	}

	assert(physicsScene != nullptr);

	
	shader->start();
	shader->setUniformMat4("VP", projection * scene.mainCamera.transform);

	vao.bind();
	glDrawArrays(GL_LINES, 0, num_elements);

	return ++start;
}


void BasicRenderer::renderPostprocess() {
	
}

void BasicRenderer::clearOpenGLState() {
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


