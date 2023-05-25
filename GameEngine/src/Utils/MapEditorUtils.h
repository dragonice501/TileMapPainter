#pragma once

#include "../Utils/Vec2D.h"

#include <SDL.h>
#include <stdint.h>

enum EEditorState
{
	ES_EDITING_MAP,
	ES_SELECTING_SPRITE,
	ES_PLAYING_GAME
};

enum EGameState
{
	GS_PLAYER_IDLE,
	GS_UNIT_MOVING,
	GS_SELECTING_ACTION,
	GS_SELECTING_TARGET,
	GS_ATTACKING,
	GS_ENEMY_PHASE
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
	BARBARIAN,
	BARBARIAN_ARCHER,
	BARBARIAN_CHIEF,
	NONE
};

enum EUnitState
{
	US_IDLE,
	US_MOVING,
	US_SELECTING_ACTION,
	US_SELECTING_TARGET,
	US_ATTACKING
};

enum EUnitMovementDirection
{
	UM_IDLE,
	UM_UP,
	UM_DOWN,
	UM_LEFT,
	UM_RIGHT
};

enum EAttackType
{
	AT_PHYSICAL,
	AT_RANGED,
	AT_MAGIC,
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
	uint8_t maxHP = 1;
	uint8_t currentHP = 1;
	uint8_t strength = 1;
	uint8_t magic = 1;
	uint8_t skill = 1;
	uint8_t speed = 1;
	uint8_t luck = 1;
	uint8_t defense = 1;
	uint8_t movement = 1;

	EUnitState unitState = US_IDLE;
	EAttackType attackType = AT_NONE;
	EUnitMovementDirection movementDirection = UM_IDLE;
	EUnitMovementDirection attackDirection = UM_IDLE;

	std::vector<Vec2D> movementPath;
	int currentPathGoalIndex;
	float movementRate = 0.0f;
	float movementSpeed = 5.0f;

	void Update(const float& deltaTime)
	{
		currentFrame = static_cast<int>(((SDL_GetTicks() - startTime) * frameRate / 1000.0f)) % numFrames;

		if (unitState == US_MOVING)
		{
			MoveThroughPath(deltaTime);
		}
	}

	bool operator!= (const AnimatedUnitSprite& other)
	{
		return
			position.GetX() != other.position.GetX() ||
			position.GetY() != other.position.GetY() ||
			unitTexture != other.unitTexture ||
			level != other.level ||
			maxHP != other.maxHP ||
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
			maxHP == other.maxHP &&
			strength == other.strength &&
			magic == other.magic &&
			skill == other.skill &&
			speed == other.speed &&
			luck == other.luck &&
			defense == other.defense &&
			movement == other.movement;
	}

	void MoveThroughPath(float deltaTime)
	{
		if (position == movementPath[currentPathGoalIndex + 1])
		{
			movementRate = 0.0f;
			currentPathGoalIndex++;
			if (movementPath[currentPathGoalIndex] == movementPath.back())
			{
				currentPathGoalIndex = 0;
				unitState = US_SELECTING_ACTION;
				return;
			}
			SetMovementDirection();
		}
		else
		{
			movementRate += movementSpeed * deltaTime;
			position = Vec2D::Lerp(movementPath[currentPathGoalIndex], movementPath[currentPathGoalIndex + 1], movementRate);
			return;
		}
	}

	void SetMovementDirection()
	{
		if (movementPath[currentPathGoalIndex].GetX() < movementPath[currentPathGoalIndex + 1].GetX()) movementDirection = UM_RIGHT;
		else if (movementPath[currentPathGoalIndex].GetX() > movementPath[currentPathGoalIndex + 1].GetX()) movementDirection = UM_LEFT;
		else if (movementPath[currentPathGoalIndex].GetY() < movementPath[currentPathGoalIndex + 1].GetY()) movementDirection = UM_DOWN;
		else if (movementPath[currentPathGoalIndex].GetY() > movementPath[currentPathGoalIndex + 1].GetY()) movementDirection = UM_UP;
	}

	void SetAttackDirection(const Vec2D& targetPosition)
	{
		if (targetPosition.GetX() < position.GetX()) movementDirection = UM_LEFT;
		else if (targetPosition.GetX() > position.GetX()) movementDirection = UM_RIGHT;
		else if (targetPosition.GetY() < position.GetY()) movementDirection = UM_UP;
		else if (targetPosition.GetY() > position.GetY()) movementDirection = UM_DOWN;
	}
};
