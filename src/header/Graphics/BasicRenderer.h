#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Graphics/ShaderSet.h"
#include "Graphics/Scene.h"
#include "Graphics/RenderStateKey.h"
#include "Graphics/PostProcessPipeline.h"

namespace LOA::Systems {
	class ShaderSystem;
}

namespace LOA::Graphics {
	class BasicRenderer {
	public:
		friend class LOA::Systems::ShaderSystem;

		BasicRenderer();
		void render(const Scene &scene);

	private:
		typedef std::vector<RenderStateKeyValue>::const_iterator draw_iterator;

		std::vector<RenderStateKeyValue> drawList;
		ShaderSet shaders;
		glm::mat4 projection = glm::identity<glm::mat4>();

		TEX noise3D;

		PostProcessPipeline postProcess;

		int current_width = 0;
		int current_height = 0;

		void prerender(const Scene &scene);

		void setViewPort(const Scene &scene, ViewPort viewport);
		void setBlendType(const Scene& scene, BlendType blend);

		draw_iterator renderNormal(const Scene &scene, draw_iterator start, draw_iterator end);
		draw_iterator renderBasicLit(const Scene& scene, draw_iterator start, draw_iterator end);
		draw_iterator renderDissolve(const Scene& scene, draw_iterator start, draw_iterator end);

		draw_iterator renderParticle(const Scene& scene, draw_iterator start, draw_iterator end);
		draw_iterator renderFireParticle(const Scene& scene, draw_iterator start, draw_iterator end);

		void renderPostprocess();

		void clearOpenGLState();
		void loadPointLights(const Scene& scene, GLSLProgram &shader);
		glm::mat4 makeTransform(const glm::vec3& t, const glm::quat& r, const glm::vec3& s) const;
	};
}