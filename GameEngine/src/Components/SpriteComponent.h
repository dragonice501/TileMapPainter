#pragma once

#include <SDL.h>
#include <string>

struct SpriteComponent
{
	SpriteComponent(std::string assetId = "", uint32_t width = 0, uint32_t height = 0, uint32_t zIndex = 0, uint32_t srcRectX = 0, uint32_t srcRectY = 0)
	{
		this->assetId = assetId;
		this->width = width;
		this->height = height;
		this->zIndex = zIndex;
		this->srcRect = {
			static_cast<int>(srcRectX),
			static_cast<int>(srcRectY),
			static_cast<int>(width),
			static_cast<int>(height) };
	}

	std::string assetId;
	uint32_t width;
	uint32_t height;
	uint32_t zIndex;
	SDL_Rect srcRect;
};