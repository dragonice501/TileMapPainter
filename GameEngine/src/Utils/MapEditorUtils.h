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
	SELECT_UNIT_TOOL
};

enum ETerrainType
{
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
	CASTLE_WALL,
	UNDEFINED
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

enum EAttackType
{
	PHYSICAL,
	RANGED,
	MAGIC,
	AT_NONE
};

enum EAttackDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	UP_RIGHT,
	UP_LEFT,
	DOWN_RIGHT,
	DOWN_LEFT
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

	EAttackType attackType = AT_NONE;

	bool operator!= (const AnimatedUnitSprite& other)
	{
		return
			position.GetX() != other.position.GetX() ||
			position.GetY() != other.position.GetY() ||
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
			position.GetX() == other.position.GetX() &&
			position.GetY() == other.position.GetY() &&
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
