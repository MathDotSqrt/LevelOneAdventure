#pragma once

#include "Systems/System.h"
#include "common.h"
#include <unordered_map>


namespace LOA::Systems {
	class ShaderSystem : public BaseSystem {
	public:
		void init(Engine &engine);
		void update(Engine &engine, float delta);

	private:
		std::unordered_map<std::string, i64> fileTimestamp;
	};
}