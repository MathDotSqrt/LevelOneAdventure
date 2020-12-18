#include "Physics/PhysicsDebugDrawer.h"
#include "Graphics/GLError.h"

using namespace LOA::Physics;
using namespace LOA::Graphics;

PhysicsDebugDrawer::PhysicsDebugDrawer() : vbo(VBO::BufferType::ARRAY_BUFFER){
	vao.bind();
		vbo.bind();
		vao.addVertexAttribPtr<PositionAttrib, ColorAttrib>();
		vbo.bufferData(lineGeometry.sizeofVertex() * MAX_ELEMENTS, nullptr, VBO::BufferHint::STREAM_DRAW);
		vbo.unbind();
	vao.unbind();
}

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	glm::vec3 start(from.x(), from.y(), from.z());
	glm::vec3 end(to.x(), to.y(), to.z());
	glm::vec3 c(color.x(), color.y(), color.z());

	lineGeometry.pushVertex(start, c);
	lineGeometry.pushVertex(end, c);
}

void PhysicsDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {
	//TODO:
}

void PhysicsDebugDrawer::reportErrorWarning(const char* warningString) {
	//TODO:
}

void PhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString) {
	//TODO:
}

void PhysicsDebugDrawer::setDebugMode(int debugMode) {
	this->debugMode = debugMode;
}

int PhysicsDebugDrawer::getDebugMode() const {
	return debugMode;
}



void PhysicsDebugDrawer::bufferData() {
	//each vertex is an element for opengl
	numElements = lineGeometry.getVerticies().size();

	vbo.bind();
	//improves stream draws
	vbo.bufferOrphan();
	vbo.bufferSubData(lineGeometry.getVerticies());
	vbo.unbind();

	lineGeometry.clear();
}

const VAO& PhysicsDebugDrawer::getVAO() const {
	return vao;
}

size_t PhysicsDebugDrawer::getNumElements() const { 
	return numElements;
}