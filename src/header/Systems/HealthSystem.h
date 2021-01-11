#pragma once

#include "System.h"

class HealthSystem : public BaseSystem{
public:
	HealthSystem(LOA::Engine& engine) : BaseSystem(engine) {}
	void init();
	void update(float delta);
};