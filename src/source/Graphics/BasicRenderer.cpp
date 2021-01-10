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
	white(TEX::Builder().rgb().nearest().clampToEdge().buildTexture(4, 4, std::vector(4 * 4, glm::u8vec3(255)))),	//all white texture
	//postProcess(*this, 3440, 1440)
	postProcess(*this, 3440, 1370),
	current_width(3440),
	current_height(1370)
	{

	using namespace entt;

	glEnable(GL_CULL_FACE);
	
	shaders.load(BasicMaterial::ShaderID, "basic/basic.vert", "basic/basic.frag");
	shaders.load(BasicLitMaterial::ShaderID, "basic_deferred/basic_deferred.vert", "basic_deferred/basic_deferred.frag");
	shaders.load(DissolveMaterial::ShaderID, "dissolve_lit/dissolve_lit.vert", "dissolve_lit/dissolve_lit.frag");
	shaders.load(NormalMaterial::ShaderID, "normal/normal.vert", "normal/normal.frag");
	shaders.load("LightVolumeShader"_hs, "light_volume/light_volume.vert", "light_volume/light_volume.frag");

	shaders.load(FireParticleMaterial::ShaderID, "fire_particle/fire_particle.vert", "fire_particle/fire_particle.frag");
	shaders.load(LineMaterial::ShaderID, "basic_vertex_color/basic_color.vert", "basic_vertex_color/basic_color.frag");
	//glEnable(GL_MULTISAMPLE);
}

void BasicRenderer::prerender(const Scene& scene, bool drawPhysicsDebug) {
	Util::Timer timer("Prerender");
	auto& window = Window::getInstance();

	current_width = window.getWidth();
	current_height = window.getHeight();

	postProcess.clearFrameBuffers();

	drawList.clear();

	//Add scene object draw calls
	for (u32 i = 0; i < scene.instances.size(); i++) {
		const auto& instance = scene.instances[i];
		

		const RenderStateKey renderKey = instance.material->getRenderState();
		const RenderStateKeyValue renderCall{renderKey, i};

		drawList.push_back(renderCall);
	}

	//Add point light draw calls
	for (u32 i = 0; i < scene.pointLights.size(); i++) {
		const auto& pointLight = scene.pointLights[i];

		//Settings for light volumes
		const RenderStateKey renderKey{ ViewPortLayer::DEFERRED_LIGHT, BlendType::ADD, MaterialType::LIGHT_VOLUME_MATERIAL_ID };
		const RenderStateKeyValue renderCall{ renderKey, i };
		drawList.push_back(renderCall);
	}

	//Add particle system draw calls (one call per system)
	for (u32 i = 0; i < scene.particleSystemInstances.size(); i++) {
		const auto& instance = scene.particleSystemInstances[i];
		const RenderStateKey renderKey = instance.material->getRenderState();
		const RenderStateKeyValue renderCall{ renderKey, i };
		drawList.push_back(renderCall);
	}

	//Add debug physics mesh draw call (only one if enabled)
	if(drawPhysicsDebug){
		const RenderStateKey renderKey{ ViewPortLayer::DEBUG, BlendType::OPAQUE, MaterialType::LINE_MATERIAL_ID };
		const RenderStateKeyValue renderCall{ renderKey, 0 };
		drawList.push_back(renderCall);
	}

	//Force some post processing draw calls
	{
		//0) Gaurentees post process buffer has run
		//1) Gaurentees antialiasing buffer has run
		//2) Gaurentees final draw would be made
		const RenderStateKey renderKey0{ ViewPortLayer::FORWARD, BlendType::OPAQUE, MaterialType::RENDER_COMMAND_ID };
		const RenderStateKey renderKey1{ ViewPortLayer::DEBUG, BlendType::OPAQUE, MaterialType::RENDER_COMMAND_ID };
		const RenderStateKey renderKey2{ ViewPortLayer::NUM_VIEW_PORT_LAYERS, BlendType::OPAQUE, MaterialType::RENDER_COMMAND_ID };
		
		drawList.push_back({ renderKey0, 0 });
		drawList.push_back({ renderKey1, 0 });
		drawList.push_back({ renderKey2, 0 });
	}

	std::sort(drawList.begin(), drawList.end());
}

void BasicRenderer::setViewPort(const Scene& scene, ViewPort port) {
	const PerspectiveCamera camera = scene.mainCamera;
	projection = glm::perspective(camera.fov, current_width / (float)current_height, camera.near, camera.far);
}

void BasicRenderer::setViewPortLayer(const Scene& scene, ViewPortLayer layer, ViewPortLayer prev) {
	assert(layer != prev);

	//TODO: investigate the need of the deferred light pass because only main viewport needs to be binded
	//transparent objects get rendered later anyways

	switch (prev) {
	case ViewPortLayer::DEFERRED:
		postProcess.renderDeferred(scene, *this);
		break;
	case ViewPortLayer::DEFERRED_LIGHT:	
		break;
	case ViewPortLayer::FORWARD:
		postProcess.renderPostProcess(*this);
		break;
	case ViewPortLayer::DEBUG:
		postProcess.renderAntiAlias(*this);
		break;
	default:
		break;
	}

	switch (layer) {
	case ViewPortLayer::DEFERRED:
		postProcess.bindGBuffer(current_width, current_height);
		break;
	case ViewPortLayer::DEFERRED_LIGHT:
	case ViewPortLayer::FORWARD:
		postProcess.bindPostProcessBuffer(current_width, current_height);
		break;
	case ViewPortLayer::DEBUG:
		postProcess.bindFXAA(current_width, current_height);
		break;
	default:
		break;
	}
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

	ViewPort prev_viewport = ViewPort::NUM_VIEW_PORTS;				//dummy
	ViewPortLayer prev_layer = ViewPortLayer::NUM_VIEW_PORT_LAYERS;	//dummy	
	BlendType prev_blend = BlendType::NUM_BLEND_TYPES;				//dummy

	while (start != end) {
		RenderStateKey key = start->getKey();
		
		ViewPort current_viewport = key.getViewPort();
		if (prev_viewport != current_viewport) {
			setViewPort(scene, current_viewport);
			prev_viewport = current_viewport;
		}

		ViewPortLayer current_layer = key.getViewPortLayer();
		if (prev_layer != current_layer) {
			setViewPortLayer(scene, current_layer, prev_layer);
			prev_layer = current_layer;
		}

		BlendType current_blend = key.getBlendType();
		if (prev_blend != current_blend) {
			setBlendType(scene, current_blend);
			prev_blend = current_blend;
		}

		MaterialType current_material = key.getMaterialType();
		switch (current_material) {
		case MaterialType::BASIC_MATERIAL_ID:
			start = renderBasic(scene, start, end);
			break;
		case MaterialType::BASIC_LIT_MATERIAL_ID:
			start = renderBasicLit(scene, start, end);
			break;
		case MaterialType::LIGHT_VOLUME_MATERIAL_ID:
			start = renderLightVolumes(scene, start, end);
			break;
		case MaterialType::NORMAL_MATERIAL_ID:
			start = renderNormal(scene, start, end);
			break;
		case MaterialType::DISSOLVE_MATERIAL_ID:
			start = renderDissolve(scene, start, end);
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
}

BasicRenderer::draw_iterator
BasicRenderer::renderBasic(const Scene& scene, draw_iterator start, draw_iterator end) {
	auto shader = shaders.get(BasicMaterial::ShaderID);
	if (!shader) {
		return end;
	}

	shader->start();

	shader->setUniformMat4("VP", projection * scene.mainCamera.transform);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		const Instance& instance = scene.instances[instance_id];
		const Mesh& mesh = instance.mesh;
		const BasicMaterial& material = scene.castMaterial<BasicMaterial>(instance.material);
		const glm::mat4& transform = instance.transform;

		shader->setUniformMat4("M", transform);
		shader->setUniform3f("u_color", material.getColor());
		shader->setUniform1f("u_alpha", material.getAlpha());

		bindOrDefault(scene, material.getTexture(), 0);
		shader->setUniform1i("diffuse", 0);

		mesh.vao.bind();
		mesh.ebo.bind();
		glDrawElements(GL_TRIANGLES, mesh.getNumIndicies(), GL_UNSIGNED_INT, 0);

		++start;
	}

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderBasicLit(const Scene& scene, draw_iterator start, draw_iterator end) {
	auto shader = shaders.get(BasicLitMaterial::ShaderID);
	if (!shader) {
		return end;
	}

	shader->start();
	shader->setUniformMat4("P", projection);
	shader->setUniformMat4("V", scene.mainCamera.transform);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		const Instance& instance = scene.instances[instance_id];
		const Mesh& mesh = instance.mesh;
		const BasicLitMaterial& material = scene.castMaterial<BasicLitMaterial>(instance.material);
		const glm::mat4& transform = instance.transform;
		shader->setUniformMat4("M", transform);

		glm::mat3 inverse = glm::inverse(glm::mat3(transform));
		shader->setUniformMat3("inverse_transpose", inverse, true);

		bindOrDefault(scene, material.texture, 0);
		shader->setUniform1i("diffuse", 0);

		shader->setUniform3f("u_color", material.color);

		mesh.vao.bind();
		mesh.ebo.bind();
		glDrawElements(GL_TRIANGLES, mesh.getNumIndicies(), GL_UNSIGNED_INT, 0);

		start++;
	}
	shader->end();

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderLightVolumes(const Scene& scene, draw_iterator start, draw_iterator end) {
	using namespace entt;

	//We want to render the inside of the volume to handle the case of the 
	//camera being inside the volume radius
	glDisable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);

	auto sphere = scene.meshCache.handle("sphere"_hs);
	assert(sphere);
	sphere->vao.bind();
	sphere->ebo.bind();


	auto shader = shaders.get("LightVolumeShader"_hs);
	if (!shader) {
		return end;
	}
	shader->start();
	shader->setUniformMat4("P", projection);

	//GBuffer is where all of the data is to compute lighting
	const FBO& gBuffer = postProcess.getGBuffer();
	shader->setUniform2f("color_attachment_size.fbo_size", gBuffer.getWidth(), gBuffer.getHeight());
	shader->setUniform2f("color_attachment_size.window_size", gBuffer.getActualSize(glm::vec2(current_width, current_height)));
	
	glm::vec2 size = gBuffer.getActualSize(glm::vec2(current_width, current_height));
	shader->setUniform2f("inv_viewport_size", 1.0f / size);

	shader->setUniform1i("position_attachment", 0);
	shader->setUniform1i("normal_attachment", 1);
	shader->setUniform1i("color_attachment", 2);
	gBuffer.getColorAttachment(0).bindActiveTexture(0);
	gBuffer.getColorAttachment(1).bindActiveTexture(1);
	gBuffer.getColorAttachment(2).bindActiveTexture(2);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto light_id = start->getValue();
		const auto &light = scene.pointLights[light_id];
		
		glm::mat4 V = scene.mainCamera.transform;
		glm::vec4 view_pos = V * glm::vec4(light.position, 1);
		shader->setUniform3f("u_view_light.pos", glm::vec3(view_pos));
		shader->setUniform3f("u_view_light.color", light.color);
		shader->setUniform1f("u_view_light.intensity", light.intensity);
		shader->setUniform1f("u_view_light.radius", light.radius);
		
		glDrawElements(GL_TRIANGLES, sphere->getNumIndicies(), GL_UNSIGNED_INT, 0);
		start++;
	}
	shader->end();

	//Reset opengl state
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderNormal(const Scene& scene, draw_iterator start, draw_iterator end) {

	auto shader = shaders.get(NormalMaterial::ShaderID);
	if (!shader) {
		return end;
	}
	shader->start();
	shader->setUniformMat4("VP", projection * scene.mainCamera.transform);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		const Instance& instance = scene.instances[instance_id];
		const Mesh& mesh = instance.mesh;

		auto &transform = instance.transform;
		glm::mat3 inverse = glm::inverse(glm::mat3(transform));

		shader->setUniformMat3("inverse_transpose", inverse, true);
		shader->setUniformMat4("M", transform);

		mesh.vao.bind();
		mesh.ebo.bind();
		glDrawElements(GL_TRIANGLES, mesh.getNumIndicies(), GL_UNSIGNED_INT, 0);

		start++;
	}
	shader->end();

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderDissolve(const Scene& scene, draw_iterator start, draw_iterator end) {
	
	auto shader = shaders.get(DissolveMaterial::ShaderID);
	if (!shader) {
		return end;
	}

	glDisable(GL_CULL_FACE);

	shader->start();
	shader->setUniformMat4("VP", projection * scene.mainCamera.transform);
	shader->setUniformMat4("V", scene.mainCamera.transform);

	loadPointLights(scene, *shader);

	const RenderStateKey current_state = start->getKey();
	while (start != end && current_state == start->getKey()) {
		auto instance_id = start->getValue();
		const Instance& instance = scene.instances[instance_id];
		const Mesh& mesh = instance.mesh;
		const DissolveMaterial& material = scene.castMaterial<DissolveMaterial>(instance.material);
		const glm::mat4& transform = instance.transform;
		shader->setUniformMat4("M", transform);

		glm::mat3 inverse = glm::inverse(glm::mat3(transform));
		shader->setUniformMat3("inverse_transpose", inverse, true);

		shader->setUniform3f("u_color", material.color);
		shader->setUniform1f("u_dissolve", material.time);
		shader->setUniform1f("u_offset", material.offset);
		shader->setUniform3f("u_dissolve_color", material.dissolve_color);
		
		shader->setUniform1i("diffuse", 0);
		shader->setUniform1i("noise", 1);
		bindOrDefault(scene, material.texture, 0);
		noise3D.bindActiveTexture(1);

		mesh.vao.bind();
		mesh.ebo.bind();
		glDrawElements(GL_TRIANGLES, mesh.getNumIndicies(), GL_UNSIGNED_INT, 0);

		start++;
	}

	glEnable(GL_CULL_FACE);

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderParticle(const Scene &scene, draw_iterator start, draw_iterator end) {
	//auto shader = shaders.get(ParticleMaterial::ShaderID);
	//if (!shader) {
	//	return end;
	//}

	//shader->start();
	//shader->setUniformMat4("P", projection);
	//shader->setUniformMat4("V", scene.mainCamera.transform);

	//const RenderStateKey current_state = start->getKey();
	//while (start != end && current_state == start->getKey()) {
	//	auto instance_id = start->getValue();
	//	auto& particle_system_instances = scene.particleSystemInstances[instance_id];
	//	auto& render_data = particle_system_instances.data;
	//	auto& transform = particle_system_instances.transform;
	//	
	//	shader->setUniformMat4("M", transform);
	//	
	//	render_data.vao.bind();
	//	render_data.ebo.bind();
	//	auto prim_count = render_data.instances;
	//	glDrawElementsInstanced(GL_TRIANGLES, render_data.ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0, prim_count);

	//	start++;
	//}

	return start;
}

BasicRenderer::draw_iterator
BasicRenderer::renderFireParticle(const Scene &scene, draw_iterator start, draw_iterator end) {
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
		const auto& particle_system_instances = scene.particleSystemInstances[instance_id];
		const auto& fire_material = scene.castMaterial<FireParticleMaterial>(particle_system_instances.material);
		
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

void BasicRenderer::bindOrDefault(const Scene& scene, entt::id_type texture_id, int unit) {
	auto texture = scene.texCache.handle(texture_id);
	if (texture) {
		texture->bindActiveTexture(unit);
	}
	else {
		white.bindActiveTexture(unit);
	}
}

void BasicRenderer::loadPointLights(const Scene& scene, GLSLProgram &shader) {

	size_t num_lights = glm::min(scene.pointLights.size(), (size_t)MAX_POINT_LIGHTS);

	shader.setUniform1i("u_num_point_lights", num_lights);
	for (int i = 0; i < num_lights; i++) {
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


int BasicRenderer::getCurrentWidth() const {
	return current_width;
}

int BasicRenderer::getCurrentHeight() const {
	return current_height;
}

const glm::mat4& BasicRenderer::getProjection() const {
	return projection;
}

ShaderSet& BasicRenderer::getShaderSet() {
	return shaders;
}