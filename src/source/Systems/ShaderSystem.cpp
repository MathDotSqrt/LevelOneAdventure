#include "Systems/ShaderSystem.h"
#include "Engine.h"


#include "Util/Timer.h"
#include "Util/FileUtil.h"

const std::string rel = "./res/shaders/";

using namespace LOA::Systems;

void ShaderSystem::init(){
	
}

void ShaderSystem::update(float delta) {
	Util::Timer timer("ShaderSystem");

	auto& renderer = engine.getRenderer();
	auto& shaders = renderer.shaders;
	
	if (files.size() == 0) {
		for (const auto& file : shaders.loadedFiles) {
			files.push(file);
		}
	}
	
	const auto& file = files.front();

	auto last_write = Util::last_write(rel + file);

	if (last_write) {
		i64 new_time = last_write.value();
		if (fileTimestamp.find(file) == fileTimestamp.end()) {
			fileTimestamp[file] = new_time;
		}

		if (new_time > fileTimestamp[file]) {
			shaders.reload(file);
			fileTimestamp[file] = new_time;
		}
	}

	files.pop();
}