#include "Graphics/GeometryBuilder.h"

using namespace LOA::Graphics;

Geometry<PositionAttrib, TexcoordAttrib> LOA::Graphics::gen_quad(float width) {
	Geometry<PositionAttrib, TexcoordAttrib> geometry;

	float h = width / 2;

	geometry.pushVertex(glm::vec3(-h, h, 0), glm::vec2(0, 0));
	geometry.pushVertex(glm::vec3(-h, -h, 0), glm::vec2(0, 1));
	geometry.pushVertex(glm::vec3(h, h, 0), glm::vec2(1, 0));
	geometry.pushVertex(glm::vec3(h, -h, 0), glm::vec2(1, 1));

	geometry.pushTriangle(0, 1, 2);
	geometry.pushTriangle(2, 1, 3);

	return geometry;
}