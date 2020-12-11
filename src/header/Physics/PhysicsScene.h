#pragma once

#include "glm/glm.hpp"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace LOA::Physics {
	class PhysicsScene {
	public:
		PhysicsScene();
		PhysicsScene(const PhysicsScene&) = delete;
		PhysicsScene(PhysicsScene&&) = default;
		~PhysicsScene();
		PhysicsScene& operator=(const PhysicsScene&) = delete;

		void setGravity(glm::vec3 g);
	private:
		btDefaultCollisionConfiguration* config;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* broadphase;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* world;
	};
}