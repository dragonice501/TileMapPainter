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
	void DrawSelectedUnitMovement(SDL_Renderer* renderer);
	void DrawSelectedUnitAttackRange(SDL_Renderer* renderer);
	void DrawGUI();

	bool TileInsideCamera(uint16_t x, uint16_t y);
	bool CursorInGUI();

	// Map Functions
	void InitMap();
	void SetMapRectPositions();
	void SetMapSpriteIndeces();
	void SetMapTerrainIndeces();
	ETerrainType GetTerrainType(uint32_t mapSpriteIndex);

	// SpriteShet Functions
	void InitSpriteSheet();

	void ResetTools();

	// Tile Functions
	Vec2D GetCursorMapRect();
	void CheckCursorInMap();
	void CheckCursorInSpriteSheet();
	bool SquareContainsCursorPosition(const SDL_Rect& rect);
	void CopyMapRectSprite();

	// Fill Function
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
	void GetMovementPositions(const Vec2D& currentPosition, const float& movement);
	void CheckMovementPosition(const Vec2D& oldPosition, const Vec2D& newPosition, const float& movement, const EAttackDirection& direction);
	float GetTerrainMovementCost(const EUnitClass& unit, const ETerrainType& terrain);
	void CheckAttackPosition(const Vec2D& oldPosition, const Vec2D& newPosition, const int& attackRange, const EAttackDirection& direction);
	void DeleteMovementPositionCopies();
	void DeleteAttackPositionCopies();
	void PrintTerrain(const ETerrainType& terrain);
	bool MovementsAlreadyContainsPosition(const Vec2D& position);
	bool CursorInSelectedUnitMovement(const Vec2D& mapPosition);

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
	bool mShowSelectedUnitMovement = false;
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

	std::vector<Vec2D> mMovementPositions;
	std::vector<Vec2D> mAttackPositions;

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
	ETerrainType** mMapTerrainIndeces;

	std::vector<uint32_t> mLoadedSpriteIndeces;

	// Terrain Indeces
	uint16_t mRoadIndeces[47] =
	{
		56,57,58,59,60,61,64,66,67,68,69,84,85,86,87,88,89,92,93,95,96,97,113,114,115,116,117,
		118,119,120,121,122,123,124,125,141,144,145,146,147,149,152,312,313,314,315,318
	};

	uint16_t mBridgeIndeces[5] =
	{
		529,530,531,561,593
	};

	uint16_t mPlainIndeces[40] =
	{
		0,1,2,3,4,6,7,8,9,11,12,28,29,30,31,32,33,33,34,35,36,37,38,39,40,41,62,91,112,140,201,202,204,205,308,
		309,338,366,495,496
	};

	uint16_t mSandIndeces[10] =
	{
		454,455,456,457,458,486,487,488,489,490
	};

	uint16_t mForestIndeces[16] =
	{
		168,169,170,171,172,173,174,175,176,177,196,197,198,199,200,203
	};

	uint16_t mThicketIndeces[18] =
	{
		224,225,226,227,228,229,230,231,232,233,252,253,254,255,256,259,260,261
	};

	uint16_t mMountainIndeces[58] =
	{
		16,17,18,19,23,24,44,45,47,50,51,52,53,54,72,73,74,75,77,78,79,80,101,102,103,105,106,107,108,109,110,
		111,129,130,134,135,136,138,139,161,162,163,164,165,166,167,189,190,191,192,193,194,217,218,219,221,247,248
	};

	/*uint16_t mPeakIndeces[] =
	{

	};*/

	uint16_t mCliffIndeces[38] =
	{
		235,236,237,267,268,269,270,299,300,301,302,321,322,323,324,325,326,327,
		328,329,330,332,355,356,513,514,515,516,517,518,520,521,522,545,549,550,552,555
	};

	uint16_t mSeaIndeces[4] =
	{
		402,403,404,459
	};

	uint16_t mRiverIndeces[17] =
	{
		336,364,365,337,339,367,368,369,341,342,370,371,343,344,372,373,345
	};

	/*uint16_t mDesertIndeces[] =
	{

	};*/

	uint16_t mVillageIndeces[3] =
	{
		522,523,524
	};

	uint16_t mChurchIndeces[1] =
	{
		594
	};

	uint16_t mBragiTowerIndeces[3] =
	{
		626,658,690
	};

	uint16_t mCastleDefenseIndeces[4] =
	{
		467,468,548,576
	};

	uint16_t mCastleWallIndeces[35] =
	{
		535,536,538,539,566,567,568,569,570,571,598,599,600,601,602,603,662,
		663,665,666,667,693,694,695,696,697,698,699,725,726,727,728,729,762,763
	};
};

