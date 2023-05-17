#pragma once

#include "Color.h"

#include <vector>
#include <string>
#include <stdint.h>

class BMPImage
{
public:
	BMPImage();

	bool Load(const std::string& path);

	const std::vector<Color>& GetPixels() const { return mPixels; }
	inline void PushColor(const Color& color) { mPixels.push_back(color); }
	inline void ClearPixels() { mPixels.clear(); }
	inline void SetWidth(uint32_t width) { mWidth = width; }
	inline void SetHeight(uint32_t height) { mHeight = height; }

	inline uint32_t GetWidth() const { return mWidth; }
	inline uint32_t GetHeight() const { return mHeight; }	

private:
	std::vector<Color> mPixels;
	uint32_t mWidth;
	uint32_t mHeight;
};

