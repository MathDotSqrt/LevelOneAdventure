#include "Graphics/ShaderSet.h"

#include <optional>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>

using namespace LOA::Graphics;

std::optional<std::string> read_file(const std::string& filename) {
	std::ifstream reader("res/shaders/" + filename);
	if (reader) {
		std::stringstream buffer;
		buffer << reader.rdbuf();
		std::string file = buffer.str();
		return file;
	}
	else {
		std::cerr << "ERROR: Could not find file: ./res/shaders/" << filename << "\n";
		return {};
	}
}


std::string get_program_name(const std::vector<std::string>& shaders) {
	std::stringstream stream;
	stream << "{";

	for (int i = 0; i < shaders.size(); i++) {
		if (i != 0) {
			stream << ",";
		}

		stream << shaders[i];
	}

	stream << "}";

	return stream.str();
}

std::optional<std::string> preprocessor(std::string& source) {
	const std::string INCLUDE_STRING = "#include ";
	const size_t MAX_SHADER_CODE_LEN = 10000;

	if (source.length() > MAX_SHADER_CODE_LEN) {
		return {};
	}

	std::size_t found = source.find(INCLUDE_STRING);

	//if no include statements were found return the unmodified source
	if (found == std::string::npos) {
		return source;
	}

	std::size_t index = found + INCLUDE_STRING.length();

	//include filename will be placed here
	std::string include_file = "";
	while (source[index] != '\n') {
		include_file += source[index];
		index++;
	}

	std::size_t replace_length = INCLUDE_STRING.length() + include_file.length();

	if (include_file.length() == 0 || include_file[0] != '"' || include_file[include_file.length() - 1] != '"') {
		//LOG_ERROR("Include is invalid for %s", include_file.c_str());
		source.replace(found, replace_length, "\n");
	}

	include_file.erase(0, 1);						//erase first "
	include_file.erase(include_file.size() - 1);	//erase last  "

	auto replace_string = read_file(include_file);
	if (replace_string.has_value()) {
		const auto iter = std::find(replace_string->begin(), replace_string->end(), '\0');
		replace_string->erase(iter, replace_string->end());
		replace_string->append("\n");
		source.replace(found, replace_length, replace_string.value());
		source += '\n';
		return preprocessor(source);
	}
	return replace_string;

}

GLuint compile_shader(const std::string& source, GLenum shader_type) {
	const char* src = source.c_str();

	GLuint shaderID = glCreateShader(shader_type);
	glShaderSource(shaderID, 1, &src, NULL);
	glCompileShader(shaderID);

	int compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		//Getting shader compulation Error
		int infologLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, (GLint*)&infologLength);
		char* infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(shaderID, infologLength, NULL, infoLog); // will include terminate char
		std::cerr << "(S) Shader compilation error:\n" << infoLog << "\n";
		free(infoLog);

		glDeleteShader(shaderID);
		return 0;
	}
	std::cout << "(S) OK - Shader ID : [" << shaderID << "]\n";
	return shaderID;
}


GLuint link_program(GLuint vertex, GLuint fragment) {
	GLuint program = glCreateProgram();

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	GLint is_linked = 0;
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
	if (is_linked == GL_FALSE) {
		//todo maybe detach shader?
		GLint info_log_len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
		char* info_log = (char*)malloc(info_log_len);
		glGetProgramInfoLog(program, info_log_len, NULL, info_log); // will include terminate char
		std::cerr << "(P) Program linking error:\n" << info_log << "\n";
		free(info_log);
		glDeleteProgram(program);
		return 0;
	}

	std::cout << "(P) Linked program " << program << "\n";
	return program;
}

GLenum get_shader_type(const std::string& file) {
	const size_t pos = file.find_last_of('.');

	if (pos == std::string::npos) {
		std::cerr << "(S) Invalid filename [" << file << "]\n";
		return 0;
	}

	const std::string ext = file.substr(pos + 1);
	if (ext == "vert") {
		return GL_VERTEX_SHADER;
	}
	else if (ext == "frag") {
		return GL_FRAGMENT_SHADER;
	}
	else {
		std::cerr << "(S) Invalid filetype [" << file << "]\n";
		return 0;
	}
}

GLuint create_shader(const std::string& shader) {
	GLuint shader_id = 0;
	GLenum shader_type = get_shader_type(shader);
	if (shader_type == 0) {
		return 0;
	}

	auto source = read_file(shader);
	if (source) {
		const auto processedSource = preprocessor(source.value());
		if (processedSource) {
			shader_id = compile_shader(processedSource.value(), shader_type);
		}
	}

	return shader_id;
}

std::shared_ptr<GLSLProgram>
create_program(const std::vector<std::string>& shaders) {
	assert(shaders.size() >= 2);

	GLSLProgram::VertexID vertex = create_shader(shaders[0]);
	GLSLProgram::FragmentID fragment = create_shader(shaders[1]);

	if (vertex == 0 || fragment == 0) {
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return nullptr;
	}

	GLSLProgram::ProgramID program = link_program(vertex, fragment);
	if (program == 0) {
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return nullptr;
	}

	return std::make_shared<GLSLProgram>(program, vertex, fragment);
}

std::shared_ptr<GLSLProgram>
ShaderSet::getShader(const std::vector<std::string>& shaders) {
	const auto name = get_program_name(shaders);

	const auto it = shaderSet.find(name);
	if (it == shaderSet.end()) {
		std::cout << "Creating program: " << name << "\n";

		auto program = create_program(shaders);

		if (!program) {
			std::cerr << "Failed to create program: " << name << "\n";
			return nullptr;
		}

		shaderSet[name] = program;
		return program;
	}

	return it->second;
}