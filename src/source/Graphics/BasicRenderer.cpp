#include "Graphics/BasicRenderer.h"
#include <iostream>
#include <vector>
#include <assert.h>

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "common.h"
#include "Graphics/Attrib.h"
#include "Graphics/Geometry.h"
#include "Util/PackedFreeList.h"



using namespace LOA::Graphics;

Geometry<PositionAttrib, NormalAttrib, TexcoordAttrib> loadModel(Assimp::Importer &importer, std::string filename) {

	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
	assert(scene);

	const auto mesh = scene->mMeshes[0];
	
	const auto num_vectices = mesh->mNumVertices;
	const auto num_indices = mesh->mNumFaces * 3;

	Geometry<PositionAttrib, NormalAttrib, TexcoordAttrib> geometry;
	geometry.resizeVerticies(num_vectices);
	geometry.resizeIndices(num_indices);

	for (int i = 0; i < num_vectices; i++) {
		const auto p = mesh->mVertices[i];
		const auto n = mesh->mNormals[i];
		const auto t = mesh->mTextureCoords[0][i];

		geometry.pushVertex(glm::vec3(p.x, p.y, p.z), glm::vec3(n.x, n.y, n.z), glm::vec2(t.x, t.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		const auto face = mesh->mFaces[i];
		const auto indices = face.mIndices;
		geometry.pushTriangle(indices[0], indices[1], indices[2]);
	}
	return geometry;
}

Mesh BasicRenderer::loadMesh(std::string model_name, std::string tex_name) {
	const auto geometry = loadModel(importer, model_name);
	
	return Mesh(geometry, TEX::Builder(tex_name).buildTexture());
}

BasicRenderer::BasicRenderer() {
	projection = glm::perspective<float>(70, 1, .1f, 1000.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	meshes.emplace_back(loadMesh("./res/models/demo_girl/chr_knight.xobj", "./res/models/demo_girl/chr_knight.png"));
	meshes.emplace_back(loadMesh("./res/models/demo_room/room.xobj", "./res/models/demo_room/room.png"));

	Util::PackedFreeList<Mesh> packedList;
	const auto id = packedList.insert(loadMesh("./res/models/demo_girl/chr_knight.xobj", "./res/models/demo_girl/chr_knight.png"));



	for (auto& test : packedList) {
		const auto& test = packedList[id];
		std::cout << "test\n";
	}
	glEnable(GL_MULTISAMPLE);


}

void BasicRenderer::update(float time) {
	rotation = glm::angleAxis(sin(time) / 4 + glm::pi<float>() / 4, glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, .2f, glm::vec3(1, 0, 0));
	position.z = -time - 10;
	position.y = -2;
}

void BasicRenderer::render(float time) {
	glm::mat4 transform = glm::identity<glm::mat4>();
	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, glm::angle(rotation), glm::axis(rotation));
	transform = glm::scale(transform, scale);
	
	glm::mat3 inverse = glm::inverse(glm::mat3(transform));

	//glClearColor(sin(time), cos(time), 1 - sin(time), 1);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//auto basic_shader = shaders.getShader({ "normal/normal.vert", "normal/normal.frag" });
	auto basic_shader = shaders.getShader({"room_diffuse/room.vert", "room_diffuse/room.frag"});
	assert(basic_shader, "SHADER IS NULL");

	basic_shader->start();
	
	basic_shader->setUniformMat4("P", projection);
	basic_shader->setUniformMat4("M", transform);
	basic_shader->setUniform3f("u_light_pos", glm::vec3(-.1, .5, -3 * time));
	basic_shader->setUniformMat3("inverse_transpose", inverse, true);
	basic_shader->setUniform1i("diffuse", 0);

	for (auto& mesh : meshes) {
		mesh.tex.bindActiveTexture(0);

		mesh.vao.bind();
		mesh.ebo.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glEnableVertexAttribArray(TEXCOORD_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh.ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, (void*)0);
		mesh.ebo.unbind();
		mesh.vao.unbind();

		mesh.tex.unbind();
	}

	
	basic_shader->end();
}