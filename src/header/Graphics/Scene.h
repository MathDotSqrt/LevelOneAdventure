#pragma once

#include <typeinfo>
#include <tuple>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/ParticleRenderData.h"
#include "Util/PackedFreeList.h"

#include "Graphics/MeshCache.h"
#include "Graphics/TEXCache.h"
#include "entt/entt.hpp"

namespace LOA::Graphics {
	struct PerspectiveCamera {
		float fov;
		float aspect;
		float near;
		float far;

		glm::mat4 transform = glm::identity<glm::mat4>();

		PerspectiveCamera();
		PerspectiveCamera(float fov, float aspect, float near, float far);
	};

	struct PointLight {
		glm::vec3 position;
		glm::vec3 color;
		float intensity;
		float radius = 2.0f;
	};

	struct DirLight {
		glm::vec3 color;
		glm::vec3 dir;
		float intensity;
	};

	struct AmbientLight {
		glm::vec3 color;
		float intensity;
	};

	struct Instance {
		entt::resource_handle<Mesh> mesh;
		std::unique_ptr<BaseMaterial> material;
		glm::mat4 transform = glm::identity<glm::mat4>();
		
		Instance(entt::resource_handle<Mesh> mesh);
		Instance(entt::resource_handle<Mesh> mesh, BaseMaterial* type) : mesh(mesh), material(type){}
	};

	struct ParticleSystemInstance {
		ParticleRenderData data;
		std::unique_ptr<BaseMaterial> material;
		glm::mat4 transform = glm::identity<glm::mat4>();

		ParticleSystemInstance(ParticleRenderData&& data, BaseMaterial* material) : data(std::move(data)), material(material){}
		ParticleSystemInstance(size_t max, BaseMaterial* material) : data(max), material(material) {}

	};

	class Scene {
	public:
		MeshCache meshCache;
		TEXCache texCache;

		Scene();


		entt::resource_handle<Mesh> loadTexturedMesh(entt::id_type meshID, std::string path, glm::vec3 offset = glm::vec3(0), glm::vec3 scale = glm::vec3(1));
		entt::resource_handle<Mesh> loadMesh(entt::id_type meshID, std::string path, glm::vec3 offset = glm::vec3(0), glm::vec3 scale=glm::vec3(1));
		
		template<typename ...T>
		entt::resource_handle<Mesh> loadRawMesh(entt::id_type meshID, const Geometry<T...>& geometry) {
			return meshCache.load<RawMeshLoader>(meshID, geometry);
		}


		entt::resource_handle<TEX> loadTEX(entt::id_type meshID, std::string path);
		entt::resource_handle<TEX> loadTEX(entt::id_type meshID, TEX::Builder settings, std::string path);

		
		ID addInstance(entt::id_type meshID);

		template<typename MATERIAL>
		ID addInstance(entt::id_type meshID, MATERIAL& material) {
			return instances.insert(Instance{meshCache.handle(meshID), new MATERIAL(material)});
		}

		void removeInstance(ID instance);

		template<typename MATERIAL>
		void changeMaterial(ID instanceID, MATERIAL& material) {
			getInstance(instanceID).material = std::make_unique<MATERIAL>(material);
		}

		template<typename MATERIAL>
		inline MATERIAL& castMaterial(const std::unique_ptr<BaseMaterial>& material) {
			return *static_cast<MATERIAL*>(material.get());
		}

		template<typename MATERIAL>
		inline const MATERIAL& castMaterial(const std::unique_ptr<BaseMaterial>& material) const {
			return *static_cast<MATERIAL*>(material.get());
		}

		ID addPointLight();
		ID addPointLight(PointLight light);
		void removePointLight(ID id);

		template<typename MATERIAL>
		ID createParticleInstance(size_t max_particles, MATERIAL& material) {
			return particleSystemInstances.insert(ParticleSystemInstance{max_particles, new MATERIAL(material)});
		}

		Instance& getInstance(ID id);
		PointLight& getPointLight(ID id);
		ParticleSystemInstance& getParticleSystemInstance(ID id);
		
		void setMainCamera(PerspectiveCamera camera);
		PerspectiveCamera& getMainCamera();

		glm::vec3 getCameraPos() const;

		void setAmbientLight(AmbientLight light);
		AmbientLight getAmbientLight() const;

		void setDirLight(DirLight light);
		DirLight getDirLight() const;

		const PerspectiveCamera& getMainCamera() const;

	private:
		friend class BasicRenderer;

		Util::PackedFreeList<Instance> instances;
		Util::PackedFreeList<ParticleSystemInstance> particleSystemInstances;
		Util::PackedFreeList<PointLight> pointLights;

		PerspectiveCamera mainCamera;
		DirLight dirLight;
		AmbientLight ambient;


	};
}