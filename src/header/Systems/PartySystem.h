#pragma once
#include "Systems/System.h"

namespace LOA::Systems {
	class PartySystem : public BaseSystem {
	public:
		PartySystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);
	};
}