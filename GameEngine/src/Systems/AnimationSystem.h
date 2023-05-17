#pragma once

#include "../ECS/ESC.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

class AnimationSystem :public System
{
public:
	AnimationSystem()
	{
		RequireComponent<SpriteComponent>();
		RequireComponent<AnimationComponent>();
	}

	void Update()
	{
		for (auto entity : GetSystemEntities())
		{
			auto& animationComponent = entity.GetComponent<AnimationComponent>();
			auto& spriteComponent = entity.GetComponent<SpriteComponent>();

			animationComponent.currentFrame = 
				static_cast<int>(((SDL_GetTicks() - animationComponent.startTime) * animationComponent.frameRateSpeed / 1000.0f)) % animationComponent.numFrames;

			spriteComponent.srcRect.x = animationComponent.currentFrame * spriteComponent.width;
		}
	}
};