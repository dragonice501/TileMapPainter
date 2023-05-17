#pragma once

#include "ScreenBuffer.h"
#include "Color.h"

#include <stdint.h>
#include <vector>
#include <string>
#include <functional>

class Vec2D;
class Line2D;
class Star2D;
class Triangle;
class Circle;
class AARectangle;
class BMPImage;
class SpriteSheet;
class BitmapFont;

struct Sprite;
struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_PixelFormat;
struct SDL_Texture;

class Screen
{
public:
	Screen();
	~Screen();

	SDL_Window* Init(uint32_t w, uint32_t h, uint32_t mag, bool fast = true);
	void SwapScreen();

	inline void SetClearColor(const Color& clearColor) { mClearColor = clearColor; }
	inline uint32_t GetWidth() const { return mWidth; }
	inline uint32_t GetHeight() const { return mHeight; }

	void Draw(int x, int y, const Color& color);
	void Draw(const Vec2D& point, const Color& color);
	void Draw(const Line2D& line, const Color& color);
	void Draw(const Star2D& star, const Color& color);
	void Draw(const Triangle& triangle, const Color& color, bool fill = false, const Color& fillColor = Color::White());
	void Draw(const AARectangle& rectangle, const Color& color, bool fill = false, const Color& fillColor = Color::White());
	void Draw(const Circle& circle, const Color& color, bool fill = false, const Color& fillColor = Color::White());
	void Draw(const BMPImage& image, const Vec2D& pos);
	void Draw(const BMPImage& image, const Sprite& sprite, const Vec2D& pos, const Color& overlayColor = Color::White());
	void Draw(const SpriteSheet& spriteSheet, const std::string& spriteName, const Vec2D& pos, const Color& overlayColor = Color::White());
	void Draw(const BitmapFont& font, const std::string& textline, const Vec2D& pos, const Color& overlayColor = Color::White());

private:
	Screen(const Screen& screen);
	Screen& operator=(const Screen& screen);

	void ClearScreen();

	using FillPolyFunc = std::function<Color(uint32_t x, uint32_t y)>;
	void FillPoly(const std::vector<Vec2D>& points, FillPolyFunc function);

	uint32_t mWidth;
	uint32_t mHeight;

	Color mClearColor;
	ScreenBuffer mBackBuffer;

	SDL_Window* moptrWindow;
	SDL_Surface* mnoptrWindowSurface;

	SDL_Renderer* mRenderer;
	SDL_PixelFormat* mPixelFormat;
	SDL_Texture* mTexture;
	bool mFast;
};

