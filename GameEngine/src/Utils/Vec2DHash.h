#pragma once

#include <Vec2D.h>

template<>
struct std::hash<Vec2D>
{
	std::size_t operator()(const Vec2D& other) const
	{
		return((hash<float>()(other.GetX()) ^ (hash<float>()(other.GetY()) << 1)));
	}
};