#include "NotImplementedScene.h"
#include "App.h"
#include "../Shapes/AARectangle.h"
#include "Screen.h"

void NotImplementedScene::Init()
{
	ButtonAction backAction;
	backAction.key = GameController::CancelKey();
	backAction.action = [this](uint32_t dt, InputState state)
	{
		App::Singleton().PopScene();
	};
	mGameController.AddInputActionForKey(backAction);
}

void NotImplementedScene::Update(uint32_t dt)
{
}

void NotImplementedScene::Render(Screen& screen)
{
	const BitmapFont& font = App::Singleton().GetFont();

	AARectangle rect = { Vec2D::Zero, App::Singleton().GetWidth(), App::Singleton().GetHeight()};

	Vec2D textDrawPosition;
	textDrawPosition = font.GetDrawPosition(GetSceneName(), rect, BFXA_CENTER, BFYA_CENTER);

	screen.Draw(font, GetSceneName(), textDrawPosition, Color::Red());
}

const std::string& NotImplementedScene::GetSceneName() const
{
	static std::string name = "Not Implemented";
	return name;
}
