#pragma once

#include "../ECS/ESC.h"
#include "../AssetStore/AssetStore.h"

#include <SDL.h>
#include <stdint.h>
#include <memory>


class ECSScene
{
public:
	virtual ~ECSScene() {}

	virtual void Init(std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer) = 0;
	virtual void Destroy() = 0;
	virtual void Setup(std::unique_ptr<Registry>& registry) = 0;

	virtual void Input(std::unique_ptr<Registry>& registry) = 0;
	virtual void Update(std::unique_ptr<Registry>& registry, uint32_t deltaTime) = 0;
	virtual void Render(std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer) = 0;

	virtual const bool IsRunning() const = 0;

protected:
	bool mIsRunning;
};