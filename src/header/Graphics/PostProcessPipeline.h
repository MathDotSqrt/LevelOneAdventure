#pragma once 
#include "Graphics/Scene.h"

#include "Graphics/FBO.h"
#include "Graphics/Mesh.h"
#include "Graphics/ShaderSet.h"
#include <vector>

namespace LOA::Graphics {
	class Scene;
	class BasicRenderer;

	class PostProcessPipeline {
	public:
		PostProcessPipeline(BasicRenderer& renderer, int max_width, int max_height);

		void bindGBuffer(int current_width, int current_height) const;
		void bindPostProcessBuffer(int current_width, int current_height) const;
		void bindFXAA(int current_width, int current_height) const;
		void unbind() const;

		void clearFrameBuffers();

		void renderDeferred(const Scene& scene, BasicRenderer& renderer);
		void renderPostProcess(BasicRenderer& renderer);
		void renderAntiAlias(BasicRenderer& renderer);
		void renderStage(int width, int height, const FBO& fbo, GLSLProgram &shader);

		const FBO& getGBuffer() const;
	private:
		//1) render all deferred geometry into gbuffer with ambient+ssao
		//2) bind final buffer blit depth from gbuffer
		//3) render ambiant/ssao/directional lighting to final from gbuffer
		//4) render light volumes and sample from gbuffer, addativly blend into final
		//5) ... more post processing
		FBO gBuffer;	//Bytes per pixel: 6 + 6 + 4 + 3 
		FBO ssao;		//Bytes per pixel: 2
		FBO blurX;		//Bytes per pixel: 6 / 4
		FBO blurY;		//Bytes per pixel: 6 / 4
		FBO final;		//Bytes per pixel: 6


		FBO fxaa;		//Bytes per pixel: 3

		TEX ssaoNoise;
		std::vector<glm::vec3> ssaoKernel;

		Mesh quad;

	};
}