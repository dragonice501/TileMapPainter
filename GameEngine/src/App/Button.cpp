#include "Button.h"
#include "../Graphics/Screen.h"

Button::Button(const BitmapFont& bitmapFont, const Color& textColor, const Color& highlightColor):
	mBitmapFont(bitmapFont), mTextColor(textColor), mHighlightColor(highlightColor), mTitle(""), mHighlighted(false), mAction(NULL)
{
}

void Button::Init(Vec2D topLeft, unsigned int width, unsigned height)
{
	mBBox = { topLeft, width, height };
}

void Button::Draw(Screen& screen)
{
	screen.Draw(mBitmapFont, mTitle, mBitmapFont.GetDrawPosition(mTitle, mBBox, BFXA_CENTER, BFYA_CENTER), mTextColor);

	if (mHighlighted)
	{
		screen.Draw(mBBox, mHighlightColor);
	}
}

void Button::ExecuteAction()
{
	mAction();
}

Button Button::operator=(const Button& otherButton)
{
	Button newButton(otherButton.GetFont(), otherButton.GetTextColor(), otherButton.GetHighlightColor());
	return newButton;
}
