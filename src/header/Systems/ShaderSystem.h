#pragma once

#include "Systems/System.h"
#include "common.h"
#include <unordered_map>
#include <queue>


namespace LOA::Systems {
	class ShaderSystem : public BaseSystem {
	public:
		void init(Engine &engine);
		void update(Engine &engine, float delta);

	private:
		std::queue<std::string> files;
		std::unordered_map<std::string, i64> fileTimestamp;
	};
}