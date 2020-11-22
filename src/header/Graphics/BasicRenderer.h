#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>

#include "Graphics/Mesh.h"
#include "Graphics/ShaderSet.h"
#include "Util/PackedFreeList.h"



namespace LOA::Graphics {
	class BasicRenderer {
	public:
		BasicRenderer();
		void update(float dt);
		void render(float time);

	private:
		Mesh loadMesh(std::string, std::string);

		Util::PackedFreeList<Mesh> meshes;
		Assimp::Importer importer;


		ShaderSet shaders;

		glm::vec3 position = glm::vec3();
		glm::quat rotation = glm::quat(0, 0, 0, 1);
		glm::vec3 scale = glm::vec3(1);
		glm::mat4 projection;
	};
}