#include "Graphics/GeometryBuilder.h"
#include <glm/glm.hpp>
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

Geometry<Position2DAttrib, TexcoordAttrib> LOA::Graphics::gen_quad2D(float width) {
	Geometry<Position2DAttrib, TexcoordAttrib> geometry;

	float h = width / 2;

	geometry.pushVertex(glm::vec2(-h, h), glm::vec2(0, 0));
	geometry.pushVertex(glm::vec2(-h, -h), glm::vec2(0, 1));
	geometry.pushVertex(glm::vec2(h, h), glm::vec2(1, 0));
	geometry.pushVertex(glm::vec2(h, -h), glm::vec2(1, 1));

	geometry.pushTriangle(0, 1, 2);
	geometry.pushTriangle(2, 1, 3);

	return geometry;
}

Geometry<Position2DAttrib> LOA::Graphics::gen_circle(int divisions, float radius) {
	Geometry<Position2DAttrib> geometry;
	
	geometry.pushVertex(glm::vec2(0, 0));

	const float PI = 3.141592653f;

	const float theta_delta = 2 * PI / divisions;

	for (int i = 0; i < divisions; i++) {
		float theta0 = theta_delta * i;
		float theta1 = theta_delta * (i + 1);

		glm::vec2 pos0 = glm::vec2(glm::cos(theta0), glm::sin(theta0)) * radius;
		glm::vec2 pos1 = glm::vec2(glm::cos(theta1), glm::sin(theta1)) * radius;

		geometry.pushVertex(pos0);
		geometry.pushVertex(pos1);
		size_t size = geometry.getVertexCount();
		geometry.pushTriangle(0, size - 2, size - 1);
	}

	return geometry;

}

Geometry<PositionAttrib, NormalAttrib> LOA::Graphics::gen_sphere(float radius, int long_divisions, int lat_divisions) {
	Geometry<PositionAttrib, NormalAttrib> geometry;
	geometry.resizeVerticies((size_t)long_divisions * lat_divisions);		//reserve space so it allocates once
	geometry.resizeIndices((size_t)long_divisions * lat_divisions * 6);		//reserve space so it allocates once

	const float PI = 3.141592653f;
	float theta_delta = 2 * PI / long_divisions;
	float phi_delta = 2 * PI / lat_divisions;

	for (int j = 0; j < lat_divisions; j++) {
		for (int i = 0; i < long_divisions; i++) {
			float theta = i * theta_delta;
			float phi = j * phi_delta;

			float x = radius * glm::sin(theta) * glm::cos(phi);
			float y = radius * glm::sin(theta) * glm::sin(phi);
			float z = radius * glm::cos(theta);

			glm::vec3 pos(x, y, z);
			glm::vec3 normal = glm::normalize(pos);
			geometry.pushVertex(pos, normal);
		}
	}
	
	//Converts 2D index to 1D index. Allows wrapping
	auto to_index = [long_divisions, lat_divisions](int x, int y) {
		x = (x + long_divisions) % long_divisions;
		y = (y + lat_divisions) % lat_divisions;
		return x + y * lat_divisions;
	};
	for (int y = 0; y < lat_divisions; y++) {
		for (int x = 0; x < long_divisions; x++) {
			int i0 = to_index(x, y);
			int i1 = to_index(x + 1, y);
			int i2 = to_index(x, y + 1);
			int i3 = to_index(x + 1, y + 1);

			geometry.pushTriangle(i0, i1, i2);
			geometry.pushTriangle(i1, i3, i2);
		}
	}

	return geometry;
}

//this is really bad code. pls dont look at it
Geometry<PositionAttrib, TexcoordAttrib, NormalAttrib> LOA::Graphics::gen_cube(float width) {
	Geometry<PositionAttrib, TexcoordAttrib, NormalAttrib> geometry;

	float h = width / 2;

	auto gen_face = [&geometry, h](int component, int first, int second, int sign) {
		
		//int first = (component + 1) % 3;
		//int second = (component + 2) % 3;

		{
			glm::vec3 pos;
			pos[first] = -h;
			pos[second] = h;
			pos[component] = sign * h;

			glm::vec2 tex;
			tex[0] = 0;
			tex[1] = 0;

			glm::vec3 norm(0);
			norm[component] = sign;

			geometry.pushVertex(pos, tex, norm);
		}

		{
			glm::vec3 pos;
			pos[first] = -h;
			pos[second] = -h;
			pos[component] = sign * h;

			glm::vec2 tex;
			tex[0] = 0;
			tex[1] = 1;

			glm::vec3 norm(0);
			norm[component] = sign;

			geometry.pushVertex(pos, tex, norm);
		}
		
		{
			glm::vec3 pos;
			pos[first] = h;
			pos[second] = h;
			pos[component] = sign * h;

			glm::vec2 tex;
			tex[0] = 1;
			tex[1] = 0;

			glm::vec3 norm(0);
			norm[component] = sign;

			geometry.pushVertex(pos, tex, norm);
		}

		{
			glm::vec3 pos;
			pos[first] = h;
			pos[second] = -h;
			pos[component] = sign * h;

			glm::vec2 tex;
			tex[0] = 1;
			tex[1] = 1;

			glm::vec3 norm(0);
			norm[component] = sign;

			geometry.pushVertex(pos, tex, norm);
		}

		auto offset = geometry.getVertexCount() - 4;
		geometry.pushTriangle(offset + 0, offset + 1, offset + 2);
		geometry.pushTriangle(offset + 2, offset + 1, offset + 3);

		//geometry.pushVertex(glm::vec4);
	};

	gen_face(0, 1, 2, 1);
	gen_face(0, 2, 1, -1);

	gen_face(1, 2, 0, 1);
	gen_face(1, 0, 2, -1);

	gen_face(2, 0, 1, 1);
	gen_face(2, 1, 0, -1);


	return geometry;
}