#pragma once

#include <typeinfo>
#include <tuple>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Util/PackedFreeList.h"

#include "Graphics/MeshCache.h"
#include "Graphics/TEXCache.h"

namespace LOA::Graphics {
	struct PerspectiveCamera {
		glm::vec3 eye;
		glm::vec3 target;
		glm::vec3 up;

		float fov;
		float aspect;
		float near;
		float far;

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

		glm::vec3 pos = glm::vec3(0);
		glm::quat rot = glm::quat(1, 0, 0, 0);
		glm::vec3 scale = glm::vec3(1);
		
		Instance(entt::resource_handle<Mesh> mesh, MaterialType type, ID matID);
		Instance(entt::resource_handle<Mesh> mesh, MaterialType type);
	};

	class Scene {
	public:
		MeshCache meshCache;
		TEXCache texCache;

		entt::resource_handle<Mesh> loadMesh(entt::id_type meshID, std::string path);
		entt::resource_handle<TEX> loadTEX(entt::id_type meshID, std::string path);
		entt::resource_handle<TEX> loadTEX(entt::id_type meshID, TEX::Builder settings, std::string path);

		ID addInstance(entt::id_type meshID, ColorMaterial material);
		ID addInstance(entt::id_type meshID, NormalMaterial material);
		ID addInstance(entt::id_type meshID, BasicLitMaterial material);

		ID addPointLight(PointLight light);

		Instance& getInstance(ID id);

		void setMainCamera(PerspectiveCamera camera);




	private:
		friend class BasicRenderer;
		PerspectiveCamera mainCamera;

		Util::PackedFreeList<Instance> instances;
		Util::PackedFreeList<ColorMaterial> colorMaterials;
		Util::PackedFreeList<BasicLitMaterial> basicLitMaterials;
		
		Util::PackedFreeList<PointLight> pointLights;
	};
}