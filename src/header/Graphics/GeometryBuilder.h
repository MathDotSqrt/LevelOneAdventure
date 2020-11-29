#pragma once
#include "Graphics/Attrib.h"
#include "Graphics/Geometry.h"

namespace LOA::Graphics {
	Geometry<PositionAttrib, TexcoordAttrib> gen_quad(float width=1);
}