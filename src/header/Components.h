#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>

#include "common.h"
#include "Util/PackedFreeList.h"

class btRigidBody;

namespace LOA::Systems {
	class RenderSystem;
}

namespace LOA::Component {
	struct Transformation {
		glm::vec3 pos = glm::vec3(0);
		glm::quat rot = glm::quat(1, 0, 0, 0);
		glm::vec3 scale = glm::vec3(1);

		Transformation() = default;
		Transformation(glm::vec3 pos) : pos(pos) {}
		Transformation(glm::vec3 pos, glm::quat rot) : pos(pos), rot(rot) {}
	};

	struct Velocity : glm::vec3 {
		Velocity(const glm::vec3& vel) : glm::vec3(vel) {}
		operator const glm::vec3& () const { return static_cast<glm::vec3>(*this); }
	};

	struct Renderable {
		friend class LOA::Systems::RenderSystem;
		
		LOA::ID instance_id;

		Renderable(LOA::ID id) : 
			instance_id(id),
			mesh_id(0),
			type(Graphics::MaterialType::NUM_MATERIAL_ID){}
		
		Renderable(entt::id_type mesh) :
			instance_id(LOA::NullID),
			mesh_id(mesh),
			type(Graphics::MaterialType::NUM_MATERIAL_ID) {}

	private:
		entt::id_type mesh_id;
		Graphics::MaterialType type;

	};

	struct Direction {
		glm::vec3 forward = glm::vec3(0, 0, -1);
		glm::vec3 right = glm::vec3(1, 0, 0);
		glm::vec3 up = glm::vec3(0, 1, 0);
	};

	struct MovementState {
		float forward = 0;
		float strafe = 0;
		float fly = 0;
		bool fire = false;
		glm::vec2 rotate = glm::vec2(0);
	};

	struct Input {
		glm::vec2 lastCursorPos = glm::vec2(0);
		bool lastFire = false;
	};

	struct Camera {
		float fov;
		float aspect;
		float near;
		float far;

		entt::entity player;


		float theta = 0;
		float phi = glm::pi<float>() / 4;
		float distance = 10;
	};
	 
	struct Dissolve {
		float time = 0;
		float offset = 0;
		glm::vec3 dissolve_color = glm::vec3(1);
	};

	struct PointLight {
		LOA::ID instance_id;
		glm::vec3 color;
		float intensity;
	};

	struct FireParticle {
		float spawn_rate;
	};

	struct Collision {
		float mass;
		btRigidBody* body;

		Collision(float mass=1) : mass(mass), body(nullptr){}
	};
}