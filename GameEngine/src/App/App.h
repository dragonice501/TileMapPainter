#pragma once

#include "../Graphics/Screen.h"
#include "../Scenes/Scene.h"
#include "../Input/InputController.h"
#include "../Graphics/BitmapFont.h"

#include <stdint.h>
#include <vector>
#include <memory>

struct SDL_Window;

class App
{
public:
	static App& Singleton();
	static const std::string& GetAssetsPath();
	static const Vec2D& GetTrueCursorPosition(const Vec2D& mCursorPosition);

	bool Init();
	void Run();

	inline uint32_t GetWidth() const { return mScreen.GetWidth(); }
	inline uint32_t GetHeight() const { return mScreen.GetHeight(); }
	inline int GetMagnification() const { return SCREEN_MAGNIFICATION; }

	inline const BitmapFont& GetFont() const { return mFont; }

	void PushScene(std::unique_ptr<Scene> scene);
	void PopScene();
	Scene* TopScene();

private:
	static const int SCREEN_WIDTH = 800;
	static const int SCREEN_HEIGHT = 500;
	static const int SCREEN_MAGNIFICATION = 2;

	Screen mScreen;
	SDL_Window* mnoptrWindow;

	std::vector<std::unique_ptr<Scene>> mSceneStack;
	InputController mInputController;

	BitmapFont mFont;
};

