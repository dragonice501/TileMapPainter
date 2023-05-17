#pragma once

#include "../Scenes/Scene.h"
#include "Button.h"

#include <string>
#include <vector>

class Screen;

class ButtonOptionsScene: public Scene
{
public:
	ButtonOptionsScene(const std::vector<std::string>& optionNames, const Color& textColor);

	virtual void Init() override;
	virtual void Update(uint32_t deltaTime) override;
	virtual void Render(Screen& screen) override;

	void SetButtonAction(const std::vector<Button::ButtonAction>& buttonActions);

private:
	void SetNextButtonHighlighted();
	void SetPreviousButtonHighlighted();
	void ExecuteCurrentButtonAction();
	void HighlightCurrentButton();

	std::vector<Button> mButtons;
	int mHighlightedOption;
};

