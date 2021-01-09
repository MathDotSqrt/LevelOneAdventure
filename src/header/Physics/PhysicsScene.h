#pragma once

#include <vector>
#include "glm/glm.hpp"
#include <glm/gtx/quaternion.hpp>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btCollisionShape;
class btKinematicCharacterController;
class btPairCachingGhostObject;

namespace LOA::Physics {
	class PhysicsDebugDrawer;

	enum class CollisionFlag {
		STATIC = 1,
		KINEMATIC = 2,
		DYNAMIC = 4,
		CHARACTER = 8,
		ALL = -1
	};

	class PhysicsScene {
	public:
		PhysicsScene();
		PhysicsScene(const PhysicsScene&) = delete;
		PhysicsScene(PhysicsScene&&) = default;
		~PhysicsScene();
		PhysicsScene& operator=(const PhysicsScene&) = delete;
		
		
		void prerender();
		void setGravity(glm::vec3 g);
		void update(float delta);
		void checkForContacts(btPairCachingGhostObject* ghost);
		
		std::pair<bool,glm::vec3> castRay(glm::vec3 start, glm::vec3 stop, bool debug=false) const;
		
		btRigidBody* createBox(float mass, glm::vec3 dim, glm::vec3 pos = glm::vec3(0), glm::quat rot = glm::quat(1, 0, 0, 0));
		btRigidBody* createStaticBox(float mass, glm::vec3 dim, glm::vec3 pos=glm::vec3(0), glm::quat rot=glm::quat(1, 0, 0, 0));
		btRigidBody* createStaticPlane(glm::vec3 normal, float scalar);
		btPairCachingGhostObject* createHitBox(glm::vec3 dim, glm::vec3 pos);

		void freeBox(btRigidBody* body);
		void freeCharacterController(btKinematicCharacterController* controller);
		void freeHitBox(btPairCachingGhostObject* ghost);

		btKinematicCharacterController* createCharacterController();

		PhysicsDebugDrawer* getDrawer() const;
	private:

		PhysicsDebugDrawer* glDrawer;

		btDefaultCollisionConfiguration* config;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* broadphase;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* world;

		std::vector<btCollisionShape*> shapes;
	};
}