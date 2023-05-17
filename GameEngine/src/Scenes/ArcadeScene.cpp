#include "ArcadeScene.h"
#include "GameScene.h"
#include "../Scenes/NotImplementedScene.h"
#include "../Graphics/Screen.h"
#include "../Graphics/Color.h"
#include "../Input/GameController.h"
#include "../App/App.h"

#include <SDL.h>
#include <iostream>
#include <vector>

ArcadeScene::ArcadeScene(): ButtonOptionsScene({"Tetris", "BreakOut"}, Color::Cyan())
{
}

void ArcadeScene::Init()
{
	ButtonAction acceptAction;
	acceptAction.key = GameController::ActionKey();
	acceptAction.action = [this](uint32_t dt, InputState state)
	{
		
	};
	mGameController.AddInputActionForKey(acceptAction);

	ButtonAction backAction;
	backAction.key = GameController::CancelKey();
	backAction.action = [this](uint32_t dt, InputState state)
	{

	};
	mGameController.AddInputActionForKey(backAction);

	ButtonAction upAction;
	upAction.key = GameController::UpKey();
	upAction.action = [this](uint32_t dt, InputState state)
	{

	};
	mGameController.AddInputActionForKey(upAction);

	ButtonAction downAction;
	downAction.key = GameController::DownKey();
	downAction.action = [this](uint32_t dt, InputState state)
	{

	};
	mGameController.AddInputActionForKey(downAction);

	ButtonAction leftAction;
	leftAction.key = GameController::LeftKey();
	leftAction.action = [this](uint32_t dt, InputState state)
	{

	};
	mGameController.AddInputActionForKey(leftAction);

	ButtonAction rightAction;
	rightAction.key = GameController::RightKey();
	rightAction.action = [this](uint32_t dt, InputState state)
	{

	};
	mGameController.AddInputActionForKey(rightAction);

	mGameController.SetMouseMovedAction([this](const MousePosition& mousePosition)
	{
		//mCursorPosition = { static_cast<float>(mousePosition.xPos), static_cast<float>(mousePosition.yPos) };
	});

	MouseButtonAction mouseAction;
	mouseAction.mouseButton = GameController::LeftMouseButton();
	mouseAction.mouseInputAction = [this](InputState state, const MousePosition& position)
	{

	};
	mGameController.AddMouseButtonAction(mouseAction);

	std::vector<Button::ButtonAction> actions;
	actions.push_back([this]
		{
			App::Singleton().PushScene(GetScene(TETRIS));
		});

	actions.push_back([this]
		{
			App::Singleton().PushScene(GetScene(BREAK_OUT));
		});

	SetButtonAction(actions);

	ButtonOptionsScene::Init();

	mSpriteSheet.Load("PacmanSprites");
	mAnimatedSprite.Init(App::Singleton().GetAssetsPath() + "Pacman_animations.txt", mSpriteSheet);
	mAnimatedSprite.SetAnimation("move_right", true);
}

void ArcadeScene::Update(uint32_t dt)
{
	mAnimatedSprite.Update(dt);
}

void ArcadeScene::Render(Screen& screen)
{
	ButtonOptionsScene::Render(screen);

	mAnimatedSprite.Draw(screen);
}

const std::string& ArcadeScene::GetSceneName() const
{
	static std::string sceneName = "Arcade";
	return sceneName;
}

std::unique_ptr<Scene> ArcadeScene::GetScene(eGame game)
{
	switch (game)
	{
	case ASTEROIDS:
		break;
	case BREAK_OUT:
		break;
	case TETRIS:
	{
		std::unique_ptr<Scene> notImplementedScene = std::make_unique<NotImplementedScene>();
		return notImplementedScene;
	}
		break;
	case PACMAN:
		break;
	default:
		break;
	}

	return nullptr;
}
