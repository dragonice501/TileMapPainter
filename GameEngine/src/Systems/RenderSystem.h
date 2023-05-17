#pragma once

#include "../ECS/ESC.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"

#include <SDL.h>
#include <algorithm>

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore)
	{
		/*struct RenderableEntity
		{
			TransformComponent transformComponent;
			SpriteComponent spriteComponent;
		};

		std::vector<RenderableEntity> renderableEntities;
		for (auto entity : GetSystemEntities())
		{
			RenderableEntity renderableEntity;
			renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
			renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
			renderableEntities.emplace_back(renderableEntity);
		}

		std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b)
		{
			return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
		});

		for (auto entity : renderableEntities)
		{
			const auto transform = entity.transformComponent;
			const auto sprite = entity.spriteComponent;

			SDL_Rect dstRect = {
				static_cast<int>(transform.position.GetX()),
				static_cast<int>(transform.position.GetY()),
				static_cast<int>(sprite.width * transform.scale.GetX()),
				static_cast<int>(sprite.height * transform.scale.GetY())
			};

			SDL_RenderCopyEx(renderer, assetStore->GetTexture(sprite.assetId), &sprite.srcRect, &dstRect, transform.rotation, NULL, SDL_FLIP_NONE);
		}*/

		for (auto entity : GetSystemEntities())
		{
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto sprite = entity.GetComponent<SpriteComponent>();

			SDL_Rect dstRect = {
				static_cast<int>(transform.position.GetX()),
				static_cast<int>(transform.position.GetY()),
				static_cast<int>(sprite.width * transform.scale.GetX()),
				static_cast<int>(sprite.height * transform.scale.GetY())
			};

			SDL_RenderCopyEx(renderer, assetStore->GetTexture(sprite.assetId), &sprite.srcRect, &dstRect, transform.rotation, NULL, SDL_FLIP_NONE);
		}
	}
};