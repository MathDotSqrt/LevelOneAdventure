#pragma once

#include "Graphics/BasicRenderer.h"
#include "Graphics/Scene.h"
#include "Util/PackedFreeList.h"
#include "Engine.h"
#include "Graphics/ParticleGenerator.h"

namespace LOA {
	class PlayState {
	public:
		PlayState();

		void update(float dt);
		void render();
	
	private:
		Engine engine;

	};
}