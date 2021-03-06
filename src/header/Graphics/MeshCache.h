#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <entt/resource/cache.hpp>
#include <entt/resource/loader.hpp>
#include <entt/core/hashed_string.hpp>


#include "Graphics/Mesh.h"
#include "Graphics/Geometry.h"
namespace LOA::Graphics {

	struct TexturedMeshLoader : entt::resource_loader<TexturedMeshLoader, Mesh> {

		Geometry<PositionAttrib, NormalAttrib, TexcoordAttrib> loadModel(std::string filename, glm::vec3 offset, glm::vec3 scale) const {
			static Assimp::Importer importer;
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

				geometry.pushVertex(scale * glm::vec3(p.x, p.y, p.z) + offset, glm::vec3(n.x, n.y, n.z), glm::vec2(t.x, t.y));
			}

			for (int i = 0; i < mesh->mNumFaces; i++) {
				const auto face = mesh->mFaces[i];
				const auto indices = face.mIndices;
				geometry.pushTriangle(indices[0], indices[1], indices[2]);
			}
			return geometry;
		}

		std::shared_ptr<Mesh> load(std::string filename, glm::vec3 offset = glm::vec3(0), glm::vec3 scale = glm::vec3(1)) const {
			return std::make_shared<Mesh>(loadModel(filename, offset, scale));
		}

	};

	struct MeshLoader : entt::resource_loader<MeshLoader, Mesh> {
		Geometry<PositionAttrib, NormalAttrib> loadModel(std::string filename, glm::vec3 offset, glm::vec3 scale) const {
			static Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
			assert(scene);

			const auto mesh = scene->mMeshes[0];

			const auto num_vectices = mesh->mNumVertices;
			const auto num_indices = mesh->mNumFaces * 3;

			Geometry<PositionAttrib, NormalAttrib> geometry;
			geometry.resizeVerticies(num_vectices);
			geometry.resizeIndices(num_indices);

			for (int i = 0; i < num_vectices; i++) {
				const auto p = mesh->mVertices[i];
				const auto n = mesh->mNormals[i];

				geometry.pushVertex(scale * glm::vec3(p.x, p.y, p.z) + offset, glm::vec3(n.x, n.y, n.z));
			}

			for (int i = 0; i < mesh->mNumFaces; i++) {
				const auto face = mesh->mFaces[i];
				const auto indices = face.mIndices;
				geometry.pushTriangle(indices[0], indices[1], indices[2]);
			}
			return geometry;
		}

		std::shared_ptr<Mesh> load(std::string filename, glm::vec3 offset=glm::vec3(0), glm::vec3 scale=glm::vec3(1)) const {
			return std::make_shared<Mesh>(loadModel(filename, offset, scale));
		}

		
	};

	struct RawMeshLoader : entt::resource_loader<RawMeshLoader, Mesh> {
		template<typename ...T>
		std::shared_ptr<Mesh> load(const Geometry<T...>& geometry) const {
			return std::make_shared<Mesh>(geometry);
		}
	};

	using MeshCache = entt::resource_cache<Mesh>;
}