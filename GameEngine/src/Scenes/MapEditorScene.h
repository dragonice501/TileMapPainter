#pragma once

#include "../_App/Application.h"
#include "../Utils/MapEditorUtils.h"
#include "../Utils/Vec2D.h"

#include <SDL.h>
#include <stdint.h>
#include <vector>
#include <memory>

class MapEditorScene
{
public:
	MapEditorScene();
	~MapEditorScene();

	bool Init(SDL_Renderer* renderer);
	void Destroy();

	void Setup(SDL_Renderer* renderer);

	void Input();
	void Update(float deltaTime);
	void Render(SDL_Renderer* renderer);

	void DrawMap(SDL_Renderer* renderer);
	void DrawTileMap(SDL_Renderer* renderer);
	void DrawAnimatedSprites(SDL_Renderer* renderer);
	void DrawGUI();
	bool TileInsideCamera(uint16_t x, uint16_t y);
	bool CursorInGUI();

	void InitMap();
	void SetMapRectPositions();
	void SetMapSpriteIndeces();

	Vec2D GetCursorMapRect();
	void CheckCursorInMap();
	void CheckCursorInSpriteSheet();
	bool SquareContainsCursorPosition(const SDL_Rect& rect);
	void CopyMapRectSprite();
	void FillTile(uint16_t xIndex, uint16_t yIndex);

	// Unit Functions
	void PaintUnit(Vec2D position);
	void SetSelectedUnitClass(int unitSelectionIndex);
	void RemoveUnit(Vec2D position);
	void SaveUnits();
	void LoadUnits();
	void InitUnits();
	std::string GetUnitTypeName(EUnitClass unit);
	void SelectUnit(Vec2D position);


	// Select Functions
	void SetSelectionRect();
	void MoveSelectionUp();
	void MoveSelectionDown();
	void MoveSelectionLeft();
	void MoveSelectionRight();

	void SaveMap();
	void LoadMap();

	bool mIsRunning;

private:

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	uint32_t mWindowWidth = 1024;
	uint32_t mWindowHeight = 1024;

	uint32_t millisecondsPreviousFrame;

	// Editor Variables
	EEditorState mEditorState = EDITING_MAP;
	ESelectedTool mSelectedTool = PAN_TOOL;

	bool mMouseButtonDown = false;
	bool mShowOverlay = false;
	SDL_Texture* mOverlayTexture;

	Vec2D mCursorPosition;

	// Paint Variables
	uint16_t mSelectedSpriteIndex = 168;

	// Unit Variables
	std::vector<SDL_Texture*> mUnitClassTextures;
	std::vector<AnimatedUnitSprite> mAnimatedUnitSprites;
	int mSelectedUnitClassIndex = 0;
	AnimatedUnitSprite mSelectedMapUnit;
	EUnitClass mSelectedUnit = NONE;
	std::vector<AnimatedUnitSprite> mLoadedUnitSprites;
	std::vector<int> mLoadedUnits;
	std::vector<int> mLoadedUnitsXPositions;
	std::vector<int> mLoadedUnitsYPositions;
	std::vector<int> mLoadedUnitsHP;
	std::vector<int> mLoadedUnitsStrength;
	std::vector<int> mLoadedUnitsMagic;
	std::vector<int> mLoadedUnitsSkill;
	std::vector<int> mLoadedUnitsSpeed;
	std::vector<int> mLoadedUnitsLuck;
	std::vector<int> mLoadedUnitsDefense;
	std::vector<int> mLoadedUnitsModifier;

	int mNewUnitLevel = 1;
	int mNewUnitHP = 1;
	int mNewUnitStrength = 1;
	int mNewUnitMagic = 1;
	int mNewUnitSkill = 1;
	int mNewUnitSpeed = 1;
	int mNewUnitLuck = 1;
	int mNewUnitDefense = 1;
	int mNewUnitMovement = 1;

	// Select Tool Variables
	SDL_Rect mSelectionRect;
	Vec2D mSelectionRectStart;
	Vec2D mSelectionRectEnd;
	bool mShowTileSelection = false;
	uint16_t mSelectionXStart;
	uint16_t mSelectionYStart;
	uint16_t mSelectionWidth;
	uint16_t mSelectionHeight;

	// Map Variables
	float mMapXOffset = 0;
	float mMapYOffset = 0;
	uint16_t mMapWidth = 256;
	uint16_t mMapHeight = 256;
	float mMapZoom = 1.0f;
	const uint8_t MAX_MAP_SIZE = 512;
	const uint8_t SQUARE_PIXEL_SIZE = 16;
	const uint8_t SQUARE_RENDER_SIZE = 32;
	static const uint8_t SPRITE_SHEET_SIZE = 32;
	static const uint8_t TILE_MAP_WIDTH = 28;
	static const uint8_t TILE_MAP_HEIGHT = 24;

	SDL_Texture* mSpriteSheet;
	SDL_Texture* mTileMap[TILE_MAP_WIDTH][TILE_MAP_HEIGHT];
	SDL_Rect** mMapRects;
	SDL_Rect* mSpriteSheetRects[TILE_MAP_WIDTH][TILE_MAP_HEIGHT];
	uint16_t** mMapSpriteIndeces;

	std::vector<uint32_t> mLoadedSpriteIndeces;
};

