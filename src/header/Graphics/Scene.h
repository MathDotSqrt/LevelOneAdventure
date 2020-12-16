#pragma once

#include <typeinfo>
#include <tuple>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/ParticleRenderData.h"
#include "Util/PackedFreeList.h"

#include "Graphics/MeshCache.h"
#include "Graphics/TEXCache.h"

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
	};

	struct Instance {
		entt::resource_handle<Mesh> mesh;
		MaterialType materialType;
		ID materialID;

		glm::mat4 transform = glm::identity<glm::mat4>();
		
		Instance(entt::resource_handle<Mesh> mesh, MaterialType type, ID matID);
		Instance(entt::resource_handle<Mesh> mesh, MaterialType type);
	};

	struct ParticleSystemInstance {
		ParticleRenderData data;
		MaterialType materialType;
		ID materialID;
		glm::mat4 transform = glm::identity<glm::mat4>();

		ParticleSystemInstance(ParticleRenderData&& data, MaterialType type, ID matID);
		ParticleSystemInstance(size_t max, MaterialType type, ID matID);

	};

	class Scene {
	public:
		MeshCache meshCache;
		TEXCache texCache;

		entt::resource_handle<Mesh> loadMesh(entt::id_type meshID, std::string path, glm::vec3 offset = glm::vec3(0), glm::vec3 scale=glm::vec3(1));
		entt::resource_handle<TEX> loadTEX(entt::id_type meshID, std::string path);
		entt::resource_handle<TEX> loadTEX(entt::id_type meshID, TEX::Builder settings, std::string path);

		ID addInstance(entt::id_type meshID, NormalMaterial material);
		ID addInstance(entt::id_type meshID, BasicLitMaterial material);
		ID addInstance(entt::id_type meshID, DissolveMaterial material);

		ID addPointLight();
		ID addPointLight(PointLight light);

		ID createParticleInstance(size_t max_particles, ParticleMaterial material);
		ID createParticleInstance(size_t max_particles, FireParticleMaterial material);


		Instance& getInstance(ID id);
		PointLight& getPointLight(ID id);
		ParticleSystemInstance& getParticleSystemInstance(ID id);

		DissolveMaterial& getDissolveMaterial(ID id);

		void setMainCamera(PerspectiveCamera camera);
		PerspectiveCamera& getMainCamera();

	private:
		friend class BasicRenderer;
		PerspectiveCamera mainCamera;

		Util::PackedFreeList<Instance> instances;
		Util::PackedFreeList<BasicLitMaterial> basicLitMaterials;
		Util::PackedFreeList<DissolveMaterial> dissolveMaterials;
		Util::PackedFreeList<ParticleMaterial> particleMaterials;
		Util::PackedFreeList<FireParticleMaterial> fireParticleMaterials;
		
		Util::PackedFreeList<ParticleSystemInstance> particleSystemInstances;
		
		Util::PackedFreeList<PointLight> pointLights;
	};
}