#pragma once

#include "../_App/Application.h"
#include "../ECS/ESC.h"
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

	bool Init(SDL_Renderer* renderer, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore);
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
	enum EEditorState
	{
		EDITING_MAP,
		SELECTING_SPRITE
	};

	enum ESelectedTool
	{
		PAINT_TILE_TOOL,
		FILL_TILE_TOOL,
		PAINT_UNIT_TOOL,
		PAN_TOOL,
		SELECT_TOOL
	};

	enum ETerrainType
	{
		UNDEFINED = 0,
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
		SWORD_ARMOUR
	};

	struct AnimatedSprite
	{
		uint8_t textureIndex = 0;
		uint8_t textureSize = 128;
		uint8_t frameSize = 32;
		uint8_t numFrames = 4;
		uint8_t frameRate = 4;
		uint8_t currentFrame = 0;
		uint32_t startTime = 0;
		Vec2D position = Vec2D::Zero;
		EUnitClass unitTexture = BOW_FIGHTER;
	};

	uint32_t mRoadIndeces[47] =
	{
		65,66,67,68,69,70,73,75,76,77,78,97,98,99,100,101,102,105,106,108,109,110,130,131,132,133,
		134,135,136,137,138,139,140,141,142,162,165,166,167,168,170,173,357,358,359,360,363
	};

	uint32_t mBridgeIndeces[5] =
	{
		529,530,531,561,593
	};

	uint32_t mPlainIndeces[42] =
	{
		1,2,3,4,5,7,8,9,10,12,13,33,34,35,36,37,38,39,40,41,42,43,44,45,
		46,129,161,163,164,353,354,387,419,230,231,233,234,353,354,562,564,565
	};

	uint32_t mSandIndeces[10] =
	{
		454,455,456,457,458,486,487,488,489,490
	};

	uint32_t mForestIndeces[17] =
	{
		193,194,195,196,197,198,198,199,200,201,202,225,226,227,228,229,232
	};

	uint32_t mThicketIndeces[18] =
	{
		257,258,259,260,261,262,263,264,265,266,289,290,291,292,293,296,297,298
	};

	uint32_t mMountainIndeces[58] =
	{
		17,18,19,20,24,25,49,50,52,55,56,57,58,59,81,82,83,84,86,87,88,89,114,115,116,118,119,120,
		121,122,123,124,146,147,151,152,153,155,156,182,183,184,185,186,187,188,214,215,216,217,218,219,246,247,248,250,280,281
	};

	/*uint32_t mPeakIndeces[] =
	{

	};*/

	uint32_t mCliffIndeces[38] =
	{
		235,236,237,267,268,269,270,299,300,301,302,321,322,323,324,325,326,327,
		328,329,330,332,355,356,513,514,515,516,517,518,520,521,522,545,549,550,552,555
	};

	/*uint32_t mSeaIndeces[] =
	{

	};*/

	uint32_t mRiverIndeces[17] =
	{
		385,386,388,390,391,392,393,394,417,418,420,421,422,423,424,425,426
	};

	/*uint32_t mDesertIndeces[] =
	{

	};*/

	uint32_t mVillageIndeces[3] =
	{
		595,596,597
	};

	uint32_t mChurchIndeces[1] =
	{
		594
	};

	uint32_t mBragiTowerIndeces[3] =
	{
		626,658,690
	};

	uint32_t mCastleDefenseIndeces[4] =
	{
		532,533,625,657
	};

	uint32_t mCastleWallIndeces[35] =
	{
		535,536,538,539,566,567,568,569,570,571,598,599,600,601,602,603,662,
		663,665,666,667,693,694,695,696,697,698,699,725,726,727,728,729,762,763
	};

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	std::unique_ptr<Registry> mRegistry;

	uint32_t mWindowWidth = 1024;
	uint32_t mWindowHeight = 1024;

	uint32_t millisecondsPreviousFrame;

	EEditorState mEditorState = EDITING_MAP;
	ESelectedTool mSelectedTool = PAINT_UNIT_TOOL;

	bool mMouseButtonDown = false;
	bool mShowOverlay = false;
	SDL_Texture* mOverlayTexture;

	Vec2D mCursorPosition;

	// Paint Variables
	uint16_t mSelectedSpriteIndex = 168;

	// Unit Variables
	std::vector<SDL_Texture*> mUnitClassTextures;
	std::vector<AnimatedSprite> mAnimatedSprites;
	int mSelectedUnitClassIndex = 0;
	AnimatedSprite mSelectedAnimatedSprite;
	EUnitClass mSelectedUnit = KNIGHT_LORD;

	// Select Tool Variables
	SDL_Rect mSelectionRect;
	Vec2D mSelectionRectStart;
	Vec2D mSelectionRectEnd;
	bool mShowSelection = false;
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

