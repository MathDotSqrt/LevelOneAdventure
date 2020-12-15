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
	for (auto shape : shapes) {
		delete shape;
	}

	delete world;
	delete solver;
	delete broadphase;
	delete dispatcher;
	delete config;
}

void PhysicsScene::update(float delta) {
	world->stepSimulation(delta);
}

void PhysicsScene::setGravity(glm::vec3 g) {
	world->setGravity(btVector3(g.x, g.y, g.z));
}

btRigidBody* PhysicsScene::createBox(glm::vec3 pos, glm::vec3 dim) {
	btCollisionShape* shape = new btBoxShape(btVector3(dim.x, dim.y, dim.z));
	btVector3 inertia(1, 1, 1);
	shape->calculateLocalInertia(1, inertia);

	shapes.push_back(shape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	btRigidBody::btRigidBodyConstructionInfo rbInfo(1, nullptr, shape, inertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setWorldTransform(groundTransform);
	world->addRigidBody(body);
	return body;
}

btRigidBody* PhysicsScene::createStaticPlane(glm::vec3 normal, float scalar) {
	btCollisionShape* shape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), scalar);
	shapes.push_back(shape);
	
	btTransform groundTransform;
	groundTransform.setIdentity();
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, shape);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setWorldTransform(groundTransform);

	world->addRigidBody(body);
	return body;
}