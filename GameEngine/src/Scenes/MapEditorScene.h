#pragma once

#include "../_App/Application.h"
#include "../Utils/MapEditorUtils.h"
#include "../Utils/Vec2D.h"

#include <SDL.h>
#include <stdint.h>
#include <vector>
#include <queue>
#include <map>

class MapEditorScene
{
public:
	MapEditorScene();
	~MapEditorScene();

	// Initialize
	bool Init(SDL_Renderer* renderer);
	void Setup(SDL_Renderer* renderer);

	void LoadMap();
	void LoadSceneEntrances();
	bool SaveMapExists();
	void SaveMap();

	// Destroy
	void Destroy();

	// Input
	void Input();
	void InputEditMode(const SDL_Event& sdlEvent, Vec2D& cursorMapPosition);
	void InputSelectingSpriteMode(const SDL_Event& sdlEvent, Vec2D& cursorMapPosition);
	void InputPlayMode(const SDL_Event& sdlEvent, Vec2D& cursorMapPosition);
	bool CheckCursorIsHoveringUnit(const Vec2D& cursorMapPosition);

	// Update
	void Update(const float& deltaTime);
	void UpdateEditor(const float& deltaTime);
	void UpdateGame(const float& deltaTime);

	// Render
	void Render(SDL_Renderer* renderer);
	void RenderEditorMode(SDL_Renderer* renderer);
	void RenderPlayMode(SDL_Renderer* renderer);
	void DrawMap(SDL_Renderer* renderer);
	void DrawTileSelection(SDL_Renderer* renderer);
	void DrawTileMap(SDL_Renderer* renderer);
	void DrawAnimatedSprites(SDL_Renderer* renderer);
	void DrawUnitHealthBars(SDL_Renderer* renderer, const AnimatedUnitSprite& playerUnit, const AnimatedUnitSprite& enemyUnit);
	void DrawSelectedUnitMovement(SDL_Renderer* renderer);
	void DrawSelectedUnitAttackRange(SDL_Renderer* renderer);
	void DrawStartPosition(SDL_Renderer* renderer);
	void DrawSceneEntrances(SDL_Renderer* renderer);
	void DrawGUI();
	void DrawHoveredUnitStats();
	Vec2D GetCursorToScreenRect();
	void DrawUnitActions();
	Vec2D GetUnitToScreenPosition(const Vec2D& unitMapPosition);

	bool TileInsideCamera(uint16_t x, uint16_t y);
	bool CursorInGUI();

	// Map Functions
	void InitMap();
	void SetMapRectPositions();
	void SetMapSpriteIndeces();
	void SetMapTerrainIndeces();
	ETerrainType GetTerrainType(uint32_t mapSpriteIndex);
	bool InMapBounds(const Vec2D& position);
	void IncreaseMapWidth();
	void DecreaseMapWidth();
	void IncreaseMapHeight();
	void DecreaseMapHeight();

	// SpriteSheet Functions
	void InitSpriteSheet();
	void ResetTools();

	// Tile Functions
	Vec2D GetCursorMapRect();
	bool CheckCursorInMap();
	void CheckCursorInSpriteSheet();
	bool SquareContainsCursorPosition(const SDL_Rect& rect);
	void CopyMapRectSprite();

	// Fill Function
	void FillTile(const Vec2D& start);

	// Unit Functions
	void PaintUnit(Vec2D position);
	void SetSelectedUnitClass(int& unitSelectionIndex);
	void SetUnitAttackType(int unitAttackType);
	void RemoveUnit(Vec2D position);
	void SaveUnits();
	void LoadUnits();
	void InitUnits();
	std::string GetUnitTypeName(EUnitClass unit);
	std::string GetUnitAttackTypeName(EAttackType type);
	void SelectUnit(Vec2D position);
	void ClearSelectedUnit();

	void TestDijkstra(const Vec2D& startPosition, const float& movement);
	void GetAttackPositions(const Vec2D& currentPosition);
	std::vector<Vec2D> DijkstraGetPath(const Vec2D& startPosition, const Vec2D& goalPosition, const float& movement);

	bool PositionAlreadyChecked(const Vec2D& position, const std::vector<Vec2D>& movementStack);

	void GetMovementPositions(const Vec2D& currentPosition, const float& movement);
	void CheckMovementPosition(const Vec2D& oldPosition, const Vec2D& newPosition, const float& movement, const EAttackDirection& direction);
	float GetTerrainMovementCost(const EUnitClass& unit, const ETerrainType& terrain);
	void CheckAttackPosition(const Vec2D& oldPosition, const Vec2D& newPosition, const int& attackRange, const EAttackDirection& direction);
	void SetAttackPositions(const Vec2D& attackingPosition, const EAttackType& attackType);
	void DeleteMovementPositionCopies();
	void DeleteAttackPositionCopies();

	void PrintTerrain(const ETerrainType& terrain);
	bool MovementsAlreadyContainsPosition(const Vec2D& position);
	bool CursorInSelectedUnitMovement(const Vec2D& mapPosition);
	bool SetUnitMovementPath(const Vec2D& destination);
	bool CheckMovementPath(const Vec2D& oldPosition, const Vec2D& newPosition, const Vec2D& destination, const float& movement);
	void ReverseMovementPath();
	bool EnemyInAttackRange(const Vec2D& startPosition, const EAttackType& attackType);
	bool UnitIsEnemy(const EUnitClass& unitClass);
	void SetUnitAttacks(AnimatedUnitSprite& playerUnit, AnimatedUnitSprite& enemyUnit);
	void ClearUnitAttacks(AnimatedUnitSprite& playerUnit, AnimatedUnitSprite& enemyUnit);
	void ClearActiveUnits();
	void DeleteUnit(const int& unitIndex);

	// Select Functions
	void MoveSelectionUp();
	void MoveSelectionDown();
	void MoveSelectionLeft();
	void MoveSelectionRight();

	// Game Functions
	void ApplyDamage(const AnimatedUnitSprite& attackingUnit, AnimatedUnitSprite& attackedUnit);

	bool mIsRunning;

private:
	// Editor Variables
	EEditorState mEditorState = ES_EDITING_MAP;
	ESelectedTool mSelectedTool = PAINT_TILE_TOOL;

	bool mMouseButtonDown = false;
	bool mShowOverlay = false;
	SDL_Texture* mOverlayTexture;

	Vec2D mCursorPosition;

	// ImGUI variables
	Vec2D mGUISize;
	bool mSaveMapExists = false;

	// Game variables
	EGameState mGameState = GS_PLAYER_IDLE;
	float waitTime = 1.0f;
	uint8_t dyingUnitBlend = 255;

	// Paint Variables
	uint16_t mSelectedSpriteIndex = 192;

	// Map Variables
	size_t mFileNameSize = 16;
	char mFileName[16];
	int mMapGUIWidth = 5;
	int mMapGUIHeight = 5;

	// RPG Variables
	bool mShowStartPosition = true;
	Vec2D mStartPosition;

	bool mShowSceneEntrances = true;
	int mSceneToLoadName = 0;
	int mSceneToLoadEntranceIndex = 0;
	std::vector<SceneEntrance> mSceneEntrances;

	std::vector<int> mLoadedSceneEntranceNames;
	std::vector<int> mLoadedSceneEntranceIndeces;
	std::vector<float> mLoadedSceneEntranceXs;
	std::vector<float> mLoadedSceneEntranceYs;

	// Unit Variables
	std::vector<SDL_Texture*> mUnitClassTextures;
	std::vector<AnimatedUnitSprite> mAnimatedUnitSprites;
	int mSelectedUnitClassIndex = 0;

	int mSelectedMapUnitIndex = -1;
	int mHoveredUnitIndex = -1;
	int mSelectedTargetUnitIndex = -1;

	bool mShowSelectedUnitMovement = false;
	bool mUnitHovered = false;

	std::vector<Vec2D> mMovementPositions;
	std::vector<Vec2D> mAttackPositions;
	std::vector<Vec2D> mUnitMovementPath;

	const float UNIT_HEALTH_BAR_X_OFFSET = 2.0f;
	const float UNIT_HEALTH_BAR_X_PADDING = 4.0f;
	const float UNIT_HEALTH_BAR_Y_OFFSET = 6.0f;
	const float UNIT_HEALTH_BAR_HEIGHT = 5.0f;

	// Unit Loading Variables
	std::vector<AnimatedUnitSprite> mLoadedUnitSprites;
	std::vector<int> mLoadedUnits;
	std::vector<int> mLoadedUnitsAttackTypes;
	std::vector<int> mLoadedUnitsXPositions;
	std::vector<int> mLoadedUnitsYPositions;
	std::vector<int> mLoadedUnitsLevel;
	std::vector<int> mLoadedUnitsMaxHP;
	std::vector<int> mLoadedUnitsCurrentHP;
	std::vector<int> mLoadedUnitsStrength;
	std::vector<int> mLoadedUnitsMagic;
	std::vector<int> mLoadedUnitsSkill;
	std::vector<int> mLoadedUnitsSpeed;
	std::vector<int> mLoadedUnitsLuck;
	std::vector<int> mLoadedUnitsDefense;
	std::vector<int> mLoadedUnitsMovement;

	// Paint Unit Tool Variables
	EUnitClass mNewSelectedUnit = NONE;
	EAttackType mNewUnitAttackType = AT_PHYSICAL;
	int mNewUnitLevel = 1;
	int mNewUnitMaxHP = 1;
	int mNewUnitStrength = 1;
	int mNewUnitMagic = 1;
	int mNewUnitSkill = 1;
	int mNewUnitSpeed = 1;
	int mNewUnitLuck = 1;
	int mNewUnitDefense = 1;
	int mNewUnitMovement = 1;

	// Tile Select Tool Variables
	Vec2D mSelectionRectStart;
	Vec2D mSelectionRectEnd;
	int mSelectionWidth;
	int mSelectionHeight;
	bool mShowTileSelection = false;

	// Fill Tile Tool Variables
	int mMaxFillRange = 20;

	// Map Variables
	float mMapXOffset = 0;
	float mMapYOffset = 0;
	uint16_t mMapWidth = 256;
	uint16_t mMapHeight = 256;
	float mMapZoom = 1.0f;
	const int MAX_MAP_SIZE = 256;
	const uint8_t SQUARE_PIXEL_SIZE = 16;
	const uint8_t SQUARE_RENDER_SIZE = 32;
	static const uint8_t SPRITE_SHEET_SIZE = 32;
	static const size_t TILE_MAP_SIZE = 32;

	SDL_Texture* mSpriteSheet;
	SDL_Texture* mTileMap[TILE_MAP_SIZE][TILE_MAP_SIZE];
	SDL_Rect** mMapRects;
	SDL_Rect* mSpriteSheetRects[TILE_MAP_SIZE][TILE_MAP_SIZE];
	uint16_t** mMapSpriteIndeces;
	ETerrainType** mMapTerrainIndeces;

	std::vector<uint32_t> mLoadedSpriteIndeces;

	Vec2D Directions[4] =
	{
		Vec2D(0.0f, -1.0f),
		Vec2D(1.0f, 0.0f),
		Vec2D(0.0f, 1.0f),
		Vec2D(-1.0f, 0.0f)
	};

	// Scenes
	const char* scenes[2] =
	{
		"World",
		"Town"
	};

	// Terrain Indeces
	uint16_t mRoadIndeces[48] =
	{
		64,65,66,67,68,69,70,72,74,75,76,77,96,97,98,99,100,101,103,104,105,107,108,109,128,129,130,
		131,132,133,134,135,136,137,138,139,140,141,160,161,162,163,164,165,166,167,169,172
	};

	uint16_t mBridgeIndeces[5] =
	{
		528,529,530,560,592
	};

	uint16_t mPlainIndeces[41] =
	{
		0,1,2,3,4,6,7,8,9,11,12,28,29,30,31,32,33,33,34,35,36,37,38,39,40,41,62,91,112,140,201,202,204,205,308,
		309,338,366,495,496,561
	};

	uint16_t mSandIndeces[17] =
	{
		448,449,453,454,455,456,457,480,481,485,486,487,488,489,490,491,492
	};

	uint16_t mForestIndeces[16] =
	{
		192,193,194,195,196,197,198,199,200,201,224,225,226,227,228,231
	};

	uint16_t mThicketIndeces[18] =
	{
		256,257,258,259,260,261,262,263,264,265,288,289,290,291,292,295,296,297
	};

	uint16_t mMountainIndeces[59] =
	{
		16,17,18,19,23,24,48,49,51,54,55,56,57,58,80,81,82,83,85,86,87,88,113,114,115,117,118,119,120,121,122,123,145,146,150,151,152,153,154,155,
		181,182,183,184,185,186,187,213,214,215,216,217,218,245,246,247,249,279,280
	};

	/*uint16_t mPeakIndeces[] =
	{

	};*/

	uint16_t mCliffIndeces[38] =
	{
		234,235,236,266,267,268,269,298,299,300,301,320,321,322,323,324,325,326
		,327,328,329,330,331,354,355,360,361,512,513,514,515,516,517,519,544,548,549,551
	};

	uint16_t mSeaIndeces[12] =
	{
		450,451,452,458,459,460,482,483,484,523,545,546
	};

	uint16_t mRiverIndeces[17] =
	{
		384,385,387,389,390,391,392,393,416,417,419,420,421,422,423,424,425
	};

	/*uint16_t mDesertIndeces[] =
	{

	};*/

	uint16_t mVillageIndeces[3] =
	{
		594,595,596
	};

	uint16_t mChurchIndeces[1] =
	{
		593
	};

	uint16_t mBragiTowerIndeces[3] =
	{
		625,657,689
	};

	uint16_t mCastleDefenseIndeces[4] =
	{
		531,532,624,656
	};

	uint16_t mCastleWallIndeces[35] =
	{
		534,535,537,538,565,566,567,568,569,570,597,598,599,600,601,602,661,
		662,664,665,666,692,693,694,695,696,697,698,756,757,758,759,760,761,762
	};
};

