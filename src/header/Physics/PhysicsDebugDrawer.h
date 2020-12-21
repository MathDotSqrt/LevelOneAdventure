#pragma once
#include <LinearMath/btIDebugDraw.h>

#include "Graphics/Attrib.h"
#include "Graphics/Geometry.h"
#include "Graphics/VAO.h"
#include "Graphics/VBO.h"


namespace LOA::Physics {
	class PhysicsDebugDrawer : public btIDebugDraw {
	public:
		const static size_t MAX_ELEMENTS = 4096 << 1;

		PhysicsDebugDrawer();

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		
		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;

		void reportErrorWarning(const char* warningString) override;

		void draw3dText(const btVector3& location, const char* textString) override;

		void setDebugMode(int debugMode) override;

		int getDebugMode() const override;

		void bufferData();

		const Graphics::VAO& getVAO() const;
		size_t getNumElements() const;
	private:
		Graphics::VAO vao;
		Graphics::VBO vbo;
		Graphics::Geometry<Graphics::PositionAttrib, Graphics::ColorAttrib> lineGeometry;
		size_t numElements = 0;
		int debugMode = 0;
		
	};
}