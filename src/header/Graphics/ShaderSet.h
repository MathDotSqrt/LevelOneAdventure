#pragma once

#include <memory>
#include <unordered_map>
#include <set>

#include "GLSLProgram.h"

#include "entt/resource/loader.hpp"
#include "entt/resource/cache.hpp"


namespace LOA::Systems {
	class ShaderSystem;
}

namespace LOA::Graphics {

	class ShaderLoader : public entt::resource_loader<ShaderLoader, GLSLProgram> {
	public:
		std::shared_ptr<GLSLProgram> load(const std::string& vert, const std::string& frag) const;
		std::shared_ptr<GLSLProgram> load(std::shared_ptr<GLSLProgram> program) const;
	};

	class ShaderSet {

	public:
		friend class LOA::Systems::ShaderSystem;

		entt::resource_handle<GLSLProgram>
		get(entt::id_type shaderID);

		entt::resource_handle<GLSLProgram> 
		load(entt::id_type shaderID, std::string vert, std::string frag);

		void reload(std::string shaderFile);
	private:
		using ShaderCache = entt::resource_cache<GLSLProgram>;
		ShaderCache cache;

		std::set<std::string> loadedFiles;
		std::unordered_map<entt::id_type, std::pair<std::string, std::string>> loadedShaders;
	};
}