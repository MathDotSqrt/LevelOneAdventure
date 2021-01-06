#pragma once

#include "Systems/System.h"

namespace LOA::Systems {
	class MouseCast : public BaseSystem {
	public:
		MouseCast(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
	};
}