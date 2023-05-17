#pragma once

#include "../Shapes/AARectangle.h"
#include "../Graphics/Color.h"
#include "../Graphics/BitmapFont.h"

#include <string>
#include <functional>

class Screen;

class Button
{
public:
	using ButtonAction = std::function<void(void)>;

	Button(const BitmapFont& bitmapFont, const Color& textColor, const Color& highlightColor = Color::White());
	void Init(Vec2D topLeft, unsigned int width, unsigned height);
	void Draw(Screen& screen);
	void ExecuteAction();

	Button operator= (const Button& otherButton);

	inline void SetButtonText(const std::string& text) { mTitle = text; }
	inline const std::string& GetText() const { return mTitle; }
	inline void SetHighlighted(bool highlighted) { mHighlighted = highlighted; }
	inline bool IsHighlighted() const { return mHighlighted; }
	inline void SetButtonAction(Button::ButtonAction action) { mAction = action; }
	inline const BitmapFont& GetFont() const { return mBitmapFont; }
	inline Color GetTextColor() const { return mTextColor; }
	inline Color GetHighlightColor() const { return mHighlightColor; }
	inline const AARectangle& GetBox() const { return mBBox; }

private:
	const BitmapFont& mBitmapFont;
	std::string mTitle;
	AARectangle mBBox;

	bool mHighlighted;
	ButtonAction mAction;
	Color mHighlightColor;
	Color mTextColor;
};

