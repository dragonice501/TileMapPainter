#include "ButtonOptionsScene.h"
#include "App.h"
#include "../Graphics/BitmapFont.h"
#include "../Utils/Utils.h"

ButtonOptionsScene::ButtonOptionsScene(const std::vector<std::string>& optionNames, const Color& textColor):
	mHighlightedOption(0)
{
	const BitmapFont& font = App::Singleton().GetFont();

	for (size_t i = 0; i < optionNames.size(); i++)
	{
		mButtons.push_back(Button(font, textColor));
		mButtons.back().SetButtonText(optionNames[i]);
	}

	if (optionNames.size() > 0)
	{
		mButtons[mHighlightedOption].SetHighlighted(true);
	}
}

void ButtonOptionsScene::Init()
{
	ButtonAction upAction;
	upAction.key = GameController::UpKey();
	upAction.action = [this](uint32_t dt, InputState state)
	{
		SetPreviousButtonHighlighted();
	};
	mGameController.AddInputActionForKey(upAction);

	ButtonAction downAction;
	downAction.key = GameController::DownKey();
	downAction.action = [this](uint32_t dt, InputState state)
	{
		SetNextButtonHighlighted();
	};
	mGameController.AddInputActionForKey(downAction);

	ButtonAction acceptAction;
	acceptAction.key = GameController::ActionKey();
	acceptAction.action = [this](uint32_t dt, InputState state)
	{
		ExecuteCurrentButtonAction();
	};
	mGameController.AddInputActionForKey(acceptAction);

	uint32_t height = App::Singleton().GetHeight();
	uint32_t width = App::Singleton().GetWidth();

	const BitmapFont& font = App::Singleton().GetFont();

	if (mButtons.size() > 0)
	{
		Size fontSize = font.GetSizeOf(mButtons[0].GetText());

		const int BUTTON_PADDING = 10;

		unsigned int buttonHeight = fontSize.height + BUTTON_PADDING * 2;

		uint32_t maxButtonWidth = fontSize.width;

		for (const auto& button : mButtons)
		{
			Size s = font.GetSizeOf(button.GetText());

			if (s.width > maxButtonWidth)
			{
				maxButtonWidth = s.width;
			}
		}

		maxButtonWidth += BUTTON_PADDING * 2;

		const uint32_t Y_PADDING = 1;

		uint32_t yOffset = height / 2 - ((buttonHeight + Y_PADDING) * static_cast<uint32_t>(mButtons.size())) / 2;

		for (auto& button : mButtons)
		{
			button.Init(Vec2D(width / 2 - maxButtonWidth / 2, yOffset), maxButtonWidth, buttonHeight);

			yOffset += buttonHeight + Y_PADDING;
		}

		mButtons[mHighlightedOption].SetHighlighted(true);
	}
}

void ButtonOptionsScene::Update(uint32_t deltaTime)
{
}

void ButtonOptionsScene::Render(Screen& screen)
{
	for (auto& button : mButtons)
	{
		button.Draw(screen);
	}
}

void ButtonOptionsScene::SetButtonAction(const std::vector<Button::ButtonAction>& buttonActions)
{
	for (size_t i = 0; i < mButtons.size(); i++)
	{
		mButtons[i].SetButtonAction(buttonActions[i]);
	}
}

void ButtonOptionsScene::SetNextButtonHighlighted()
{
	mHighlightedOption = (mHighlightedOption + 1) % mButtons.size();

	HighlightCurrentButton();
}

void ButtonOptionsScene::SetPreviousButtonHighlighted()
{
	mHighlightedOption--;

	if (mHighlightedOption < 0)
	{
		mHighlightedOption = static_cast<int>(mButtons.size()) - 1;
	}

	HighlightCurrentButton();
}

void ButtonOptionsScene::ExecuteCurrentButtonAction()
{
	mButtons[mHighlightedOption].ExecuteAction();
}

void ButtonOptionsScene::HighlightCurrentButton()
{
	for (auto& button : mButtons)
	{
		button.SetHighlighted(false);
	}

	mButtons[mHighlightedOption].SetHighlighted(true);
}
