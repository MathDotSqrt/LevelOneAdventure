#include "Systems/ShaderSystem.h"
#include "Engine.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

#include "Util/Timer.h"

const std::string rel = "./res/shaders/";

using namespace LOA::Systems;

void ShaderSystem::init(Engine& engine){
	
}

void ShaderSystem::update(Engine& engine, float delta) {
	Util::Timer timer("ShaderSystem");

	auto& renderer = engine.getRenderer();
	auto& shaders = renderer.shaders;
	
	if (files.size() == 0) {
		for (const auto& file : shaders.loadedFiles) {
			files.push(file);
		}
	}
	
	const auto& file = files.front();

	struct stat result;
	if (stat((rel + file).c_str(), &result) == 0){
		auto new_time = result.st_mtime;
			
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