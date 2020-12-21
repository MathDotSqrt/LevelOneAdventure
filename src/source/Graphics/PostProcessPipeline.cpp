#include "Graphics/PostProcessPipeline.h"
#include "Graphics/GeometryBuilder.h"
#include "GL/glew.h"
#include "entt/core/hashed_string.hpp"

#include <iostream>

using namespace LOA::Graphics;

PostProcessPipeline::PostProcessPipeline(ShaderSet &shaders, int width, int height) :
	mainViewPort(width, height),
	blurX(width / 4, height / 4),
	blurY(width / 4, height / 4),
	final(width, height),
	quad(gen_quad2D(2.0f)){
	
	using namespace entt;


	shaders.load("FilterPP"_hs, "postprocess/pp.vert", "postprocess/pp_filter.frag");
	shaders.load("BlurX"_hs, "postprocess/pp.vert", "postprocess/pp_blurX.frag");
	shaders.load("BlurY"_hs, "postprocess/pp.vert", "postprocess/pp_blurY.frag");
	shaders.load("FinalPP"_hs, "postprocess/pp.vert", "postprocess/pp.frag");

	TEX::Builder colorSettings = TEX::Builder().rgb16f().clampToEdge().linear();
	TEX::Builder depthSettings = TEX::Builder().depth24().clampToEdge().linear();

	mainViewPort.addColorAttachment(colorSettings);
	mainViewPort.addDepthAttachment(depthSettings);

	blurX.addColorAttachment(colorSettings);
	blurX.addDepthAttachment(depthSettings);

	blurY.addColorAttachment(colorSettings);
	blurY.addColorAttachment(depthSettings);

	final.addColorAttachment(colorSettings);
	final.addColorAttachment(depthSettings);
}

void PostProcessPipeline::bindMainViewPort(int width, int height) const {
	mainViewPort.bind(width, height);
}

void PostProcessPipeline::unbind() const {
	mainViewPort.unbind();
}

void PostProcessPipeline::renderPostProcess(ShaderSet &shaders, int width, int height) {
	using namespace entt;
	
	mainViewPort.unbind();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	{
		blurX.bind(width, height);
		auto& shader = shaders.get("FilterPP"_hs);
		shader->start();
		renderStage(width, height, mainViewPort, *shader);
		shader->end();
		blurX.unbind();
	}

	{
		blurY.bind(width, height);
		auto& shader = shaders.get("BlurX"_hs);
		shader->start();
		renderStage(width, height, blurX, *shader);
		shader->end();
		blurY.unbind();
	}

	{
		final.bind(width, height);
		auto& shader = shaders.get("BlurY"_hs);
		shader->start();
		renderStage(width, height, blurY, *shader);
		shader->end();
		final.unbind();
	}

	{
		auto& shader = shaders.get("FinalPP"_hs);
		shader->start();
		
		shader->setUniform2f("blur_attachment_size.fbo_size", final.getWidth(), final.getHeight());
		shader->setUniform2f("blur_attachment_size.window_size", final.getActualSize(glm::vec2(width, height)));
		shader->setUniform1i("blur_attachment", 1);
		final.bindAllColorAttachments();
		renderStage(width, height, mainViewPort, *shader);
		shader->end();
	}
}

void PostProcessPipeline::renderStage(int width, int height, const FBO &fbo, GLSLProgram &shader) {
	glClear(GL_COLOR_BUFFER_BIT);

	shader.setUniform2f("color_attachment_size.fbo_size", fbo.getWidth(), fbo.getHeight());
	shader.setUniform2f("color_attachment_size.window_size", fbo.getActualSize(glm::vec2(width, height)));
	shader.setUniform1i("color_attachment", 0);
	fbo.bindAllColorAttachments();

	quad.vao.bind();
	quad.ebo.bind();

	glDrawElements(GL_TRIANGLES, quad.ebo.getNumBytes() / sizeof(GLuint), GL_UNSIGNED_INT, 0);
}