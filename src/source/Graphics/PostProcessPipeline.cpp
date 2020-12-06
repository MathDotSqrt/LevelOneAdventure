#include "Graphics/PostProcessPipeline.h"
#include "Graphics/GeometryBuilder.h"
#include "GL/glew.h"
#include "entt/core/hashed_string.hpp"

#include <iostream>

using namespace LOA::Graphics;

PostProcessPipeline::PostProcessPipeline(int width, int height) :
	mainViewPort(width, height),
	blurX(width / 4, height / 4),
	blurY(width / 4, height / 4),
	final(width, height),
	quad(gen_quad2D(2.0f)){

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
	//{
	//	blurX.bind(width, height);
	//	auto& shader = shaders.getShader({ "postprocess/pp.vert", "postprocess/pp_filter.frag" });
	//	shader->start();
	//	renderStage(width, height, mainViewPort, *shader);
	//	shader->end();
	//	blurX.unbind();
	//}

	//{
	//	blurY.bind(width, height);
	//	auto& shader = shaders.getShader({ "postprocess/pp.vert", "postprocess/pp_blurX.frag" });
	//	shader->start();
	//	renderStage(450, 360, blurX, *shader);
	//	shader->end();
	//	blurY.unbind();
	//}

	//{
	//	final.bind(width, height);
	//	auto& shader = shaders.getShader({ "postprocess/pp.vert", "postprocess/pp_blurY.frag" });
	//	shader->start();
	//	renderStage(450, 360, blurY, *shader);
	//	shader->end();
	//	final.unbind();
	//}

	{
		auto& shader = shaders.get("FinalPP"_hs);
		shader->start();
		//shader->setUniform1i("blur", 1);
		//blurY.getColorAttachment().bindActiveTexture(1);
		renderStage(width, height, mainViewPort, *shader);
		//blurY.getColorAttachment().unbind();
		shader->end();
	}
}

void PostProcessPipeline::renderStage(int width, int height, const FBO &fbo, GLSLProgram &shader) {
	glClear(GL_COLOR_BUFFER_BIT);

	shader.setUniform2f("color_attachment_size.fbo_size", fbo.getWidth(), fbo.getHeight());
	shader.setUniform2f("color_attachment_size.window_size", width, height);
	shader.setUniform1i("color_attachment", 0);
	fbo.getColorAttachment().bindActiveTexture(0);

	quad.vao.bind();
	quad.ebo.bind();

	glDrawElements(GL_TRIANGLES, quad.ebo.getNumBytes() / sizeof(GLuint), GL_UNSIGNED_INT, 0);
	fbo.getColorAttachment().unbind();
}