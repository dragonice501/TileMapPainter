#include "Application.h"
#include "../Scenes/MapEditorScene.h"

uint32_t Application::mWindowWidth = 0;
uint32_t Application::mWindowHeight = 0;
float Application::millisecondsPreviousFrame = 0;

Application::Application(): mIsRunning(false)
{
	
}

Application::~Application()
{
	
}

bool Application::Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return false;

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	mWindowWidth = displayMode.w;
	mWindowHeight = displayMode.h;
	//mWindowWidth = 1024;
	//mWindowHeight = 1024;
	mWindow = SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWindowWidth, mWindowHeight, SDL_WINDOW_FOREIGN);
	if (!mWindow) return false;

	//SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer) return false;

	mIsRunning = true;

	return true;
}

void Application::Run()
{
	MapEditorScene mMapEditorScene;
	mMapEditorScene.Init(mRenderer);

	while (mMapEditorScene.mIsRunning)
	{
		uint32_t timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
		if (timeToWait > 0 && timeToWait <= millisecondsPreviousFrame)
		{
			SDL_Delay(timeToWait);
		}

		double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0;

		millisecondsPreviousFrame = SDL_GetTicks();

		mMapEditorScene.Input();
		mMapEditorScene.Update(deltaTime);
		mMapEditorScene.Render(mRenderer);
	}

	mMapEditorScene.Destroy();
}

void Application::Destroy()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

const uint32_t Application::GetWindowWidth()
{
	return mWindowWidth;
}

const uint32_t Application::GetWindowHeight()
{
	return mWindowHeight;
}
