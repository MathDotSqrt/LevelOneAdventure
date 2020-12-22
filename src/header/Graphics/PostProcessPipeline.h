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

		void clearFrameBuffers();

		void renderDeferred(ShaderSet& shaders, int current_width, int current_height);
		void renderPostProcess(ShaderSet &shaders, int current_width, int current_height);
		void renderStage(int width, int height, const FBO& fbo, GLSLProgram &shader);
	private:
		//1) render all geometry into gbuffer
		//2) compute deferred lighting 
		//3) store deferred lighting into final deferred buffer
		//4) render all forward geometry into final deferred buffer
		FBO gBuffer;
		FBO final;

		Mesh quad;

	};
}