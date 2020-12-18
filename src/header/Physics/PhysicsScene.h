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

namespace LOA::Physics {
	class PhysicsDebugDrawer;

	class PhysicsScene {
	public:
		PhysicsScene();
		PhysicsScene(const PhysicsScene&) = delete;
		PhysicsScene(PhysicsScene&&) = default;
		~PhysicsScene();
		PhysicsScene& operator=(const PhysicsScene&) = delete;

		void update(float delta);
		void prerender();
		void setGravity(glm::vec3 g);


		btRigidBody* createBox(float mass, glm::vec3 dim, glm::vec3 pos=glm::vec3(0), glm::quat rot=glm::quat(1, 0, 0, 0));
		btRigidBody* createStaticPlane(glm::vec3 normal, float scalar);

		void freeBox(btRigidBody* body);
		void freeCharacterController(btKinematicCharacterController* controller);
	
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