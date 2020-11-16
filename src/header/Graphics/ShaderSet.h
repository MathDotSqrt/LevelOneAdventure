#pragma once

#include <memory>
#include <unordered_map>

#include "GLSLProgram.h"

namespace LOA::Graphics {
	class ShaderSet {

	public:
		std::shared_ptr<GLSLProgram>
		getShader(const std::vector<std::string>& shaders);

	private:
		std::unordered_map<std::string, std::shared_ptr<GLSLProgram>> shaderSet;

	};
}