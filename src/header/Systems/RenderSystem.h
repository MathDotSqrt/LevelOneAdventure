#pragma once

#include "Systems/System.h"

namespace LOA::Systems {
	class RenderSystem : public BaseSystem {
	public:
		RenderSystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
	};
}