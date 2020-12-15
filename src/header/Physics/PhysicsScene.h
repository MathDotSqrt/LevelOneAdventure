#pragma once

#include <vector>
#include "glm/glm.hpp"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btCollisionShape;

namespace LOA::Physics {
	class PhysicsScene {
	public:
		PhysicsScene();
		PhysicsScene(const PhysicsScene&) = delete;
		PhysicsScene(PhysicsScene&&) = default;
		~PhysicsScene();
		PhysicsScene& operator=(const PhysicsScene&) = delete;

		void update(float delta);
		void setGravity(glm::vec3 g);

		btRigidBody* createBox(glm::vec3 pos, glm::vec3 dimensions, float mass=1);
		btRigidBody* createStaticPlane(glm::vec3 normal, float scalar);
	private:
		btDefaultCollisionConfiguration* config;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* broadphase;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* world;

		std::vector<btCollisionShape*> shapes;
	};
}