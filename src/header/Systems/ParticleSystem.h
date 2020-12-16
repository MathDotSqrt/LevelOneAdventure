#pragma once

#include "Systems/System.h"

#include "Graphics/ParticleGenerator.h"
#include "Util/PackedFreeList.h"
namespace LOA::Systems {
	class ParticleSystem : public BaseSystem {
	public:
		ParticleSystem(LOA::Engine &engine);

		void init();
		void update(float delta);
	private:

		LOA::ID fireInstanceID;
		Graphics::ParticleGenerator fireGenerator;
	};
}