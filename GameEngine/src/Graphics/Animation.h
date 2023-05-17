#pragma once

#include "../Utils/Vec2D.h"
#include "Color.h"
#include "../Utils/Utils.h"

#include <string>
#include <vector>

struct AnimationFrame
{
	std::string frame = "";
	std::string overlay = "";
	Color frameColor = Color::White();
	Color overlayColor = Color::White();
	Vec2D size;
	Vec2D offset = Vec2D::Zero;
	int frameColorSet = 0;
};

class Animation
{
public:
	Animation();

	AnimationFrame GetAnimationFrame(uint32_t frameNum) const;

	static std::vector<Animation> LoadAnimations(const std::string& animationFilePath);

	void AddFrame(const std::string& frame) { mFrames.push_back(frame); }
	void AddFrameColor(const Color& color) { mFrameColors.push_back(color); }
	void AddOverlayColor(const Color& color) { mOverlayColors.push_back(color); }
	void AddFrameOffset(const Vec2D& offset) { mFrameOffsets.push_back(offset); }

	inline void SetSpriteSheetName(const std::string& spriteSheetName) { mSpriteSheetName = spriteSheetName; }
	inline const std::string& GetSpriteSheetName() const { return mSpriteSheetName; }
	inline void SetName(const std::string& animationName) { mAnimationName = animationName; }
	inline const std::string& GetName() const { return mAnimationName; }

	inline Vec2D GetSize() const { return mSize; }
	inline void SetSize(const Vec2D& size) { mSize = size; }

	inline void SetFPS(const int& fps) { mFPS = fps; }
	inline const int GetFPS() const { return mFPS; }

	inline void SetOverlay(const std::string& overlayName) { mOverlay = overlayName; }
	inline const std::string& OverlayName() const { return mOverlay; }

	inline size_t NumFrames() const { return mFrames.size(); }
	inline size_t NumFrameColors() const { return mFrameColors.size(); }
	inline size_t NumOverlayColors() const { return mOverlayColors.size(); }
	inline size_t NumFrameOffsets() const { return mFrameOffsets.size(); }

private:
	std::string mAnimationName;
	std::string mSpriteSheetName;
	std::string mOverlay;
	std::vector<std::string> mFrames;
	std::vector<Color> mFrameColors;
	std::vector<Color> mOverlayColors;
	std::vector<Vec2D> mFrameOffsets;
	Vec2D mSize;
	int mFPS;
};

