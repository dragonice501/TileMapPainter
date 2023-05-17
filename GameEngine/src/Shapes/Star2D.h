#pragma once

#include "../Utils/Vec2D.h"

class Star2D
{
public:
	Star2D();
	Star2D(const Vec2D& center, float armLength);

	inline const Vec2D& GetPointAtIndex(uint32_t index) const { return mPoints[index]; }

	Vec2D Midpoint();
	void Rotate(float angle);

private:
	Vec2D mCenter;
	Vec2D mPoints[5];
};

