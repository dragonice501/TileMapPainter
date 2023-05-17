#include "App.h"
#include "../Scenes/ArcadeScene.h"

#include <SDL.h>
#include <iostream>
#include <algorithm>
#include <fstream>

void CheckForBMPs()
{
	int numToCheck = 1922;
	for (int i = 1; i <= numToCheck; i++)
	{
		std::string path = App::Singleton().GetAssetsPath() + "bmps/" + "Chapter_0_" + std::to_string(i) + ".bmp";
		SDL_Surface* bmpSurface1 = SDL_LoadBMP(path.c_str());

		if (!bmpSurface1)
		{
			std::cout << i << " does not exist" << std::endl;
		}
		else
		{
			std::cout << i << " exists" << std::endl;
		}

		SDL_FreeSurface(bmpSurface1);
	}
}

void CheckForDuplicates()
{
	std::vector<int> duplicates;
	int numToCheck = 1922;

	for (int i = 1; i < numToCheck; i++)
	{
		bool hasDuplicate = false;
		for (int& index : duplicates)
		{
			if (i == index)
			{
				hasDuplicate = true;
				break;
			}
		}
		if (hasDuplicate)
		{
			//std::cout << i << " already checked" << std::endl;
			continue;
		}

		std::string path = App::Singleton().GetAssetsPath() + "bmps/" + "Chapter_0_" + std::to_string(i) + ".bmp";
		SDL_Surface* bmpSurface1 = SDL_LoadBMP(path.c_str());
		if (bmpSurface1)
		{
			SDL_LockSurface(bmpSurface1);
			uint32_t* pixels1 = (uint32_t*)bmpSurface1->pixels;

			uint32_t lengthOfFile1 = bmpSurface1->w * bmpSurface1->h;

			bool isSame = true;

			for (int j = i + 1; j <= numToCheck; j++)
			{
				std::string path2 = App::Singleton().GetAssetsPath() + "bmps/" + "Chapter_0_" + std::to_string(j) + ".bmp";
				SDL_Surface* bmpSurface2 = SDL_LoadBMP(path2.c_str());
				if (bmpSurface2)
				{
					SDL_LockSurface(bmpSurface2);
					uint32_t* pixels2 = (uint32_t*)bmpSurface2->pixels;

					for (uint32_t k = 0; k < lengthOfFile1; k++)
					{
						if (pixels1[k] == pixels2[k])
						{
							//std::cout << i << ',' << j << std::endl;
							continue;
						}
						else
						{
							isSame = false;
							break;
						}
					}

					if (isSame)
					{
						//std::cout << i << " is same as " << j << std::endl;
						duplicates.push_back(j);
					}

					isSame = true;

					SDL_UnlockSurface(bmpSurface2);
					SDL_FreeSurface(bmpSurface2);
				}
			}

			SDL_UnlockSurface(bmpSurface1);
			SDL_FreeSurface(bmpSurface1);
		}

		std::cout << i << " out of " << numToCheck << " complete" << std::endl;
	}

	std::sort(duplicates.begin(), duplicates.end());
	for (int& duplicate : duplicates)
	{
		std::string path = App::Singleton().GetAssetsPath() + "bmps/Chapter_0_" + std::to_string(duplicate) + ".bmp";
		//std::cout << duplicate << " is a duplicate" << std::endl;

		std::ifstream duplicateFile;
		duplicateFile.open(path);
		if (duplicateFile.is_open())
		{
			std::cout << duplicate << " file found" << std::endl;
			duplicateFile.close();
		}

		std::string renamePath = App::Singleton().GetAssetsPath() + "bmps/" + std::to_string(duplicate) + ".bmp";
		std::rename(path.c_str(), renamePath.c_str());
	}
}

App& App::Singleton()
{
    static App app;
    return app;
}

const std::string& App::GetAssetsPath()
{
	static std::string basePath = "C:/Users/narga/Desktop/Visual Studio Stuff/GameEngine/GameEngine/Assets/";

	return basePath;
}

const Vec2D& App::GetTrueCursorPosition(const Vec2D& mCursorPosition)
{
	return Vec2D(static_cast<int>(mCursorPosition.GetX() / SCREEN_MAGNIFICATION), static_cast<int>(mCursorPosition.GetY() / SCREEN_MAGNIFICATION));
}

bool App::Init()
{
	if (!mFont.Load("ArcadeFont"))
	{
		return false;
	}

    mnoptrWindow = mScreen.Init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_MAGNIFICATION);

    return mnoptrWindow != nullptr;
}

void App::Run()
{
	if (mnoptrWindow)
	{
		bool running = true;

		uint32_t lastTick = SDL_GetTicks();
		uint32_t currentTick = lastTick;

		uint32_t dt = 10;
		uint32_t accumulator = 0;

		mInputController.Init([&running](uint32_t dt, InputState state)
		{
			running = false;
		});

		while (running)
		{
			currentTick = SDL_GetTicks();
			uint32_t frameTime = currentTick - lastTick;

			if (frameTime > 300)
			{
				frameTime = 300;
			}

			lastTick = currentTick;

			accumulator += frameTime;

			mInputController.Update(dt);

			if (TopScene())
			{
				while (accumulator >= dt)
				{
					TopScene()->Update(dt);
					accumulator -= dt;
				}

				TopScene()->Render(mScreen);
			}

			mScreen.SwapScreen();
		}
	}
}

void App::PushScene(std::unique_ptr<Scene> scene)
{
	if (scene)
	{
		scene->Init();
		mInputController.SetGameController(scene->GetGameController());
		mSceneStack.emplace_back(std::move(scene));
		SDL_SetWindowTitle(mnoptrWindow, TopScene()->GetSceneName().c_str());
	}
}

void App::PopScene()
{
	if (mSceneStack.size() > 1)
	{
		mSceneStack.pop_back();
	}

	if (TopScene())
	{
		mInputController.SetGameController(TopScene()->GetGameController());
		SDL_SetWindowTitle(mnoptrWindow, TopScene()->GetSceneName().c_str());
	}
}

Scene* App::TopScene()
{
	if (mSceneStack.empty()) return nullptr;

	return mSceneStack.back().get();
}
