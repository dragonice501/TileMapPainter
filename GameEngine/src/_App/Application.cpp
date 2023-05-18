#include "Application.h"
#include "../Utils/FileCommandLoader.h"
#include "../_App/ECSMapTest.h"
#include "../Scenes/MapEditorScene.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

#include <iostream>
#include <fstream>
#include <string>

uint32_t Application::mWindowWidth = 0;
uint32_t Application::mWindowHeight = 0;

Application::Application(): mIsRunning(false)
{
	mRegistry = std::make_unique<Registry>();
	mAssetStore = std::make_unique<AssetStore>();
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
	mWindowWidth = 1024;
	mWindowHeight = 1024;

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
	/*Setup();
	while (mIsRunning)
	{
		Input();
		Update();
		Render();
	}*/

	/*ECSMapTest mapTest;
	mapTest.Init(mRegistry, mAssetStore, mRenderer);

	while (mapTest.IsRunning())
	{
		uint32_t timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
		if (timeToWait > 0 && timeToWait <= millisecondsPreviousFrame)
		{
			SDL_Delay(timeToWait);
		}

		double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0;

		millisecondsPreviousFrame = SDL_GetTicks();

		mapTest.Input(mRegistry);
		mapTest.Update(mRegistry, deltaTime);
		mapTest.Render(mRegistry, mAssetStore, mRenderer);
	}*/

	MapEditorScene mMapEditorScene;
	mMapEditorScene.Init(mRenderer, mRegistry, mAssetStore);

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

void Application::Setup()
{
	mRegistry->AddSystem<MovementSystem>();
	mRegistry->AddSystem<RenderSystem>();
	mRegistry->AddSystem<AnimationSystem>();

	mAssetStore->AddTexture(mRenderer, "Sigurd", "./Assets/Sigurd.png");
	mAssetStore->AddTexture(mRenderer, "tilemap-image", "./Assets/tilemaps/jungle.png");

	int tileSize = 32;
	double tileScale = 2.0;
	int mapNumCols = 25;
	int mapNumRows = 20;
	std::fstream mapFile;
	mapFile.open("./Assets/tilemaps/jungle.map");
	for (int y = 0; y < mapNumRows; y++)
	{
		for (int x = 0; x < mapNumCols; x++)
		{
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = mRegistry->CreateEntity();
			tile.AddComponent<TransformComponent>(Vec2D(x * (tileScale * tileSize), y * (tileScale * tileSize)), 0.0, Vec2D(tileScale, tileScale));
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
		}
	}
	mapFile.close();

	Entity sigurd = mRegistry->CreateEntity();
	sigurd.AddComponent<TransformComponent>(Vec2D(10.0f, 30.0f), 0.0, Vec2D(2.0f, 2.0f));
	sigurd.AddComponent<RigibodyComponent>(Vec2D(10.0f, 20.0f));
	sigurd.AddComponent<SpriteComponent>("Sigurd", 32, 32, 1, 0, 0);
	sigurd.AddComponent<AnimationComponent>(4, 4, true);
}

void Application::Input()
{
	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
				mIsRunning = false;
			break;
		default:
			break;
		}
	}
}

void Application::Update()
{
	uint32_t timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= millisecondsPreviousFrame)
	{
		SDL_Delay(timeToWait);
	}

	double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0;

	millisecondsPreviousFrame = SDL_GetTicks();

	mRegistry->GetSystem<MovementSystem>().Update(deltaTime);
	mRegistry->GetSystem<AnimationSystem>().Update();

	mRegistry->Update();
}

void Application::Render()
{
	SDL_SetRenderDrawColor(mRenderer, 21, 21, 21, 255);
	SDL_RenderClear(mRenderer);

	mRegistry->GetSystem<RenderSystem>().Update(mRenderer, mAssetStore);

	SDL_RenderPresent(mRenderer);
}