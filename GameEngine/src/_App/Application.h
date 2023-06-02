#pragma once

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

	bool mIsRunning;

	static uint32_t mWindowWidth;
	static uint32_t mWindowHeight;

	uint32_t millisecondsPreviousFrame;
};

