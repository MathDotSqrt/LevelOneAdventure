#include "Physics/PhysicsScene.h"

#include "BulletDynamics/btBulletDynamicsCommon.h"

using namespace LOA::Physics;



PhysicsScene::PhysicsScene() {
	config = new btDefaultCollisionConfiguration();

	dispatcher = new btCollisionDispatcher(config);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();

	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);
	world->stepSimulation(1 / 60.0f, 10);
	
}

PhysicsScene::~PhysicsScene() {
	delete world;
	delete solver;
	delete broadphase;
	delete dispatcher;
	delete config;
}

void PhysicsScene::setGravity(glm::vec3 g) {
	world->setGravity(btVector3(g.x, g.y, g.z));
}