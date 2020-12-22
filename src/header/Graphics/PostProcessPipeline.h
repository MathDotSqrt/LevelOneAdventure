#pragma once 

#include "Graphics/FBO.h"
#include "Graphics/Mesh.h"
#include "Graphics/ShaderSet.h"

namespace LOA::Graphics {
	class PostProcessPipeline {
	public:
		PostProcessPipeline(ShaderSet &shaders, int max_width, int max_height);

		void bindGBuffer(int current_width, int current_height) const;
		void bindMainViewPort(int current_width, int current_height) const;
		void unbind() const;

		void renderPostProcess(ShaderSet &shaders, int current_width, int current_height);
		void renderStage(int width, int height, const FBO& fbo, GLSLProgram &shader);
	private:
		FBO gBuffer;
		FBO mainViewPort;
		FBO blurX;
		FBO blurY;
		FBO final;

		Mesh quad;

	};
}