#include "Physics/PhysicsScene.h"

#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "Physics/PhysicsDebugDrawer.h"

using namespace LOA::Physics;

//btVector3 to_bt(const glm::vec3& vec) {
//	return btVector3(vec.x, vec.y, vec.z);
//}
//
//btQuaternion to_bt(const glm::quat& quat) {
//	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
//}
//
//glm::vec3 to_glm(const btVector3& vec) {
//	return glm::vec3(vec.x(), vec.y(), vec.z());
//}
//
//glm::quat to_glm(const btQuaternion& quat) {
//	return glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
//}

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

void PhysicsScene::prerender() {
	world->debugDrawWorld();
	glDrawer->bufferData();
}

void PhysicsScene::setGravity(glm::vec3 g) {
	world->setGravity(btVector3(g.x, g.y, g.z));
}

void PhysicsScene::update(float delta) {
	world->stepSimulation(delta);
	//size_t num_manifolds = world->getDispatcher()->getNumManifolds();

	//for (size_t i = 0; i < num_manifolds; i++) {
	//	btPersistentManifold* contact = world->getDispatcher()->getManifoldByIndexInternal(i);
	//	const btCollisionObject* obj0 = contact->getBody0();
	//	const btCollisionObject* obj1 = contact->getBody1();
	//}
}

std::vector<LOA::Component::HitBox::CollisionEvent> PhysicsScene::checkForContacts(btPairCachingGhostObject* ghost) {
	btManifoldArray   manifold_array;
	btBroadphasePairArray& pair_array = ghost->getOverlappingPairCache()->getOverlappingPairArray();
	size_t num_pairs = pair_array.size();
	
	std::vector<LOA::Component::HitBox::CollisionEvent> events;

	for (size_t i = 0; i < num_pairs; i++) {
		manifold_array.clear();

		const btBroadphasePair& pair = pair_array[i];
		btBroadphasePair* collision_pair = world->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
		if (collision_pair == nullptr) {
			continue;
		}

		if (collision_pair->m_algorithm != nullptr) {
			collision_pair->m_algorithm->getAllContactManifolds(manifold_array);
		}

		for (size_t j = 0; j < manifold_array.size(); j++) {
			btPersistentManifold* manifold = manifold_array[j];
			if (manifold->getNumContacts() > 0) {
				Component::HitBox::CollisionEvent event;

				const btCollisionObject* body = manifold->getBody0() == ghost ? manifold->getBody1() : manifold->getBody0();
				assert(manifold->getBody0() == ghost || manifold->getBody1() == ghost);
				
				entt::entity other = (entt::entity)body->getUserIndex();
				//todo perform some assertions to verify its correct
				Component::EventType event_type = (Component::EventType)body->getUserIndex2();
				event.other_entity = other;
				event.other_event_type = event_type;

				events.push_back(event);
			}
			else {
			
			}
		}
	}

	return events;
}

std::pair<bool,glm::vec3> PhysicsScene::castRay(glm::vec3 start, glm::vec3 stop, bool debug) const {
	if (glm::distance2(start, stop) < .001f) {
		return { false, glm::vec3(0, 0, 0) };
	}
	
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
	
	body->setUserIndex((i32)0);
	body->setUserIndex2((i32)Component::EventType::NONE);

	if (mass == 0) {
		body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		constexpr int mask = btBroadphaseProxy::DefaultFilter 
			| btBroadphaseProxy::StaticFilter 
			| btBroadphaseProxy::CharacterFilter 
			| btBroadphaseProxy::SensorTrigger;
		world->addRigidBody(body, btBroadphaseProxy::KinematicFilter, mask);
	}
	else {
		body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
		constexpr int mask = btBroadphaseProxy::DefaultFilter 
			| btBroadphaseProxy::StaticFilter 
			| btBroadphaseProxy::KinematicFilter 
			| btBroadphaseProxy::CharacterFilter 
			| btBroadphaseProxy::SensorTrigger;
		world->addRigidBody(body, btBroadphaseProxy::DefaultFilter, mask);
	}

	
	return body;
}

btRigidBody* PhysicsScene::createStaticBox(float mass, glm::vec3 dim, glm::vec3 pos, glm::quat rot) {
	btCollisionShape* shape = new btBoxShape(btVector3(dim.x, dim.y, dim.z));
	shapes.push_back(shape);

	btVector3 inertia(0, 0, 0);


	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, shape, inertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	groundTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	body->setWorldTransform(groundTransform);

	body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

	world->addRigidBody(body, btBroadphaseProxy::StaticFilter, btBroadphaseProxy::AllFilter ^ btBroadphaseProxy::StaticFilter);

	body->setUserIndex((i32)0);
	body->setUserIndex2((i32)Component::EventType::NONE);
	body->setUserPointer(nullptr);
	return body;
}

btRigidBody* PhysicsScene::createStaticPlane(glm::vec3 normal, float scalar) {
	btCollisionShape* shape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), scalar);
	shapes.push_back(shape);
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, nullptr, shape);

	btRigidBody* body = new btRigidBody(rbInfo);
	btTransform groundTransform;
	groundTransform.setIdentity();
	body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	body->setWorldTransform(groundTransform);
	body->setUserIndex((i32)0);
	body->setUserIndex2((i32)Component::EventType::NONE);

	world->addRigidBody(body);
	return body;
}

btPairCachingGhostObject* PhysicsScene::createHitBox(glm::vec3 dim, glm::vec3 pos) {
	btCollisionShape* shape = new btBoxShape(btVector3(dim.x, dim.y, dim.z));
	shapes.push_back(shape);

	btPairCachingGhostObject* ghost = new btPairCachingGhostObject();

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	ghost->setWorldTransform(groundTransform);

	ghost->setCollisionShape(shape);
	ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	ghost->setCustomDebugColor(btVector3(1, 0, 1));
	world->addCollisionObject(ghost, btBroadphaseProxy::SensorTrigger, (btBroadphaseProxy::AllFilter ^ (btBroadphaseProxy::CharacterFilter)));

	ghost->setUserIndex((i32)0);
	ghost->setUserIndex2((i32)Component::EventType::NONE);
	ghost->setUserPointer(nullptr);
	return ghost;
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
	ghost_object->setUserIndex((i32)0);
	ghost_object->setUserIndex2((i32)Component::EventType::NONE);
	//TODO: fix leak
	btKinematicCharacterController* controller = new btKinematicCharacterController(ghost_object, shape, .1, btVector3(0, 1, 0));
	
	//No clue what the parameters are 
	//Found in the demo
	world->addCollisionObject(ghost_object, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::CharacterFilter);
	world->addAction(controller);

	return controller;
}

void PhysicsScene::freeBox(btRigidBody* body) {
	delete body->getMotionState();
	world->removeRigidBody(body);
	delete body;

}

void PhysicsScene::freeCharacterController(btKinematicCharacterController* controller) {
	world->removeCollisionObject(controller->getGhostObject());
	world->removeAction(controller);

	//TODO: investigate potential leak of ghost object
	//not sure if bullet deallocates that for me
	delete controller;
}

void PhysicsScene::freeHitBox(btPairCachingGhostObject* ghost) {
	world->removeCollisionObject(ghost);
	delete ghost;
}

PhysicsDebugDrawer* PhysicsScene::getDrawer() const { 
	return glDrawer;
}