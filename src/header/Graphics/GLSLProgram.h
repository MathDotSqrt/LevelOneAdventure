#pragma once

#include <unordered_map>
#include <glm/glm.hpp>

#include "common.h"

namespace LOA::Graphics {
	class GLSLProgram {
	public:
		typedef GLuint ProgramID;
		typedef GLuint VertexID;
		typedef GLuint FragmentID;

		GLSLProgram(ProgramID program, VertexID vertex, FragmentID fragment);
		GLSLProgram(GLSLProgram&&);
		GLSLProgram(const GLSLProgram&) = delete;

		GLSLProgram& operator=(const GLSLProgram&) = delete;
		GLSLProgram& operator=(GLSLProgram&&);
		~GLSLProgram();

		void start();
		void end();

		void setUniform1i(const std::string& uniform, int i);
		void setUniform1f(const std::string& uniform, float f);
		void setUniform3f(const std::string& uniform, float x, float y, float z);
		void setUniform3f(const std::string& uniform, float v[3]);
		void setUniform3f(const std::string& uniform, const glm::vec3& vec3);

		void setUniformMat3(const std::string& uniform, const glm::mat3& mat3, bool transpose = false);
		void setUniformMat4(const std::string& uniform, const glm::mat4& mat4, bool transpose = false);
	private:
		friend class ShaderSet;
		GLint getUniformLocation(const std::string& uniform);

		std::unordered_map<std::string, GLint> uniforms;
		ProgramID program;
		VertexID vertex;
		FragmentID fragment;



		void dispose();
		bool isValid();
	};
}