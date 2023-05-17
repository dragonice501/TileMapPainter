#pragma once

#include "Scene.h"

class Screen;

class NotImplementedScene : public Scene
{
public:
	void Init() override;
	void Update(uint32_t dt) override;
	void Render(Screen& screen) override;
	const std::string& GetSceneName() const override;

private:

};