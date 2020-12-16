#pragma once

#include "Systems/System.h"
#include "common.h"
#include <unordered_map>
#include <queue>


namespace LOA::Systems {
	class ShaderSystem : public BaseSystem {
	public:
		ShaderSystem(LOA::Engine& engine) : BaseSystem(engine) {}
		void init();
		void update(float delta);

	private:
		std::queue<std::string> files;
		std::unordered_map<std::string, i64> fileTimestamp;
	};
}