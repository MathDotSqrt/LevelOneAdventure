#pragma once
#include <iostream>
#include "common.h"

namespace LOA::Graphics {
	const char* GetGLErrorStr(GLenum err);
	void CheckGLError();
}