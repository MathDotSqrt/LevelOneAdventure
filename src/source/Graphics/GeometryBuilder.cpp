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


//AHHHH SHIT FUCK IT, just load from blender file

//Geometry<PositionAttrib, NormalAttrib> LOA::Graphics::gen_sphere(float radius, int long_divisions, int lat_divisions) {
//	Geometry<PositionAttrib, NormalAttrib> geometry;
//
//	const float PI = 3.141592653f;
//	float theta_delta = 2 * PI / (long_divisions + 1);
//	float phi_delta = 2 * PI / (lat_divisions + 1);
//
//	auto get_spherical_pos = [theta_delta, phi_delta, radius](int i, int j) {
//		const float PI = 3.141592653f;
//
//		float theta = i * theta_delta;
//		float phi = j * phi_delta + PI;
//
//		float x = radius * glm::sin(theta) * glm::cos(phi);
//		float y = radius * glm::sin(theta) * glm::sin(phi);
//		float z = radius * glm::cos(theta);
//
//		glm::vec3 pos(x, y, z);
//		return pos;
//	};
//
//	//This is really inefficient to generate and wastes memory
//	//Todo: optimize mesh
//	for (int j = 0; j < lat_divisions / 2; j++) {
//		for (int i = 0; i < long_divisions; i++) {
//			size_t size = geometry.getVertexCount();
//
//			{
//				glm::vec3 pos = get_spherical_pos(i, j);
//				glm::vec3 normal = glm::normalize(pos);
//				geometry.pushVertex(pos, normal); 
//			}
//
//			{
//				glm::vec3 pos = get_spherical_pos(i + 1, j);
//				glm::vec3 normal = glm::normalize(pos);
//				geometry.pushVertex(pos, normal);
//			}
//
//			{
//				glm::vec3 pos = get_spherical_pos(i, j + 1);
//				glm::vec3 normal = glm::normalize(pos);
//				geometry.pushVertex(pos, normal);
//			}
//
//			{
//				glm::vec3 pos = get_spherical_pos(i + 1, j + 1);
//				glm::vec3 normal = glm::normalize(pos);
//				geometry.pushVertex(pos, normal);
//			}
//
//			geometry.pushTriangle(size, size + 1, size + 2);
//			geometry.pushTriangle(size + 1, size + 3, size + 2);
//
//		}
//	}
//
//	return geometry;
//}

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