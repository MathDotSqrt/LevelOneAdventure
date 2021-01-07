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

void PhysicsScene::prerender() {
	world->debugDrawWorld();
	glDrawer->bufferData();
}

void PhysicsScene::setGravity(glm::vec3 g) {
	world->setGravity(btVector3(g.x, g.y, g.z));
}

std::pair<bool,glm::vec3> PhysicsScene::castRay(glm::vec3 start, glm::vec3 stop, bool debug) const {
	btVector3 btStart(start.x, start.y, start.z);
	btVector3 btStop(stop.x, stop.y, stop.z);

	btCollisionWorld::ClosestRayResultCallback callback(btStart, btStop);
	//To only report back collisions with static objects
	callback.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;

	world->rayTest(btStart, btStop, callback);

	if (debug) {
		btVector3 color = callback.hasHit() ? btVector3(1, 0, 0) : btVector3(0, 1, 0);
		getDrawer()->drawLine(btStart, btStop, color);
	}
	glm::vec3 hitspot = glm::vec3(callback.m_hitPointWorld.x(), callback.m_hitPointWorld.y(), callback.m_hitPointWorld.z());
	std::pair<bool, glm::vec3> mypair = { callback.hasHit(),hitspot };
	return mypair;
}

btRigidBody* PhysicsScene::createBox(float mass, glm::vec3 dim, glm::vec3 pos, glm::quat rot) {
	btCollisionShape* shape = new btBoxShape(btVector3(dim.x, dim.y, dim.z));
	btVector3 inertia(0,0,0);
	if(mass != 0)
		shape->calculateLocalInertia(mass, inertia);

	shapes.push_back(shape);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, shape, inertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	groundTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	body->setWorldTransform(groundTransform);
	
	if (mass == 0) {
		body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	}

	world->addRigidBody(body);

	return body;
}

void PhysicsScene::freeBox(btRigidBody* body) {
	delete body->getMotionState();
	world->removeRigidBody(body);
	delete body;

}

btRigidBody* PhysicsScene::createStaticPlane(glm::vec3 normal, float scalar) {
	btCollisionShape* shape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), scalar);
	shapes.push_back(shape);
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, shape);

	btRigidBody* body = new btRigidBody(rbInfo);
	btTransform groundTransform;
	groundTransform.setIdentity();
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