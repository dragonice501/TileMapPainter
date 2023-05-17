#pragma once

#include "../Utils/Vec2D.h"

struct RigibodyComponent
{
	RigibodyComponent(Vec2D velocity = Vec2D(0.0f, 0.0f))
	{
		this->velocity = velocity;
	}

	Vec2D velocity;
};