#include "Physics/PhysicsScene.h"

#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "Physics/PhysicsDebugDrawer.h"

using namespace LOA::Physics;



PhysicsScene::PhysicsScene() {
	config = new btDefaultCollisionConfiguration();

	glDrawer = new PhysicsDebugDrawer();

	dispatcher = new btCollisionDispatcher(config);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

	world->setDebugDrawer(glDrawer);

	//TODO: potential leak on ghost pair callback
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

PhysicsScene::~PhysicsScene() {
	for (auto shape : shapes) {
		delete shape;
	}

	delete glDrawer;
	delete world;
	delete solver;
	delete broadphase;
	delete dispatcher;
	delete config;
}

void PhysicsScene::update(float delta) {
	world->stepSimulation(delta);
}

void PhysicsScene::render() {
	world->debugDrawWorld();
	glDrawer->bufferData();
}

void PhysicsScene::setGravity(glm::vec3 g) {
	world->setGravity(btVector3(g.x, g.y, g.z));
}

btRigidBody* PhysicsScene::createBox(glm::vec3 pos, glm::vec3 dim, float mass) {
	btCollisionShape* shape = new btBoxShape(btVector3(dim.x, dim.y, dim.z));
	btVector3 inertia(1, 1, 1);
	if(mass != 0)
		shape->calculateLocalInertia(mass, inertia);

	shapes.push_back(shape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	//groundTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, shape, inertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setWorldTransform(groundTransform);
	world->addRigidBody(body);
	return body;
}

void PhysicsScene::freeBox(btRigidBody* body) {
	world->removeRigidBody(body);
	delete body;
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

btKinematicCharacterController* PhysicsScene::createCharacterController() {
	//Reference: https://github.com/kripken/bullet/blob/master/Demos/CharacterDemo/CharacterDemo.cpp
	
	//TODO: fix leak
	btPairCachingGhostObject* ghost_object = new btPairCachingGhostObject();
	btConvexShape* shape = new btCapsuleShape(.5, 1);
	shapes.push_back(shape);

	btTransform transform;
	transform.setIdentity();

	ghost_object->setWorldTransform(transform);
	ghost_object->setCollisionShape(shape);
	ghost_object->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	
	//TODO: fix leak
	btKinematicCharacterController* controller = new btKinematicCharacterController(ghost_object, shape, .1, btVector3(0, 1, 0));
	
	//No clue what the parameters are 
	//Found in the demo
	world->addCollisionObject(ghost_object, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	world->addAction(controller);

	return controller;
}

void PhysicsScene::freeCharacterController(btKinematicCharacterController* controller) {
	world->removeCollisionObject(controller->getGhostObject());
	world->removeAction(controller);

	//TODO: investigate potential leak of ghost object
	//not sure if bullet deallocates that for me
	delete controller;
}

PhysicsDebugDrawer* PhysicsScene::getDrawer() const { 
	return glDrawer;
}