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

	struct Instance {
		entt::resource_handle<Mesh> mesh;
		MaterialType materialType = MaterialType::NUM_MATERIAL_ID;
		ID materialID = LOA::NullID;

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


		//Adding an instance without a material would only cache the instance
		//the instance wont get rendered
		ID addInstance(entt::id_type meshID);

		template<typename MATERIAL>
		ID addInstance(entt::id_type meshID, MATERIAL material) {
			Instance new_instance{ 
				meshCache.handle(meshID), 
				MATERIAL::Type
			};
			ID id = instances.insert(new_instance);
			
			Instance& instance = instances[id];
			ID materialID = insertMaterial<MATERIAL>(material);
			instance.materialID = materialID;
			
			return id;
		}

		void removeInstance(ID instance);

		template<typename MATERIAL>
		ID changeMaterial(ID instanceID, MATERIAL material){
			auto& instance = instances[instanceID];
			removeMaterial(instance.materialType, instance.materialID);

			instance.materialType = MATERIAL::Type;
			instance.materialID = insertMaterial(material);

			return instance.materialID;
		}

		template<typename MATERIAL>
		MATERIAL& getMaterial(ID id) {
			auto& freeList = getMaterialFreeList<MATERIAL>();
			return freeList[id];
		}


		ID addPointLight();
		ID addPointLight(PointLight light);
		void removePointLight(ID id);


		ID createParticleInstance(size_t max_particles, ParticleMaterial material);
		ID createParticleInstance(size_t max_particles, FireParticleMaterial material);


		Instance& getInstance(ID id);
		PointLight& getPointLight(ID id);
		ParticleSystemInstance& getParticleSystemInstance(ID id);
		
		void setMainCamera(PerspectiveCamera camera);
		PerspectiveCamera& getMainCamera();

	private:
		friend class BasicRenderer;
		PerspectiveCamera mainCamera;

		template<typename MATERIAL>
		Util::PackedFreeList<MATERIAL>& getMaterialFreeList() {
			return materialRegistry.ctx<Util::PackedFreeList<MATERIAL>>();
		}

		template<typename MATERIAL>
		const Util::PackedFreeList<MATERIAL>& getMaterialFreeList() const {
			return materialRegistry.ctx<Util::PackedFreeList<MATERIAL>>();
		}

		template<typename MATERIAL>
		ID insertMaterial(MATERIAL material) {
			auto& freeList = materialRegistry.ctx_or_set<Util::PackedFreeList<MATERIAL>>();
			
			freeListReferenceMap[MATERIAL::Type] = &freeList;
			
			auto iter = blendMap.find(MATERIAL::Type);
			if (iter == blendMap.end()) {
				blendMap[MATERIAL::Type] = MATERIAL::DefaultBlend;
				viewPortLayerMap[MATERIAL::Type] = MATERIAL::DefaultLayer;

			}
			return freeList.insert(material);
		}


		void removeMaterial(MaterialType type, ID id);

		BlendType getMaterialBlendType(MaterialType type) const;
		ViewPortLayer getMaterialViewPortLayer(MaterialType type) const;

		entt::registry materialRegistry;
		std::unordered_map<MaterialType, Util::PackedFreeListInterface*> freeListReferenceMap;
		std::unordered_map<MaterialType, BlendType> blendMap;
		std::unordered_map<MaterialType, ViewPortLayer> viewPortLayerMap;

		Util::PackedFreeList<Instance> instances;
		Util::PackedFreeList<ParticleSystemInstance> particleSystemInstances;
		Util::PackedFreeList<PointLight> pointLights;
	};
}