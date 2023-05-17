#pragma once

#include <SDL.h>
#include <stdint.h>

struct AnimationComponent
{
	AnimationComponent(uint32_t numFrames = 1, uint32_t frameRateSpeed = 1, bool shouldLoop = false)
	{
		this->numFrames = numFrames;
		this->currentFrame = 1;
		this->frameRateSpeed = frameRateSpeed;
		this->shouldLoop = shouldLoop;
		this->startTime = SDL_GetTicks();
	}

	uint32_t numFrames;
	uint32_t currentFrame;
	uint32_t frameRateSpeed;
	bool shouldLoop;
	uint32_t startTime;
};