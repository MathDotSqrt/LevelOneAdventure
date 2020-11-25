#pragma once

#include "Graphics/BasicRenderer.h"
#include "Graphics/Scene.h"
#include "Util/PackedFreeList.h"
namespace LOA {
	class PlayState {
	public:
		PlayState();

		void update(float dt);
		void render();
	
	private:
		ID entity;
		Graphics::BasicRenderer renderer;
		Graphics::Scene scene;
	};
}