#include "AssetStore.h"

#include <SDL_image.h>
#include <iostream>

AssetStore::AssetStore()
{
}

AssetStore::~AssetStore()
{
}

void AssetStore::ClearAssets()
{
	for (auto texture : textures)
	{
		SDL_DestroyTexture(texture.second);
	}

	textures.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filepath)
{
	SDL_Surface* surface = IMG_Load(filepath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	textures.emplace(assetId, texture);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId)
{
	return textures[assetId];
}
