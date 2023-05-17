#include "BMPImage.h"

#include <SDL.h>
#include <iostream>

BMPImage::BMPImage(): mWidth(0), mHeight(0)
{
}

bool BMPImage::Load(const std::string& path)
{
	SDL_Surface* bmpSurface = SDL_LoadBMP(path.c_str());

	if (bmpSurface == nullptr) return false;

	mWidth = bmpSurface->w;
	mHeight = bmpSurface->h;

	uint32_t lengthOfFile = mWidth * mHeight;

	mPixels.reserve(lengthOfFile);

	SDL_LockSurface(bmpSurface);

	uint32_t* pixels = (uint32_t*)bmpSurface->pixels;

	for (uint32_t i = 0; i < lengthOfFile; i++)
	{
		Color newColor = pixels[i];

		mPixels.push_back(pixels[i]);
	}

	SDL_UnlockSurface(bmpSurface);
	SDL_FreeSurface(bmpSurface);

	return true;
}
