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
		//1) render all deferred geometry into gbuffer with ambient+ssao
		//2) bind final buffer blit depth from gbuffer
		//3) render ambiant/ssao/directional lighting to final from gbuffer
		//4) render light volumes and sample from gbuffer, addativly blend into final
		//5) ... more post processing
		FBO gBuffer;
		FBO final;

		Mesh quad;

	};
}