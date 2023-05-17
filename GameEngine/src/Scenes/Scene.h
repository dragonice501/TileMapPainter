#pragma once

#include "../Input/GameController.h"

#include <string>
#include <stdint.h>

class Screen;

class Scene
{
public:
	virtual ~Scene() {}
	virtual void Init() = 0;
	virtual void Update(uint32_t deltaTime) = 0;
	virtual void Render(Screen& screen) = 0;

	virtual const std::string& GetSceneName() const = 0;

	GameController* GetGameController() { return &mGameController; }

protected:
	GameController mGameController;
};