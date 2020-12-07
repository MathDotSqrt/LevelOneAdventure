#pragma once
#include "Graphics/Attrib.h"
#include "Graphics/Geometry.h"

namespace LOA::Graphics {
	Geometry<PositionAttrib, TexcoordAttrib> gen_quad(float width = 1);
	Geometry<Position2DAttrib, TexcoordAttrib> gen_quad2D(float width=1);

	Geometry<PositionAttrib, TexcoordAttrib, NormalAttrib> gen_cube(float width);
}