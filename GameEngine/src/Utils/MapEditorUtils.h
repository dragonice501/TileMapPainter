#pragma once

#include "../Utils/Vec2D.h"

#include <stdint.h>

enum EEditorState
{
	EDITING_MAP,
	SELECTING_SPRITE
};

enum ESelectedTool
{
	PAN_TOOL,
	PAINT_TILE_TOOL,
	FILL_TILE_TOOL,
	SELECT_TILE_TOOL,
	PAINT_UNIT_TOOL,
	SELECT_TILE_TOL
};

enum ETerrainType
{
	UNDEFINED,
	ROAD,
	BRIDGE,
	PLAIN,
	SAND,
	RUINS,
	FOREST,
	THICKET,
	MOUNTAIN,
	PEAK,
	CLIFF,
	SEA,
	RIVER,
	DESERT,
	VILLAGE,
	CHURCH,
	BRAGI_TOWER,
	CASTLE_DEFENSE,
	CASTLE_WALL
};

enum EUnitClass
{
	BOW_FIGHTER,
	DANCER,
	KNIGHT_LORD,
	MAGE,
	SWORD_ARMOUR,
	NONE
};

struct AnimatedUnitSprite
{
	uint8_t textureSize = 128;
	uint8_t frameSize = 32;
	uint8_t numFrames = 4;
	uint8_t frameRate = 4;
	uint8_t currentFrame = 0;
	uint32_t startTime = 0;
	Vec2D position = Vec2D::Zero;
	EUnitClass unitTexture = NONE;

	uint8_t level = 1;
	uint8_t hp = 1;
	uint8_t strength = 1;
	uint8_t magic = 1;
	uint8_t skill = 1;
	uint8_t speed = 1;
	uint8_t luck = 1;
	uint8_t defense = 1;
	uint8_t movement = 1;

	bool operator!= (const AnimatedUnitSprite& other)
	{
		return
			unitTexture != other.unitTexture ||
			level != other.level ||
			hp != other.hp ||
			strength != other.strength ||
			magic != other.magic ||
			skill != other.skill ||
			speed != other.speed ||
			luck != other.luck ||
			defense != other.defense ||
			movement != other.movement;
	}
	bool operator== (const AnimatedUnitSprite& other)
	{
		return
			unitTexture == other.unitTexture &&
			level == other.level &&
			hp == other.hp &&
			strength == other.strength &&
			magic == other.magic &&
			skill == other.skill &&
			speed == other.speed &&
			luck == other.luck &&
			defense == other.defense &&
			movement == other.movement;
	}
};

//uint32_t mRoadIndeces[47] =
//{
//	65,66,67,68,69,70,73,75,76,77,78,97,98,99,100,101,102,105,106,108,109,110,130,131,132,133,
//	134,135,136,137,138,139,140,141,142,162,165,166,167,168,170,173,357,358,359,360,363
//};
//
//uint32_t mBridgeIndeces[5] =
//{
//	529,530,531,561,593
//};
//
//uint32_t mPlainIndeces[42] =
//{
//	1,2,3,4,5,7,8,9,10,12,13,33,34,35,36,37,38,39,40,41,42,43,44,45,
//	46,129,161,163,164,353,354,387,419,230,231,233,234,353,354,562,564,565
//};
//
//uint32_t mSandIndeces[10] =
//{
//	454,455,456,457,458,486,487,488,489,490
//};
//
//uint32_t mForestIndeces[17] =
//{
//	193,194,195,196,197,198,198,199,200,201,202,225,226,227,228,229,232
//};
//
//uint32_t mThicketIndeces[18] =
//{
//	257,258,259,260,261,262,263,264,265,266,289,290,291,292,293,296,297,298
//};
//
//uint32_t mMountainIndeces[58] =
//{
//	17,18,19,20,24,25,49,50,52,55,56,57,58,59,81,82,83,84,86,87,88,89,114,115,116,118,119,120,
//	121,122,123,124,146,147,151,152,153,155,156,182,183,184,185,186,187,188,214,215,216,217,218,219,246,247,248,250,280,281
//};
//
///*uint32_t mPeakIndeces[] =
//{
//
//};*/
//
//uint32_t mCliffIndeces[38] =
//{
//	235,236,237,267,268,269,270,299,300,301,302,321,322,323,324,325,326,327,
//	328,329,330,332,355,356,513,514,515,516,517,518,520,521,522,545,549,550,552,555
//};
//
///*uint32_t mSeaIndeces[] =
//{
//
//};*/
//
//uint32_t mRiverIndeces[17] =
//{
//	385,386,388,390,391,392,393,394,417,418,420,421,422,423,424,425,426
//};
//
///*uint32_t mDesertIndeces[] =
//{
//
//};*/
//
//uint32_t mVillageIndeces[3] =
//{
//	595,596,597
//};
//
//uint32_t mChurchIndeces[1] =
//{
//	594
//};
//
//uint32_t mBragiTowerIndeces[3] =
//{
//	626,658,690
//};
//
//uint32_t mCastleDefenseIndeces[4] =
//{
//	532,533,625,657
//};
//
//uint32_t mCastleWallIndeces[35] =
//{
//	535,536,538,539,566,567,568,569,570,571,598,599,600,601,602,603,662,
//	663,665,666,667,693,694,695,696,697,698,699,725,726,727,728,729,762,763
//};