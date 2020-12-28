#pragma once
#include "Graphics/Attrib.h"
#include "Graphics/Geometry.h"

namespace LOA::Graphics {
	Geometry<PositionAttrib, TexcoordAttrib> gen_quad(float width = 1);
	Geometry<Position2DAttrib, TexcoordAttrib> gen_quad2D(float width = 1);
	Geometry<Position2DAttrib> gen_circle(int divisions=10, float radius = 1);
	Geometry<PositionAttrib, NormalAttrib> gen_sphere(float radius = 1, int long_divisions = 10, int lat_divisions = 10);

	Geometry<PositionAttrib, TexcoordAttrib, NormalAttrib> gen_cube(float width);
}