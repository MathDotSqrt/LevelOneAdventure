#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>
#include <vector>
#include "common.h"
#include "Util/PackedFreeList.h"

class btRigidBody;
class btKinematicCharacterController;
class btPairCachingGhostObject;

namespace LOA::Systems {
	class RenderSystem;
}

namespace LOA::Component {

	struct Transformation {
		glm::vec3 pos = glm::vec3(0);
		glm::quat rot = glm::quat(1, 0, 0, 0);
		glm::vec3 scale = glm::vec3(1);

		Transformation() = default;
		Transformation(glm::vec3 pos, glm::quat rot=glm::quat(1, 0, 0, 0), glm::vec3 scale=glm::vec3(1)) : pos(pos), rot(rot), scale(scale) {}
	};

	//Velocity inherits vector 3 to make it automatically convertable with vec3 math
	struct Velocity : glm::vec3 {
		Velocity(const glm::vec3& vel) : glm::vec3(vel) {}

		//WTF is this line
		operator const glm::vec3& () const { return static_cast<glm::vec3>(*this); }
	};

	struct Renderable {
		LOA::ID instance_id;
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
		bool basicattack = false;
		bool powerfulattack = false;
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
	enum class AIState {
		IDLE, ATTACK, CHASE, SEARCH
	};
	struct AIComponent {
		entt::entity target;


		float attackrange;
		
		float speed = 1.0f;
		float cooldown = 0.0f;
		
		glm::vec3 lastspot = glm::vec3(10);
		AIState currentstate = AIState::IDLE;
	};

	struct PartyMember {
		entt::entity leader = entt::null;
	};

	struct HealthComponent {
		float max_health = 10;
		float current_health = 10;
	};

	struct ProjectileComponent {
		float damage = 1.0f;
	};

	struct PointLight {
		LOA::ID instance_id;
		glm::vec3 color;
		float intensity;
		float radius = 2.0f;
	};

	struct FireParticle {
		float spawn_rate;
		float life_time = 10.0f;
	};

	struct LevelTile {
		//TODO: dont store an effing string in a component
		std::string model_name;
		glm::ivec3 loc;
		int rot = 0;
	};

	struct LevelBuilder {
		int id = 0;
		int rot = 0;
	};

	//Rigid Bodies that have physics: stackable boxes, more stackable boxes
	struct RigidBody {
		glm::vec3 dim;
		glm::vec3 offset;
		float mass;
		btRigidBody* body;

		RigidBody(glm::vec3 dim, glm::vec3 offset=glm::vec3(0), float mass=1) : 
			dim(dim), 
			offset(offset), 
			mass(mass), 
			body(nullptr) {}
	};

	//Ghost bodies for dealing/taking damage

	enum class EventType : u32 {
		NONE = 0,
		STATIC,
		CHARACTER,
		FIRE_BOLT,
		NUL_EVENTS,
	};
	struct HitBox {
		struct CollisionEvent {
			entt::entity other_entity = entt::null;
			EventType other_event_type = EventType::NONE;
		};

		EventType type = EventType::STATIC;
		glm::vec3 dim = glm::vec3(1);
		glm::vec3 offset = glm::vec3(0);
		btPairCachingGhostObject* ghost = nullptr;

		//Arraylist of events
		//Hitbox system would clear these events every frame
		std::vector<CollisionEvent> events;

		//CollisionEvent event;
	};

	//Rigid Bodies that dont have physics: walls, floors, etc
	struct StaticBody {
		glm::vec3 dim;
		glm::vec3 offset;
		btRigidBody* body;

		//If user creates the body manually
		StaticBody(btRigidBody* body) : 
			dim(glm::vec3(0)), 
			offset(glm::vec3(0)), 
			body(body) {}

		//Automatically creates the static body, 
		//Updates when component transform is updated
		StaticBody(glm::vec3 dim, glm::vec3 offset = glm::vec3(0)) : 
			dim(dim), 
			offset(offset), 
			body(nullptr) {}
	};

	//Rigid Bodies for characters: player, enemies, etc
	struct CharacterController {
		btKinematicCharacterController* kinematicCollider = nullptr;
	};
	/*Classes*/
	struct FireMage {
		/*In the Future might store mana/stamina, cooldowns, ect*/
	};
}