#include "Graphics/PostProcessPipeline.h"
#include "Graphics/Scene.h"
#include "Graphics/GeometryBuilder.h"
#include "Graphics/preamble.h"
#include "Graphics/BasicRenderer.h"

#include "GL/glew.h"
#include "entt/core/hashed_string.hpp"
#include "Util/Noise.h"

#include <iostream>

using namespace LOA::Graphics;

constexpr static int G_POSITION_ATTACHMENT_INDEX = 0;
constexpr static int G_NORMAL_ATTACHMENT_INDEX = 1;
constexpr static int G_COLOR_ATTACHMENT_INDEX = 2;
constexpr static int G_DEPTH_ATTACHMENT_INDEX = 3;

PostProcessPipeline::PostProcessPipeline(BasicRenderer &renderer, int width, int height) :
	gBuffer(width, height),
	//mainViewPort(width, height),
	//blurX(width / 4, height / 4),
	//blurY(width / 4, height / 4),
	ssao(width, height),
	final(width, height),
	ssaoNoise(TEX::Builder().floatType().rgb16f().nearest().repeat().buildTexture(4, 4, Util::gen_ssao_rotation(4))),
	ssaoKernel(Util::gen_ssao_kernel(SSAO_KERNEL_SIZE)),
	quad(gen_quad2D(2.0f)){
	
	using namespace entt;

	ShaderSet& shaders = renderer.getShaderSet();
	shaders.load("FXAA"_hs, "postprocess/pp.vert", "postprocess/pp_fxaa.frag");


	shaders.load("FilterPP"_hs, "postprocess/pp.vert", "postprocess/pp_filter.frag");
	shaders.load("BlurX"_hs, "postprocess/pp.vert", "postprocess/pp_blurX.frag");
	shaders.load("BlurY"_hs, "postprocess/pp.vert", "postprocess/pp_blurY.frag");
	
	
	shaders.load("DeferredDebugPP"_hs, "postprocess/pp.vert", "postprocess/pp_debug_deferred.frag");
	shaders.load("DeferredAmbient"_hs, "postprocess/pp.vert", "postprocess/pp_deferred_ambient.frag");
	shaders.load("SSAO"_hs, "postprocess/pp.vert", "postprocess/pp_ssao.frag");
	shaders.load("FinalPP"_hs, "postprocess/pp.vert", "postprocess/pp.frag");

	gBuffer.addColorAttachment(TEX::Builder().rgb16f().clampToEdge().linear());		//Position
	gBuffer.addColorAttachment(TEX::Builder().rgb16f().clampToEdge().linear());		//Normal
	gBuffer.addColorAttachment(TEX::Builder().rgba().clampToEdge().linear());		//Color+Specular
	gBuffer.addDepthAttachment(TEX::Builder().depth24().clampToEdge().linear());	//Depth

	ssao.addColorAttachment(TEX::Builder().r16f().clampToEdge().nearest());			//SSAO

	//mainViewPort.addColorAttachment(hdrColorSettings);
	//mainViewPort.addDepthAttachment(depthSettings);

	//blurX.addColorAttachment(hdrColorSettings);
	//blurX.addDepthAttachment(depthSettings);

	//blurY.addColorAttachment(hdrColorSettings);
	//blurY.addColorAttachment(depthSettings);

	final.addColorAttachment(TEX::Builder().rgb16f().clampToEdge().linear());		//HDR final color
	final.addDepthAttachmentReference(gBuffer.getDepthAttachement());				//Depth reference
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

void PostProcessPipeline::renderDeferred(const Scene& scene, BasicRenderer& renderer) {
	using namespace entt;
	int width = renderer.getCurrentWidth();
	int height = renderer.getCurrentHeight();
	auto& shaders = renderer.getShaderSet();
	auto& projection = renderer.getProjection();
	
	glDisable(GL_DEPTH_TEST);
	quad.vao.bind();
	quad.ebo.bind();

	//Deferred SSAO Render
	{
		ssao.bind(width, height);
		auto& shader = shaders.get("SSAO"_hs);
		shader->start();

		shader->setUniform2f("color_attachment_size.fbo_size", gBuffer.getWidth(), gBuffer.getHeight());
		shader->setUniform2f("color_attachment_size.window_size", gBuffer.getActualSize(glm::vec2(width, height)));

		shader->setUniform1i("position_attachment", 0);
		shader->setUniform1i("normal_attachment", 1);
		shader->setUniform1i("ssao_noise", 2);
		gBuffer.getColorAttachment(G_POSITION_ATTACHMENT_INDEX).bindActiveTexture(0);
		gBuffer.getColorAttachment(G_NORMAL_ATTACHMENT_INDEX).bindActiveTexture(1);
		ssaoNoise.bindActiveTexture(2);

		shader->setUniform3fv("samples", ssaoKernel);
		shader->setUniformMat4("P", projection);
		shader->setUniform2f("view_port", width, height);
		
		glDrawElements(GL_TRIANGLES, quad.ebo.getNumBytes() / sizeof(GLuint), GL_UNSIGNED_INT, 0);

		shader->end();
	}

	//Deferred Ambient Render
	{
		AmbientLight ambient = scene.getAmbientLight();
		DirLight dirLight = scene.getDirLight();

		final.bind(width, height);

		auto& shader = shaders.get("DeferredAmbient"_hs);
		shader->start();
		
		glm::mat3 V = glm::mat3(scene.getMainCamera().transform);
		shader->setUniform3f("u_view_dir_light.color", dirLight.color);
		shader->setUniform3f("u_view_dir_light.dir", V * dirLight.dir);
		shader->setUniform1f("u_view_dir_light.intensity", dirLight.intensity);

		shader->setUniform3f("u_ambient_light.color", ambient.color);
		shader->setUniform1f("u_ambient_light.intensity", ambient.intensity);

		shader->setUniform2f("color_attachment_size.fbo_size", gBuffer.getWidth(), gBuffer.getHeight());
		shader->setUniform2f("color_attachment_size.window_size", gBuffer.getActualSize(glm::vec2(width, height)));
		
		shader->setUniform1i("normal_attachment", 0);
		shader->setUniform1i("color_attachment", 1);
		shader->setUniform1i("ssao_attachment", 2);
		gBuffer.getColorAttachment(G_NORMAL_ATTACHMENT_INDEX).bindActiveTexture(0);
		gBuffer.getColorAttachment(G_COLOR_ATTACHMENT_INDEX).bindActiveTexture(1);
		ssao.getColorAttachment(0).bindActiveTexture(2);

		glDrawElements(GL_TRIANGLES, quad.ebo.getNumBytes() / sizeof(GLuint), GL_UNSIGNED_INT, 0);

		shader->end();


		//final.blitDepthbuffer(gBuffer, width, height);
		final.unbind();
	}

	glEnable(GL_DEPTH_TEST);

}

void PostProcessPipeline::renderPostProcess(BasicRenderer &renderer) {
	using namespace entt;
	
	int width = renderer.getCurrentWidth();
	int height = renderer.getCurrentHeight();
	ShaderSet& shaders = renderer.getShaderSet();

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

const FBO& PostProcessPipeline::getGBuffer() const {
	return gBuffer;
}