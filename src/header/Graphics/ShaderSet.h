#pragma once

#include <memory>
#include <unordered_map>

#include "GLSLProgram.h"

#include "entt/resource/loader.hpp"
#include "entt/resource/cache.hpp"


namespace LOA::Graphics {

	class ShaderLoader : public entt::resource_loader<ShaderLoader, GLSLProgram> {
	public:
		std::shared_ptr<GLSLProgram> load(const std::string &vert, const std::string &frag) const;
	};

	class ShaderSet {

	public:
		entt::resource_handle<GLSLProgram>
		get(entt::id_type shaderID);

		entt::resource_handle<GLSLProgram> 
		load(entt::id_type shaderID, std::string vert, std::string frag);

	private:
		using ShaderCache = entt::resource_cache<GLSLProgram>;
		ShaderCache cache;

		std::unordered_map<std::string, std::shared_ptr<GLSLProgram>> shaderSet;

	};
}