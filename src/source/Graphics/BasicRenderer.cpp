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



using namespace LOA::Graphics;

Geometry<PositionAttrib, NormalAttrib> loadModel(Assimp::Importer &importer, std::string filename) {
	Geometry<PositionAttrib, NormalAttrib> geometry;

	const aiScene* scene = importer.ReadFile("./res/models/dragon/dragon2.stl", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	assert(scene);

	const auto mesh = scene->mMeshes[0];
	
	const auto num_vectices = mesh->mNumVertices;
	const auto num_indices = mesh->mNumFaces * 3;

	geometry.resizeVerticies(num_vectices);
	geometry.resizeIndices(num_indices);

	for (int i = 0; i < num_vectices; i++) {
		const auto p = mesh->mVertices[i];
		const auto n = mesh->mNormals[i];

		geometry.pushVertex(glm::vec3(p.x, p.y, p.z), glm::vec3(n.x, n.y, n.z));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		const auto face = mesh->mFaces[i];
		const auto indices = face.mIndices;
		geometry.pushTriangle(indices[0], indices[1], indices[2]);
	}
	return geometry;
}

BasicRenderer::BasicRenderer() : 
	tex(TEX::Builder("./res/textures/uv_grid.jpg").mipmapLinear().buildTexture()),
	vbo(VBO::BufferType::ARRAY_BUFFER),
	vbo_uv(VBO::BufferType::ARRAY_BUFFER),
	ebo(VBO::BufferType::ELEMENT_ARRAY_BUFFER) {
	
	Assimp::Importer importer;
	auto geometry = loadModel(importer, "./res/models/dragon/dragon2.stl");

	
	
	vao.bind();
		vbo.bind();
			vao.addVertexAttribPtr<PositionAttrib, NormalAttrib>();
			vbo.bufferData(geometry.getVerticies());
		vbo.unbind();
		ebo.bind();
			ebo.bufferData(geometry.getIndices());
		ebo.unbind();
	vao.unbind();

	projection = glm::perspective<float>(70, 1, .1f, 1000.0f);

	glEnable(GL_DEPTH_TEST);
}

void BasicRenderer::update(float time) {
	rotation = glm::rotate(rotation, .01f, glm::vec3(1, 1, 0));
	position.z = -10 *time;
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
	auto basic_shader = shaders.getShader({"basic_lit/basic_lit.vert", "basic_lit/basic_lit.frag"});
	assert(basic_shader, "SHADER IS NULL");

	basic_shader->start();
	
	basic_shader->setUniformMat4("P", projection);
	basic_shader->setUniformMat4("M", transform);
	basic_shader->setUniform3f("u_light_pos", glm::vec3(0, 0, 0));
	basic_shader->setUniformMat3("inverse_transpose", inverse, true);

	vao.bind();
	ebo.bind();
	glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
	glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
	glDrawElements(GL_TRIANGLES, ebo.getNumBytes()/sizeof(u32), GL_UNSIGNED_INT, (void*)0);
	ebo.unbind();
	vao.unbind();
	basic_shader->end();
}