#include "Star2D.h"

#include <cmath>
#include <iostream>

Star2D::Star2D(): Star2D(Vec2D(0, 0), 100.0)
{
}

Star2D::Star2D(const Vec2D& center, float armLength)
{
	mCenter = center;

	for (int i = 0; i < 5; i++)
	{
		float r = (72 * i * 3.14159) / 180;
		float xPos = -sin(r) * armLength;
		float yPos = -cos(r) * armLength;

		mPoints[i] = Vec2D(xPos + center.GetX(), yPos + center.GetY());
	}
}

Vec2D Star2D::Midpoint()
{
	Vec2D midpoint;
	for (int i = 0; i < 5; i++)
	{
		midpoint += mPoints[i];
	}
	midpoint /= 5;
	
	return midpoint;
}

void Star2D::Rotate(float angle)
{
	Vec2D p;
	for (int i = 0; i < 5; i++)
	{
		p = mPoints[i];

		p.Rotate(angle, mCenter);

		mPoints[i] = p;
	}
}
