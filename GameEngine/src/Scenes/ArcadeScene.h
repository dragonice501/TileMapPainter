#pragma once

#include "ButtonOptionsScene.h"
#include "../Graphics/AnimatedSprite.h"
#include "../Graphics/SpriteSheet.h"

#include <memory>

enum eGame
{
	ASTEROIDS,
	BREAK_OUT,
	TETRIS,
	PACMAN
};

class Screen;

class ArcadeScene : public ButtonOptionsScene
{
public:
	ArcadeScene();

	void Init() override;
	void Update(uint32_t dt) override;
	void Render(Screen& screen) override;
	const std::string& GetSceneName() const override;

private:
	std::unique_ptr<Scene> GetScene(eGame game);

	AnimatedSprite mAnimatedSprite;
	SpriteSheet mSpriteSheet;
};