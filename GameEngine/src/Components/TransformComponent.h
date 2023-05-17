#pragma once

#include "../Utils/Vec2D.h"

struct TransformComponent
{
	TransformComponent(Vec2D position = Vec2D::Zero, double rotation = 0.0, Vec2D scale = Vec2D(1.0f, 1.0f))
	{
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
	}

	Vec2D position;
	double rotation;
	Vec2D scale;
};