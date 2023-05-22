#pragma once

#include "../ECS/ESC.h"
#include "../AssetStore/AssetStore.h"

#include <SDL.h>
#include <stdint.h>

const uint8_t FPS = 60;
const uint16_t MILLISECONDS_PER_FRAME = 1000 / FPS;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	void Run();
	void Destroy();

	static const uint32_t GetWindowWidth() { return mWindowWidth; }
	static const uint32_t GetWindowHeight() { return mWindowHeight; }

private:

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	std::unique_ptr<Registry> mRegistry;
	std::unique_ptr<AssetStore> mAssetStore;

	bool mIsRunning;

	static uint32_t mWindowWidth;
	static uint32_t mWindowHeight;

	uint32_t millisecondsPreviousFrame;
};

