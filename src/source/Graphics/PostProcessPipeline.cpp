#include "Graphics/PostProcessPipeline.h"
#include "Graphics/Scene.h"
#include "Graphics/GeometryBuilder.h"
#include "GL/glew.h"
#include "entt/core/hashed_string.hpp"

#include <iostream>

using namespace LOA::Graphics;

constexpr static int G_POSITION_ATTACHMENT_INDEX = 0;
constexpr static int G_NORMAL_ATTACHMENT_INDEX = 1;
constexpr static int G_COLOR_ATTACHMENT_INDEX = 2;
constexpr static int G_DEPTH_ATTACHMENT_INDEX = 3;

PostProcessPipeline::PostProcessPipeline(ShaderSet &shaders, int width, int height) :
	gBuffer(width, height),
	//mainViewPort(width, height),
	//blurX(width / 4, height / 4),
	//blurY(width / 4, height / 4),
	final(width, height),
	quad(gen_quad2D(2.0f)){
	
	using namespace entt;


	shaders.load("FilterPP"_hs, "postprocess/pp.vert", "postprocess/pp_filter.frag");
	shaders.load("BlurX"_hs, "postprocess/pp.vert", "postprocess/pp_blurX.frag");
	shaders.load("BlurY"_hs, "postprocess/pp.vert", "postprocess/pp_blurY.frag");
	shaders.load("FinalPP"_hs, "postprocess/pp.vert", "postprocess/pp.frag");
	
	
	shaders.load("DeferredDebugPP"_hs, "postprocess/pp.vert", "postprocess/pp_debug_deferred.frag");
	shaders.load("DeferredAmbient"_hs, "postprocess/pp.vert", "postprocess/pp_deferred_ambient.frag");

	TEX::Builder floatSettings = TEX::Builder().rgb16f().clampToEdge().linear();
	TEX::Builder hdrColorSettings = TEX::Builder().rgb16f().clampToEdge().linear();
	TEX::Builder depthSettings = TEX::Builder().depth24().clampToEdge().linear();
	
	gBuffer.addColorAttachment(floatSettings); //Position
	gBuffer.addColorAttachment(floatSettings); //Normal
	gBuffer.addColorAttachment(floatSettings); //Color+Specular
	gBuffer.addDepthAttachment(depthSettings); //Depth

	//mainViewPort.addColorAttachment(hdrColorSettings);
	//mainViewPort.addDepthAttachment(depthSettings);

	//blurX.addColorAttachment(hdrColorSettings);
	//blurX.addDepthAttachment(depthSettings);

	//blurY.addColorAttachment(hdrColorSettings);
	//blurY.addColorAttachment(depthSettings);

	final.addColorAttachment(hdrColorSettings);
	final.addDepthAttachment(depthSettings);
}

void PostProcessPipeline::bindGBuffer(int width, int height) const { 
	gBuffer.bind(width, height);
}

void PostProcessPipeline::bindMainViewPort(int width, int height) const {
	final.bind(width, height);
}

void PostProcessPipeline::unbind() const {
	final.unbind();
}

void PostProcessPipeline::clearFrameBuffers() {
	gBuffer.bind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	final.bind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT);

	final.unbind();
}

void PostProcessPipeline::renderDeferred(const Scene& scene, ShaderSet& shaders, int width, int height) {
	using namespace entt;

	quad.vao.bind();
	quad.ebo.bind();

	AmbientLight ambient = scene.getAmbientLight();
	DirLight dirLight = scene.getDirLight();

	{
		final.bind(width, height);
		glDisable(GL_DEPTH_TEST);

		auto& shader = shaders.get("DeferredAmbient"_hs);
		shader->start();
		
		shader->setUniform3f("u_dir_light.color", dirLight.color);
		shader->setUniform3f("u_dir_light.dir", dirLight.dir);
		shader->setUniform1f("u_dir_light.intensity", dirLight.intensity);

		shader->setUniform3f("u_ambient_light.color", ambient.color);
		shader->setUniform1f("u_ambient_light.intensity", ambient.intensity);

		shader->setUniform2f("color_attachment_size.fbo_size", gBuffer.getWidth(), gBuffer.getHeight());
		shader->setUniform2f("color_attachment_size.window_size", gBuffer.getActualSize(glm::vec2(width, height)));
		
		shader->setUniform1i("normal_attachment", 0);
		shader->setUniform1i("color_attachment", 1);
		gBuffer.getColorAttachment(G_NORMAL_ATTACHMENT_INDEX).bindActiveTexture(0);
		gBuffer.getColorAttachment(G_COLOR_ATTACHMENT_INDEX).bindActiveTexture(1);

		glDrawElements(GL_TRIANGLES, quad.ebo.getNumBytes() / sizeof(GLuint), GL_UNSIGNED_INT, 0);

		shader->end();


		glEnable(GL_DEPTH_TEST);
		final.blitDepthbuffer(gBuffer, width, height);
		final.unbind();
	}
}

void PostProcessPipeline::renderPostProcess(ShaderSet &shaders, int width, int height) {
	using namespace entt;
	
	final.unbind();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//{
	//	blurX.bind(width, height);
	//	auto& shader = shaders.get("FilterPP"_hs);
	//	shader->start();
	//	renderStage(width, height, mainViewPort, *shader);
	//	shader->end();
	//	blurX.unbind();
	//}

	//{
	//	blurY.bind(width, height);
	//	auto& shader = shaders.get("BlurX"_hs);
	//	shader->start();
	//	renderStage(width, height, blurX, *shader);
	//	shader->end();
	//	blurY.unbind();
	//}

	//{
	//	final.bind(width, height);
	//	auto& shader = shaders.get("BlurY"_hs);
	//	shader->start();
	//	renderStage(width, height, blurY, *shader);
	//	shader->end();
	//	final.unbind();
	//}

	//{
	//	auto& shader = shaders.get("FinalPP"_hs);
	//	shader->start();
	//	
	//	shader->setUniform2f("blur_attachment_size.fbo_size", final.getWidth(), final.getHeight());
	//	shader->setUniform2f("blur_attachment_size.window_size", final.getActualSize(glm::vec2(width, height)));
	//	shader->setUniform1i("blur_attachment", 1);
	//	final.bindAllColorAttachments();
	//	renderStage(width, height, gBuffer, *shader);
	//	shader->end();
	//}

	{
		auto& shader = shaders.get("FinalPP"_hs);
		shader->start();
		//shader->setUniform2f("blur_attachment_size.fbo_size", final.getWidth(), final.getHeight());
		//shader->setUniform2f("blur_attachment_size.window_size", final.getActualSize(glm::vec2(width, height)));
		//shader->setUniform1i("blur_attachment", 1);
		//final.bindAllColorAttachments();
		renderStage(width, height, final, *shader);
		shader->end();
	}
}

void PostProcessPipeline::renderStage(int width, int height, const FBO &fbo, GLSLProgram &shader) {
	shader.setUniform2f("color_attachment_size.fbo_size", fbo.getWidth(), fbo.getHeight());
	shader.setUniform2f("color_attachment_size.window_size", fbo.getActualSize(glm::vec2(width, height)));
	shader.setUniform1i("color_attachment", 0);
	fbo.bindAllColorAttachments();

	quad.vao.bind();
	quad.ebo.bind();

	glDrawElements(GL_TRIANGLES, quad.ebo.getNumBytes() / sizeof(GLuint), GL_UNSIGNED_INT, 0);
}