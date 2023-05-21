#include "MapEditorScene.h"
#include "../_App/Application.h"
#include "../ECS/ESC.h"
#include "../Utils/FileCommandLoader.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <SDL_image.h>
#include <imgui.h>
#include <imgui_sdl.h>
#include <imgui_impl_sdl.h>

MapEditorScene::MapEditorScene() : mIsRunning(false)
{
	
}

MapEditorScene::~MapEditorScene()
{

}

bool MapEditorScene::Init(SDL_Renderer* renderer)
{
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, static_cast<int>(Application::GetWindowWidth()), static_cast<int>(Application::GetWindowHeight()));
	mIsRunning = true;

	Setup(renderer);

	return true;
}

void MapEditorScene::Destroy()
{
	ImGuiSDL::Deinitialize();
	ImGui::DestroyContext();

	SDL_DestroyTexture(mSpriteSheet);
	for (uint8_t y = 0; y < TILE_MAP_HEIGHT; y++)
	{
		for (uint8_t x = 0; x < TILE_MAP_WIDTH; x++)
		{
			SDL_DestroyTexture(mTileMap[x][y]);
		}
	}

	for (uint32_t i = 0; i < mMapWidth; i++)
	{
		delete[] mMapRects[i];
	}
	delete[] mMapRects;

	for (SDL_Texture* texture : mUnitClassTextures)
	{
		SDL_DestroyTexture(texture);
	}
	mUnitClassTextures.clear();
}

void MapEditorScene::Setup(SDL_Renderer* renderer)
{
	LoadMap();
	LoadUnits();
	//InitMap();
	InitSpriteSheet();

	SDL_Surface* surface = IMG_Load("./Assets/Chapter_0_m.png");
	mSpriteSheet = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("./Assets/world2.png");
	mOverlayTexture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	mUnitClassTextures.resize(EUnitClass::SWORD_ARMOUR + 1);

	surface = IMG_Load("./Assets/Bow_Fighter.png");
	SDL_Texture* bowFighterTexture = SDL_CreateTextureFromSurface(renderer, surface);
	mUnitClassTextures[BOW_FIGHTER] = bowFighterTexture;

	surface = IMG_Load("./Assets/Dancer.png");
	SDL_Texture* dancerTexture = SDL_CreateTextureFromSurface(renderer, surface);
	mUnitClassTextures[DANCER] = dancerTexture;

	surface = IMG_Load("./Assets/Sigurd.png");
	SDL_Texture* sigurdTexture = SDL_CreateTextureFromSurface(renderer, surface);
	mUnitClassTextures[KNIGHT_LORD] = sigurdTexture;

	surface = IMG_Load("./Assets/Mage.png");
	SDL_Texture* mageTexture = SDL_CreateTextureFromSurface(renderer, surface);
	mUnitClassTextures[MAGE] = mageTexture;

	surface = IMG_Load("./Assets/Sword_Armour.png");
	SDL_Texture* swordArmourTexture = SDL_CreateTextureFromSurface(renderer, surface);
	mUnitClassTextures[SWORD_ARMOUR] = swordArmourTexture;

	SDL_FreeSurface(surface);
}

void MapEditorScene::Input()
{
	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent))
	{
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		ImGuiIO& io = ImGui::GetIO();

		int mouseX, mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
		static bool modifier = false;

		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		//io.KeysDown[io.KeyMap[ImGuiKey_Backspace]] = SDL_BUTTON(SDL_SCANCODE_BACKSPACE);

		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		case SDL_KEYDOWN:
		{
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
			{
				mIsRunning = false;
			}
			else if (sdlEvent.key.keysym.sym == SDLK_SPACE)
			{
				modifier = true;
			}
			else if (sdlEvent.key.keysym.sym == SDLK_UP)
			{
				if (mSelectedTool == SELECT_TILE_TOOL)
				{
					MoveSelectionUp();
				}
			}
			else if (sdlEvent.key.keysym.sym == SDLK_RIGHT)
			{
				if (mSelectedTool == SELECT_TILE_TOOL)
				{
					MoveSelectionRight();
				}
			}
			else if (sdlEvent.key.keysym.sym == SDLK_LEFT)
			{
				if (mSelectedTool == SELECT_TILE_TOOL)
				{
					MoveSelectionLeft();
				}
			}
			else if (sdlEvent.key.keysym.sym == SDLK_DOWN)
			{
				if (mSelectedTool == SELECT_TILE_TOOL)
				{
					MoveSelectionDown();
				}
			}
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
			if (!CursorInGUI())
			{
				mMouseButtonDown = true;
				if (sdlEvent.button.button == SDL_BUTTON_LEFT)
				{
					if (mEditorState == EDITING_MAP)
					{
						Vec2D position = GetCursorMapRect();
						switch (mSelectedTool)
						{
						case PAN_TOOL:
							break;
						case PAINT_TILE_TOOL:
							CheckCursorInMap();
							break;
						case FILL_TILE_TOOL:
							//FillTile(static_cast<uint16_t>(position.GetX()), static_cast<uint16_t>(position.GetY()));
							break;
						case SELECT_TILE_TOOL:
							mSelectionRectStart = GetCursorMapRect();
							break;
						case PAINT_UNIT_TOOL:
							if (mSelectedUnit != NONE) PaintUnit(GetCursorMapRect());
							break;
						case SELECT_UNIT_TOOL:
							SelectUnit(GetCursorMapRect());
							break;
						default:
							break;
						}
					}
					else if (mEditorState == SELECTING_SPRITE)
					{
						mMouseButtonDown = false;
						CheckCursorInSpriteSheet();
					}
				}
				else if (sdlEvent.button.button == SDL_BUTTON_RIGHT)
				{
					switch (mSelectedTool)
					{
					case PAN_TOOL:
						break;
					case PAINT_TILE_TOOL:
						CopyMapRectSprite();
						break;
					case FILL_TILE_TOOL:
						CopyMapRectSprite();
						break;
					case SELECT_TILE_TOOL:
						mShowTileSelection = false;
						break;
					case PAINT_UNIT_TOOL:
						RemoveUnit(GetCursorMapRect());
						break;
					default:
						break;
					}
				}
			}
			break;
		case SDL_MOUSEBUTTONUP:
		{
			mMouseButtonDown = false;
			if (mSelectedTool == SELECT_TILE_TOOL)
			{
				if (sdlEvent.button.button == SDL_BUTTON_LEFT)
				{
					mSelectionRectEnd = GetCursorMapRect();
					SetSelectionRect();
				}
				else if (sdlEvent.button.button == SDL_BUTTON_RIGHT)
				{
					mShowTileSelection = false;
				}
			}
			break;
		}
		case SDL_MOUSEMOTION:
		{
			mCursorPosition = Vec2D(sdlEvent.motion.x, sdlEvent.motion.y);
			GetCursorMapRect();
			if (mMouseButtonDown && !CursorInGUI())
			{
				switch (mSelectedTool)
				{
				case PAINT_TILE_TOOL:
					break;
				case PAN_TOOL:
					mMapXOffset += sdlEvent.motion.xrel;
					mMapYOffset += sdlEvent.motion.yrel;
					SetMapRectPositions();
					break;
				case SELECT_TILE_TOOL:
					break;
				default:
					break;
				}
			}
			break;
		}
		default:
			break;
		}
	}
}

void MapEditorScene::Update(float deltaTime)
{
	if (mMouseButtonDown && mSelectedTool == PAINT_TILE_TOOL)
	{
		CheckCursorInMap();
	}

	for (AnimatedUnitSprite& sprite : mAnimatedUnitSprites)
	{
		sprite.currentFrame =
			static_cast<int>(((SDL_GetTicks() - sprite.startTime) * sprite.frameRate / 1000.0f)) % sprite.numFrames;
	}
}

void MapEditorScene::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	switch (mEditorState)
	{
	case EDITING_MAP:
		DrawMap(renderer);
		if (mShowSelectedUnitMovement)
		{
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
			//SDL_RenderFillRect(renderer, &mSelectionRect);

			DrawSelectedUnitMovement(renderer);

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
			DrawSelectedUnitAttackRange(renderer);
		}
		if (mAnimatedUnitSprites.size() > 0)
		{
			DrawAnimatedSprites(renderer);
		}
		break;
	case SELECTING_SPRITE:
		DrawTileMap(renderer);
		break;
	default:
		break;
	}

	DrawGUI();

	SDL_RenderPresent(renderer);
}

void MapEditorScene::DrawMap(SDL_Renderer* renderer)
{
	for (uint32_t y = 0; y < mMapHeight; y++)
	{
		for (uint32_t x = 0; x < mMapWidth; x++)
		{
			if (TileInsideCamera(x, y))
			{
				SDL_Rect srcRect = {
					(mMapSpriteIndeces[x][y] % TILE_MAP_WIDTH) << 4,
					(mMapSpriteIndeces[x][y] / TILE_MAP_WIDTH) << 4,
					SQUARE_PIXEL_SIZE,
					SQUARE_PIXEL_SIZE };

				SDL_Rect dstRect = mMapRects[x][y];

				SDL_RenderCopy(renderer, mSpriteSheet, &srcRect, &dstRect);
			}
		}
	}

	if (mShowOverlay)
	{
		Vec2D position = {
				static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset)),
				static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset)) };

		SDL_Rect overlayRect = {
			static_cast<int>(position.GetX()),
			static_cast<int>(position.GetY()),
			4096 * 2 * mMapZoom,
			4096 * 2 * mMapZoom };

		SDL_SetTextureBlendMode(mOverlayTexture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(mOverlayTexture, 100);
		SDL_RenderCopy(renderer, mOverlayTexture, NULL, &overlayRect);
	}
}

void MapEditorScene::DrawTileMap(SDL_Renderer* renderer)
{
	for (uint8_t y = 0; y < TILE_MAP_HEIGHT; y++)
	{
		for (uint8_t x = 0; x < TILE_MAP_WIDTH; x++)
		{
			Vec2D position = {
				static_cast<float>((Application::GetWindowWidth() / 2 - (TILE_MAP_WIDTH * SQUARE_RENDER_SIZE) / 2 + x * SQUARE_RENDER_SIZE)),
				static_cast<float>((Application::GetWindowHeight() / 2 - (TILE_MAP_HEIGHT * SQUARE_RENDER_SIZE) / 2 + y * SQUARE_RENDER_SIZE)) };

			SDL_Rect srcRect = { x * SQUARE_PIXEL_SIZE, y * SQUARE_PIXEL_SIZE, SQUARE_PIXEL_SIZE, SQUARE_PIXEL_SIZE };
			SDL_Rect dstRect = { position.GetX(), position.GetY(), SQUARE_RENDER_SIZE, SQUARE_RENDER_SIZE };

			SDL_RenderCopy(renderer, mSpriteSheet, &srcRect, &dstRect);
		}
	}
}

void MapEditorScene::DrawAnimatedSprites(SDL_Renderer* renderer)
{
	for (AnimatedUnitSprite& sprite : mAnimatedUnitSprites)
	{
		Vec2D position = {
				static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + sprite.position.GetX() * SQUARE_RENDER_SIZE * mMapZoom)),
				static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + sprite.position.GetY() * SQUARE_RENDER_SIZE * mMapZoom)) };

		SDL_Rect srcRect = { sprite.frameSize * sprite.currentFrame, 0, sprite.frameSize, sprite.frameSize };
		SDL_Rect dstRect = { position.GetX(), position.GetY() - 32 * mMapZoom, sprite.frameSize * 2 * mMapZoom, sprite.frameSize * 2 * mMapZoom };

		SDL_RenderCopy(renderer, mUnitClassTextures[sprite.unitTexture], &srcRect, &dstRect);
	}
}

void MapEditorScene::DrawSelectedUnitMovement(SDL_Renderer* renderer)
{
	for (const Vec2D& position : mMovementPositions)
	{
		Vec2D drawPosition = {
				static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + position.GetX() * SQUARE_RENDER_SIZE * mMapZoom)),
				static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + position.GetY() * SQUARE_RENDER_SIZE * mMapZoom)) };

		SDL_Rect rect = { drawPosition.GetX(), drawPosition.GetY(), SQUARE_RENDER_SIZE, SQUARE_RENDER_SIZE };

		SDL_RenderFillRect(renderer, &rect);
	}
}

void MapEditorScene::DrawSelectedUnitAttackRange(SDL_Renderer* renderer)
{
	for (const Vec2D& position : mAttackPositions)
	{
		Vec2D drawPosition = {
				static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + position.GetX() * SQUARE_RENDER_SIZE * mMapZoom)),
				static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + position.GetY() * SQUARE_RENDER_SIZE * mMapZoom)) };

		SDL_Rect rect = { drawPosition.GetX(), drawPosition.GetY(), SQUARE_RENDER_SIZE, SQUARE_RENDER_SIZE };

		SDL_RenderFillRect(renderer, &rect);
	}
}

void MapEditorScene::DrawGUI()
{
	static int mapXOffset = mMapXOffset;
	static int mapYOffset = mMapYOffset;
	static float zoomLevel = mMapZoom;

	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;

	if (ImGui::Begin("Map Editor", NULL, windowFlags))
	{
		if (ImGui::CollapsingHeader("Map Settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			Vec2D cursorRect = GetCursorMapRect();
			ImGui::Text("Mouse coordinates (x=%.1f, y=%.1f)", static_cast<float>(mCursorPosition.GetX()), static_cast<float>(mCursorPosition.GetY()));
			ImGui::Text("Map coordinates (x=%.1f, y=%.1f)", static_cast<float>(cursorRect.GetX()), static_cast<float>(cursorRect.GetY()));
			ImGui::Text("Map offset (x=%.1f, y=%.1f)", -mMapXOffset, -mMapYOffset);

			static char mapName[16];
			ImGui::InputText("File Name", mapName, IM_ARRAYSIZE(mapName));

			if (ImGui::Button("New Map"))
			{
				InitMap();
				mShowOverlay = false;
				mShowTileSelection = false;

				mAnimatedUnitSprites.clear();
			}

			if (ImGui::Button("Save Map"))
			{
				SaveMap();
				SaveUnits();
			}

			if (ImGui::Button("Load Map"))
			{
				LoadMap();
				LoadUnits();
			}
		}

		if (ImGui::CollapsingHeader("Map Tools", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::SliderFloat("Zoom", &zoomLevel, 0.1f, 3.0f))
			{
				mMouseButtonDown = false;
				mMapZoom = zoomLevel;
				SetMapRectPositions();
			}
			if (ImGui::Button("Reset Zoom"))
			{
				zoomLevel = 1;
				mMapZoom = zoomLevel;
				SetMapRectPositions();
			}
			if (ImGui::Button("Recenter Map"))
			{
				mMapXOffset = 0;
				mMapYOffset = 0;
				SetMapRectPositions();
			}
			if (ImGui::Button("Pan"))
			{
				mSelectedTool = PAN_TOOL;
				ResetTools();
			}
			if (ImGui::Button("Show Overlay"))
			{
				mShowOverlay = !mShowOverlay;
			}
		}

		if (ImGui::CollapsingHeader("Tile Tools", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Tile Map"))
			{
				if (mEditorState == EDITING_MAP) mEditorState = SELECTING_SPRITE;
				else mEditorState = EDITING_MAP;
			}
			if (ImGui::Button("Paint Tile"))
			{
				mSelectedTool = PAINT_TILE_TOOL;
				ResetTools();
			}
			if (ImGui::Button("Fill Tile"))
			{
				mSelectedTool = FILL_TILE_TOOL;
				ResetTools();
			}
			if (ImGui::Button("Select Tile"))
			{
				mSelectedTool = SELECT_TILE_TOOL;
				ResetTools();
			}
		}

		if (ImGui::CollapsingHeader("Unit Tools", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Paint Unit"))
			{
				mSelectedTool = PAINT_UNIT_TOOL;
				ResetTools();
			}
			if (ImGui::Button("Select Unit"))
			{
				mSelectedTool = SELECT_UNIT_TOOL;
				ResetTools();
			}
			if (ImGui::Button("Reset Stats"))
			{
				mSelectedUnit = NONE;
				mNewUnitLevel = 1;
				mNewUnitHP = 1;
				mNewUnitStrength = 1;
				mNewUnitMagic = 1;
				mNewUnitSkill = 1;
				mNewUnitSpeed = 1;
				mNewUnitLuck = 1;
				mNewUnitDefense = 1;
				mNewUnitMovement = 1;
			}

			static int selectedUnitClassIndex = mSelectedUnit;
			const char* classes[] =
			{
				"Bow Fighter", "Dancer", "Knight Lord", "Mage", "Sword Armour", "None"
			};
			if (ImGui::Combo("Class", &selectedUnitClassIndex, classes, IM_ARRAYSIZE(classes)))
			{
				SetSelectedUnitClass(selectedUnitClassIndex);
			}
			
			ImGui::InputInt("Level", &mNewUnitLevel);
			ImGui::InputInt("HP", &mNewUnitHP);
			ImGui::InputInt("Strength", &mNewUnitStrength);
			ImGui::InputInt("Magic", &mNewUnitMagic);
			ImGui::InputInt("Skill", &mNewUnitSkill);
			ImGui::InputInt("Speed", &mNewUnitSpeed);
			ImGui::InputInt("Luck", &mNewUnitLuck);
			ImGui::InputInt("Defense", &mNewUnitDefense);
			ImGui::InputInt("Movement", &mNewUnitMovement);

			std::string unitString = "Selected Unit: " + GetUnitTypeName(mSelectedMapUnit.unitTexture);
			std::string levelString = "Selected Unit Level: " + std::to_string(mSelectedMapUnit.level);
			std::string hpString = "Selected Unit HP: " + std::to_string(mSelectedMapUnit.hp);
			std::string strengthString = "Selected Unit Strength: " + std::to_string(mSelectedMapUnit.strength);
			std::string magicString = "Selected Unit magic: " + std::to_string(mSelectedMapUnit.magic);
			std::string skillString = "Selected Unit Skill: " + std::to_string(mSelectedMapUnit.skill);
			std::string speedString = "Selected Unit Speed: " + std::to_string(mSelectedMapUnit.speed);
			std::string luckString = "Selected Unit Luck: " + std::to_string(mSelectedMapUnit.luck);
			std::string defenseString = "Selected Unit Defense: " + std::to_string(mSelectedMapUnit.defense);
			std::string modifierString = "Selected Unit Movement: " + std::to_string(mSelectedMapUnit.movement);

			ImGui::Text(unitString.c_str());
			ImGui::Text(levelString.c_str());
			ImGui::Text(hpString.c_str());
			ImGui::Text(strengthString.c_str());
			ImGui::Text(magicString.c_str());
			ImGui::Text(skillString.c_str());
			ImGui::Text(speedString.c_str());
			ImGui::Text(luckString.c_str());
			ImGui::Text(defenseString.c_str());
			ImGui::Text(modifierString.c_str());
		}
	}
	ImGui::End();

	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
}

bool MapEditorScene::TileInsideCamera(uint16_t x, uint16_t y)
{
	Vec2D position = {
				static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + x * SQUARE_RENDER_SIZE * mMapZoom)),
				static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + y * SQUARE_RENDER_SIZE * mMapZoom)) };

	return
		position.GetX() <= Application::GetWindowWidth() &&
		position.GetX() + SQUARE_RENDER_SIZE * mMapZoom >= 0 &&
		position.GetY() <= Application::GetWindowHeight() &&
		position.GetY() + SQUARE_RENDER_SIZE * mMapZoom >= 0;
}

bool MapEditorScene::CursorInGUI()
{
	return
		mCursorPosition.GetX() < 283 &&
		mCursorPosition.GetY() < 728;
}

void MapEditorScene::InitMap()
{
	mMapRects = new SDL_Rect * [mMapWidth];
	mMapSpriteIndeces = new uint16_t * [mMapWidth];
	mMapTerrainIndeces = new ETerrainType * [mMapWidth];
	for (uint16_t i = 0; i < mMapWidth; i++)
	{
		mMapRects[i] = new SDL_Rect[mMapHeight];
		mMapSpriteIndeces[i] = new uint16_t[mMapHeight];
		mMapTerrainIndeces[i] = new ETerrainType[mMapHeight];
	}

	SetMapRectPositions();
	SetMapSpriteIndeces();
	SetMapTerrainIndeces();
}

void MapEditorScene::SetMapRectPositions()
{
	for (uint16_t y = 0; y < mMapHeight; y++)
	{
		for (uint16_t x = 0; x < mMapWidth; x++)
		{
			Vec2D position = {
				static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + x * SQUARE_RENDER_SIZE * mMapZoom)),
				static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + y * SQUARE_RENDER_SIZE * mMapZoom)) };

			mMapRects[x][y] = SDL_Rect{
				static_cast<int>(position.GetX()),
				static_cast<int>(position.GetY()),
				static_cast<int>(static_cast<float>(SQUARE_RENDER_SIZE) * mMapZoom),
				static_cast<int>(static_cast<float>(SQUARE_RENDER_SIZE) * mMapZoom) };
		}
	}
}

void MapEditorScene::SetMapSpriteIndeces()
{
	if (mLoadedSpriteIndeces.size() > 0)
	{
		for (uint32_t y = 0; y < mMapHeight; y++)
		{
			for (uint32_t x = 0; x < mMapWidth; x++)
			{
				mMapSpriteIndeces[x][y] = mLoadedSpriteIndeces[x + y * mMapWidth];
			}
		}

		mLoadedSpriteIndeces.clear();
	}
	else
	{
		for (uint16_t y = 0; y < mMapHeight; y++)
		{
			for (uint16_t x = 0; x < mMapWidth; x++)
			{
				mMapSpriteIndeces[x][y] = 0;
			}
		}
	}
}

void MapEditorScene::SetMapTerrainIndeces()
{
	for (uint16_t y = 0; y < mMapHeight; y++)
	{
		for (uint16_t x = 0; x < mMapWidth; x++)
		{
			mMapTerrainIndeces[x][y] = GetTerrainType(static_cast<uint32_t>(mMapSpriteIndeces[x][y]));
			/*if (x >= 111 && x <= 131 && y >= 210 && y <= 213)
			{
				std::cout << static_cast<int>(x) << ',' << static_cast<int>(y) << ' ';
				PrintTerrain(mMapTerrainIndeces[x][y]);
			}*/
		}
	}
}

ETerrainType MapEditorScene::GetTerrainType(uint32_t mapSpriteIndex)
{
	for (const uint32_t& index : mSeaIndeces)
	{
		if (mapSpriteIndex == index) return SEA;
	}

	for (const uint32_t& index : mRiverIndeces)
	{
		if (mapSpriteIndex == index) return RIVER;
	}

	for (const uint32_t& index : mRoadIndeces)
	{
		if (mapSpriteIndex == index) return ROAD;
	}

	for (const uint32_t& index : mPlainIndeces)
	{
		if (mapSpriteIndex == index) return PLAIN;
	}

	for (const uint32_t& index : mForestIndeces)
	{
		if (mapSpriteIndex == index) return FOREST;
	}

	for (const uint32_t& index : mMountainIndeces)
	{
		if (mapSpriteIndex == index) return MOUNTAIN;
	}

	for (const uint32_t& index : mVillageIndeces)
	{
		if (mapSpriteIndex == index) return VILLAGE;
	}

	for (const uint32_t& index : mCastleDefenseIndeces)
	{
		if (mapSpriteIndex == index) return CASTLE_DEFENSE;
	}

	return UNDEFINED;
}

void MapEditorScene::InitSpriteSheet()
{
	for (uint8_t y = 0; y < TILE_MAP_HEIGHT; y++)
	{
		for (uint8_t x = 0; x < TILE_MAP_WIDTH; x++)
		{
			Vec2D position = {
				static_cast<float>((Application::GetWindowWidth() / 2 - ((TILE_MAP_WIDTH * SQUARE_RENDER_SIZE) / 2) + x * SQUARE_RENDER_SIZE )),
				static_cast<float>((Application::GetWindowHeight() / 2 - ((TILE_MAP_HEIGHT * SQUARE_RENDER_SIZE) / 2) + y * SQUARE_RENDER_SIZE * mMapZoom)) };

			mSpriteSheetRects[x][y] = new SDL_Rect{
				static_cast<int>(position.GetX()),
				static_cast<int>(position.GetY()),
				static_cast<int>(static_cast<float>(SQUARE_RENDER_SIZE) * mMapZoom),
				static_cast<int>(static_cast<float>(SQUARE_RENDER_SIZE) * mMapZoom) };
		}
	}
}

void MapEditorScene::ResetTools()
{
	mShowTileSelection = false;
	mShowSelectedUnitMovement = false;
	mShowOverlay = false;
}

Vec2D MapEditorScene::GetCursorMapRect()
{
	for (uint16_t y = 0; y < mMapHeight; y++)
	{
		for (uint16_t x = 0; x < mMapWidth; x++)
		{
			if (SquareContainsCursorPosition(mMapRects[x][y]))
			{
				return Vec2D(x, y);
			}
		}
	}
}

void MapEditorScene::CheckCursorInMap()
{
	for (uint16_t y = 0; y < mMapHeight; y++)
	{
		for (uint16_t x = 0; x < mMapWidth; x++)
		{
			if (SquareContainsCursorPosition(mMapRects[x][y]))
			{
				mMapSpriteIndeces[x][y] = mSelectedSpriteIndex;
				return;
			}
		}
	}
}

void MapEditorScene::CheckCursorInSpriteSheet()
{
	for (uint8_t y = 0; y < TILE_MAP_HEIGHT; y++)
	{
		for (uint8_t x = 0; x < TILE_MAP_WIDTH; x++)
		{
			if (SquareContainsCursorPosition(*mSpriteSheetRects[x][y]))
			{
				mSelectedSpriteIndex = x % TILE_MAP_WIDTH + y * TILE_MAP_WIDTH;
				mEditorState = EDITING_MAP;
				std::cout << mSelectedSpriteIndex << ',';
				return;
			}
		}
	}
}

bool MapEditorScene::SquareContainsCursorPosition(const SDL_Rect& rect)
{
	return
		mCursorPosition.GetX() >= rect.x &&
		mCursorPosition.GetX() <= rect.x + rect.w &&
		mCursorPosition.GetY() >= rect.y &&
		mCursorPosition.GetY() <= rect.y + rect.h;
}

void MapEditorScene::CopyMapRectSprite()
{
	for (uint32_t y = 0; y < mMapHeight; y++)
	{
		for (uint32_t x = 0; x < mMapWidth; x++)
		{
			if (SquareContainsCursorPosition(mMapRects[x][y]))
			{
				mSelectedSpriteIndex = mMapSpriteIndeces[x][y];
				return;
			}
		}
	}
}

void MapEditorScene::FillTile(uint16_t xIndex, uint16_t yIndex)
{
	mMapSpriteIndeces[xIndex][yIndex] = mSelectedSpriteIndex;

	if (yIndex - 1 >= 0)
	{
		if (mMapSpriteIndeces[xIndex][yIndex - 1] != mSelectedSpriteIndex) FillTile(xIndex, yIndex - 1);
	}
	if (yIndex + 1 < mMapHeight)
	{
		if (mMapSpriteIndeces[xIndex][yIndex + 1] != mSelectedSpriteIndex) FillTile(xIndex, yIndex + 1);
	}
	if (xIndex - 1 >= 0)
	{
		if (mMapSpriteIndeces[xIndex - 1][yIndex] != mSelectedSpriteIndex) FillTile(xIndex - 1, yIndex);
	}
	if (xIndex + 1 < mMapWidth)
	{
		if (mMapSpriteIndeces[xIndex + 1][yIndex] != mSelectedSpriteIndex) FillTile(xIndex + 1, yIndex);
	}
}

void MapEditorScene::PaintUnit(Vec2D position)
{
	if (mAnimatedUnitSprites.size() > 0)
	{
		for (const AnimatedUnitSprite& sprite : mAnimatedUnitSprites)
		{
			if (sprite.position == position)
			{
				return;
			}
		}
	}

	AnimatedUnitSprite animatedUnitSprite;
	animatedUnitSprite.position = position;
	animatedUnitSprite.unitTexture = mSelectedUnit;
	animatedUnitSprite.startTime = SDL_GetTicks();

	animatedUnitSprite.level = mNewUnitLevel;
	animatedUnitSprite.hp = mNewUnitHP;
	animatedUnitSprite.strength = mNewUnitStrength;
	animatedUnitSprite.skill = mNewUnitSkill;
	animatedUnitSprite.speed = mNewUnitSpeed;
	animatedUnitSprite.luck = mNewUnitLuck;
	animatedUnitSprite.defense = mNewUnitDefense;
	animatedUnitSprite.movement = mNewUnitMovement;

	mSelectedMapUnit = animatedUnitSprite;

	mAnimatedUnitSprites.push_back(animatedUnitSprite);
}

void MapEditorScene::SetSelectedUnitClass(int unitSelectionIndex)
{
	EUnitClass unit = static_cast<EUnitClass>(unitSelectionIndex);

	switch (unit)
	{
	case BOW_FIGHTER:
		mSelectedUnit = BOW_FIGHTER;
		break;
	case DANCER:
		mSelectedUnit = DANCER;
		break;
	case KNIGHT_LORD:
		mSelectedUnit = KNIGHT_LORD;
		break;
	case MAGE:
		mSelectedUnit = MAGE;
		break;
	case SWORD_ARMOUR:
		mSelectedUnit = SWORD_ARMOUR;
		break;
	case NONE:
		mSelectedUnit = NONE;
		break;
	default:
		break;
	}
}

void MapEditorScene::RemoveUnit(Vec2D position)
{
	for (int i = 0; i < mAnimatedUnitSprites.size(); i++)
	{
		if (mAnimatedUnitSprites[i].position == position)
		{
			AnimatedUnitSprite temp = mAnimatedUnitSprites[i];
			mAnimatedUnitSprites[i] = mAnimatedUnitSprites.back();
			mAnimatedUnitSprites.back() = temp;
			mAnimatedUnitSprites.pop_back();
			return;
		}
	}

	mSelectedMapUnit = AnimatedUnitSprite();
}

void MapEditorScene::SaveUnits()
{
	std::string unitsPath = "./Assets/UnitsSaveFile.txt";
	std::ifstream unitsInFile;
	unitsInFile.open(unitsPath);
	if (unitsInFile.is_open())
	{
		unitsInFile.close();
		std::remove(unitsPath.c_str());
	}

	std::ofstream unitOutFile;
	unitOutFile.open(unitsPath);
	if (unitOutFile.is_open())
	{
		for (AnimatedUnitSprite& uint : mAnimatedUnitSprites)
		{
			unitOutFile << ":unit " + std::to_string(uint.unitTexture) << std::endl;
			unitOutFile << ":x " + std::to_string(static_cast<int>(uint.position.GetX())) << std::endl;
			unitOutFile << ":y " + std::to_string(static_cast<int>(uint.position.GetY())) << std::endl;
			unitOutFile << ":level " + std::to_string(static_cast<int>(uint.level)) << std::endl;
			unitOutFile << ":hp " + std::to_string(static_cast<int>(uint.hp)) << std::endl;
			unitOutFile << ":strength " + std::to_string(static_cast<int>(uint.strength)) << std::endl;
			unitOutFile << ":magic " + std::to_string(static_cast<int>(uint.magic)) << std::endl;
			unitOutFile << ":skill " + std::to_string(static_cast<int>(uint.skill)) << std::endl;
			unitOutFile << ":speed " + std::to_string(static_cast<int>(uint.speed)) << std::endl;
			unitOutFile << ":luck " + std::to_string(static_cast<int>(uint.luck)) << std::endl;
			unitOutFile << ":defense " + std::to_string(static_cast<int>(uint.defense)) << std::endl;
			unitOutFile << ":modifier " + std::to_string(static_cast<int>(uint.movement)) << std::endl << std::endl;
		}
	}
	unitOutFile.close();
}

void MapEditorScene::LoadUnits()
{
	std::string unitsFilePath = "./Assets/UnitsSaveFile.txt";
	std::ifstream unitsInFile;
	unitsInFile.open(unitsFilePath);
	if (unitsInFile.is_open())
	{
		FileCommandLoader fileLoader;

		Command unitTypeCommand;
		unitTypeCommand.command = "unit";
		unitTypeCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnits.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitTypeCommand);

		Command unitXPositionCommand;
		unitXPositionCommand.command = "x";
		unitXPositionCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsXPositions.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitXPositionCommand);

		Command unitYPositionCommand;
		unitYPositionCommand.command = "y";
		unitYPositionCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsYPositions.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitYPositionCommand);

		Command unitHPCommand;
		unitHPCommand.command = "hp";
		unitHPCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsHP.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitHPCommand);

		Command unitStengthCommand;
		unitStengthCommand.command = "strength";
		unitStengthCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsStrength.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitStengthCommand);

		Command unitMagicCommand;
		unitMagicCommand.command = "magic";
		unitMagicCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsMagic.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitMagicCommand);

		Command unitsSkillCommand;
		unitsSkillCommand.command = "skill";
		unitsSkillCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsSkill.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitsSkillCommand);

		Command unitSpeedCommand;
		unitSpeedCommand.command = "speed";
		unitSpeedCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsSpeed.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitSpeedCommand);

		Command unitLuckCommand;
		unitLuckCommand.command = "luck";
		unitLuckCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsLuck.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitLuckCommand);

		Command unitDefenseCommand;
		unitDefenseCommand.command = "defense";
		unitDefenseCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsDefense.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitDefenseCommand);

		Command unitModifierCommand;
		unitModifierCommand.command = "modifier";
		unitModifierCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedUnitsModifier.push_back(FileCommandLoader::ReadInt(params));
		};
		fileLoader.AddCommand(unitModifierCommand);

		fileLoader.LoadFile(unitsFilePath);
	}

	unitsInFile.close();

	InitUnits();
}

void MapEditorScene::InitUnits()
{
	mAnimatedUnitSprites.clear();

	AnimatedUnitSprite newUnit;
	for (int i = 0; i < mLoadedUnits.size(); i++)
	{
		newUnit.unitTexture = static_cast<EUnitClass>(mLoadedUnits[i]);
		newUnit.position = { static_cast<float>(mLoadedUnitsXPositions[i]), static_cast<float>(mLoadedUnitsYPositions[i]) };
		newUnit.startTime = SDL_GetTicks();

		newUnit.hp = mLoadedUnitsHP[i];
		newUnit.strength = mLoadedUnitsStrength[i];
		newUnit.magic = mLoadedUnitsMagic[i];
		newUnit.skill = mLoadedUnitsSkill[i];
		newUnit.speed = mLoadedUnitsSpeed[i];
		newUnit.luck = mLoadedUnitsLuck[i];
		newUnit.defense = mLoadedUnitsDefense[i];
		newUnit.movement = mLoadedUnitsModifier[i];

		mAnimatedUnitSprites.push_back(newUnit);
	}

	mLoadedUnits.clear();
	mLoadedUnitsXPositions.clear();
	mLoadedUnitsYPositions.clear();
	mLoadedUnitsHP.clear();
	mLoadedUnitsStrength.clear();
	mLoadedUnitsMagic.clear();
	mLoadedUnitsSkill.clear();
	mLoadedUnitsSpeed.clear();
	mLoadedUnitsLuck.clear();
	mLoadedUnitsDefense.clear();
	mLoadedUnitsModifier.clear();
}

std::string MapEditorScene::GetUnitTypeName(EUnitClass unit)
{
	switch (unit)
	{
	case BOW_FIGHTER:
		return "Bow Fighter";
		break;
	case DANCER:
		return "Dancer";
		break;
	case KNIGHT_LORD:
		return "Knight Lord";
		break;
	case MAGE:
		return "Mage";
		break;
	case SWORD_ARMOUR:
		return "Sword Armour";
		break;
	case NONE:
		return "";
		break;
	default:
		return "";
		break;
	}

	return "";
}

void MapEditorScene::SelectUnit(Vec2D position)
{
	for (const AnimatedUnitSprite& sprite : mAnimatedUnitSprites)
	{
		if (sprite.position == position)
		{
			if (mSelectedMapUnit != sprite)
			{
				mSelectedMapUnit = sprite;
				mShowSelectedUnitMovement = true;
				mMovementPositions.clear();
				mAttackPositions.clear();

				mMovementPositions.push_back(mSelectedMapUnit.position);
				GetMovementPositions(mSelectedMapUnit.position, static_cast<float>(mSelectedMapUnit.movement));
				DeleteMovementPositionCopies();
				
				DeleteAttackPositionCopies();
				return;
			}
			else if (mSelectedMapUnit == sprite) return;
		}
	}

	mMovementPositions.clear();
	mAttackPositions.clear();
	mShowSelectedUnitMovement = false;
	mSelectedMapUnit = AnimatedUnitSprite();
}

void MapEditorScene::GetMovementPositions(const Vec2D& currentPosition, const float& movement)
{
	CheckMovementPosition(currentPosition, currentPosition + Vec2D(0, -1), movement, UP);
	CheckMovementPosition(currentPosition, currentPosition + Vec2D(0, 1), movement, DOWN);
	CheckMovementPosition(currentPosition, currentPosition + Vec2D(-1, 0), movement, LEFT);
	CheckMovementPosition(currentPosition, currentPosition + Vec2D(1, 0), movement, RIGHT);
}

void MapEditorScene::CheckMovementPosition(const Vec2D& oldPosition, const Vec2D& newPosition, const float& movement, const EAttackDirection& direction)
{
	if (oldPosition == newPosition) return;

	float cost = GetTerrainMovementCost(mSelectedMapUnit.unitTexture, mMapTerrainIndeces[static_cast<int>(newPosition.GetX())][static_cast<int>(newPosition.GetY())]);

	if (movement - cost >= 0)
	{
		mMovementPositions.push_back(newPosition);

		CheckMovementPosition(newPosition, newPosition + Vec2D(0, -1), movement - cost, UP);
		CheckMovementPosition(newPosition, newPosition + Vec2D(0, 1), movement - cost, DOWN);
		CheckMovementPosition(newPosition, newPosition + Vec2D(1, 0), movement - cost, RIGHT);
		CheckMovementPosition(newPosition, newPosition + Vec2D(-1, 0), movement - cost, LEFT);
	}
	else if (movement - cost <= 0)
	{
		switch (direction)
		{
		case UP:
			CheckAttackPosition(oldPosition, newPosition, mSelectedMapUnit.attackRange, UP);
			return;
		case DOWN:
			CheckAttackPosition(oldPosition, newPosition, mSelectedMapUnit.attackRange, DOWN);
			return;
		case LEFT:
			CheckAttackPosition(oldPosition, newPosition, mSelectedMapUnit.attackRange, LEFT);
			return;
		case RIGHT:
			CheckAttackPosition(oldPosition, newPosition, mSelectedMapUnit.attackRange, RIGHT);
			return;
		case UP_RIGHT:
			break;
		case UP_LEFT:
			break;
		case DOWN_RIGHT:
			break;
		case DOWN_LEFT:
			break;
		default:
			break;
		}
	}
}

float MapEditorScene::GetTerrainMovementCost(const EUnitClass& unit, const ETerrainType& terrain)
{
	switch (terrain)
	{
	case ROAD:
		return 0.7;
		break;
	case BRIDGE:
		break;
	case PLAIN:
		return 1;
		break;
	case SAND:
		break;
	case RUINS:
		break;
	case FOREST:
		return 2;
		break;
	case THICKET:
		break;
	case MOUNTAIN:
		return 4;
		break;
	case PEAK:
		break;
	case CLIFF:
		break;
	case SEA:
		break;
	case RIVER:
		return 10;
		break;
	case DESERT:
		break;
	case VILLAGE:
		return 1;
		break;
	case CHURCH:
		break;
	case BRAGI_TOWER:
		break;
	case CASTLE_DEFENSE:
		return 1;
		break;
	case CASTLE_WALL:
		break;
	case UNDEFINED:
		return 0;
		break;
	default:
		break;
	}

	return 0;
}

void MapEditorScene::CheckAttackPosition(const Vec2D& oldPosition, const Vec2D& newPosition, const int& attackRange, const EAttackDirection& direction)
{
	mAttackPositions.push_back(newPosition);
	if (attackRange - 1 > 0)
	{
		switch (direction)
		{
		case UP:
			CheckAttackPosition(newPosition, newPosition + Vec2D(0, -1), attackRange - 1, UP);
			return;
		case DOWN:
			CheckAttackPosition(newPosition, newPosition + Vec2D(0, 1), attackRange - 1, DOWN);
			return;
		case LEFT:
			CheckAttackPosition(newPosition, newPosition + Vec2D(-1, 0), attackRange - 1, LEFT);
			return;
		case RIGHT:
			CheckAttackPosition(newPosition, newPosition + Vec2D(1, 0), attackRange - 1, RIGHT);
			return;
		case UP_RIGHT:
			break;
		case UP_LEFT:
			break;
		case DOWN_RIGHT:
			break;
		case DOWN_LEFT:
			break;
		default:
			break;
		}
	}
}

void MapEditorScene::DeleteMovementPositionCopies()
{
	bool contains = false;
	std::vector<Vec2D> newPositions;
	for (int i = 0; i < mMovementPositions.size(); i++)
	{
		for (const Vec2D& position : newPositions)
		{
			if (position == mMovementPositions[i])
			{
				contains = true;
				break;
			}
		}

		if (!contains)
		{
			newPositions.push_back(mMovementPositions[i]);
		}

		contains = false;
	}

	mMovementPositions.clear();
	mMovementPositions = newPositions;
}

void MapEditorScene::DeleteAttackPositionCopies()
{
	std::vector<Vec2D> newPositions;
	for (const Vec2D& attackPosition : mAttackPositions)
	{
		if (!MovementsAlreadyContainsPosition(attackPosition))
		{
			newPositions.push_back(attackPosition);
		}
	}
	mAttackPositions.clear();
	mAttackPositions = newPositions;
	newPositions.clear();

	bool contains = false;
	for (int i = 0; i < mAttackPositions.size(); i++)
	{
		for (const Vec2D& position : newPositions)
		{
			if (position == mAttackPositions[i])
			{
				contains = true;
				break;
			}
		}

		if (!contains)
		{
			newPositions.push_back(mAttackPositions[i]);
		}

		contains = false;
	}

	mAttackPositions.clear();
	mAttackPositions = newPositions;
}

void MapEditorScene::PrintTerrain(const ETerrainType& terrain)
{
	switch (terrain)
	{
	case ROAD:
		std::cout << "ROAD" << std::endl;
		break;
	case BRIDGE:
		break;
	case PLAIN:
		std::cout << "PLAIN" << std::endl;
		break;
	case SAND:
		break;
	case RUINS:
		break;
	case FOREST:
		std::cout << "FOREST" << std::endl;
		break;
	case THICKET:
		break;
	case MOUNTAIN:
		std::cout << "MOUNTAIN" << std::endl;
		break;
	case PEAK:
		break;
	case CLIFF:
		break;
	case SEA:
		std::cout << "SEA" << std::endl;
		break;
	case RIVER:
		std::cout << "RIVER" << std::endl;
		break;
	case DESERT:
		break;
	case VILLAGE:
		std::cout << "VILLAGE" << std::endl;
		break;
	case CHURCH:
		break;
	case BRAGI_TOWER:
		break;
	case CASTLE_DEFENSE:
		std::cout << "CASTLE_DEFENSE" << std::endl;
		break;
	case CASTLE_WALL:
		break;
	case UNDEFINED:
		break;
	default:
		std::cout << std::endl;
		break;
	}
}

bool MapEditorScene::MovementsAlreadyContainsPosition(const Vec2D& position)
{
	for (const Vec2D& movementPosition : mMovementPositions)
	{
		if (movementPosition == position)
		{
			return true;
		}
	}

	return false;
}

void MapEditorScene::SetSelectionRect()
{
	if (mSelectionRectEnd == mSelectionRectStart)
	{
		mSelectionXStart = mSelectionRectStart.GetX();
		mSelectionYStart = mSelectionRectStart.GetY();
		mSelectionWidth = 1;
		mSelectionHeight = 1;

		Vec2D position =
		{
			static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + mSelectionXStart * SQUARE_RENDER_SIZE * mMapZoom)),
			static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + mSelectionYStart * SQUARE_RENDER_SIZE * mMapZoom))
		};

		mSelectionRect =
		{
			static_cast<int>(position.GetX()),
			static_cast<int>(position.GetY()),
			SQUARE_RENDER_SIZE,
			SQUARE_RENDER_SIZE
		};

		mShowTileSelection = true;
		return;
	}

	if (mSelectionRectStart.GetX() < mSelectionRectEnd.GetX() && mSelectionRectStart.GetY() < mSelectionRectEnd.GetY())
	{
		mSelectionXStart = mSelectionRectStart.GetX();
		mSelectionYStart = mSelectionRectStart.GetY();
		mSelectionWidth = mSelectionRectEnd.GetX() - mSelectionRectStart.GetX() + 1;
		mSelectionHeight = mSelectionRectEnd.GetY() - mSelectionRectStart.GetY() + 1;

		Vec2D position =
		{
			static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + mSelectionXStart * SQUARE_RENDER_SIZE * mMapZoom)),
			static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + mSelectionYStart * SQUARE_RENDER_SIZE * mMapZoom))
		};

		mSelectionRect =
		{
			static_cast<int>(position.GetX()),
			static_cast<int>(position.GetY()),
			SQUARE_RENDER_SIZE * static_cast<int>(mSelectionWidth),
			SQUARE_RENDER_SIZE * static_cast<int>(mSelectionHeight)
		};
		mShowTileSelection = true;
		return;
	}
	else if (mSelectionRectStart.GetX() < mSelectionRectEnd.GetX() && mSelectionRectStart.GetY() > mSelectionRectEnd.GetY())
	{
		mSelectionXStart = mSelectionRectStart.GetX();
		mSelectionYStart = mSelectionRectEnd.GetY();
		mSelectionWidth = mSelectionRectEnd.GetX() - mSelectionRectStart.GetX() + 1;
		mSelectionHeight = mSelectionRectStart.GetY() - mSelectionRectEnd.GetY() + 1;

		Vec2D position =
		{
			static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + mSelectionXStart * SQUARE_RENDER_SIZE * mMapZoom)),
			static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + mSelectionYStart * SQUARE_RENDER_SIZE * mMapZoom))
		};

		mSelectionRect =
		{
			static_cast<int>(position.GetX()),
			static_cast<int>(position.GetY()),
			SQUARE_RENDER_SIZE * static_cast<int>(mSelectionWidth),
			SQUARE_RENDER_SIZE * static_cast<int>(mSelectionHeight)
		};
		mShowTileSelection = true;
		return;
	}
	else if (mSelectionRectStart.GetX() > mSelectionRectEnd.GetX() && mSelectionRectStart.GetY() > mSelectionRectEnd.GetY())
	{
		mSelectionXStart = mSelectionRectEnd.GetX();
		mSelectionYStart = mSelectionRectEnd.GetY();
		mSelectionWidth = mSelectionRectStart.GetX() - mSelectionRectEnd.GetX() + 1;
		mSelectionHeight = mSelectionRectStart.GetY() - mSelectionRectEnd.GetY() + 1;

		Vec2D position =
		{
			static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + mSelectionXStart * SQUARE_RENDER_SIZE * mMapZoom)),
			static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + mSelectionYStart * SQUARE_RENDER_SIZE * mMapZoom))
		};

		mSelectionRect =
		{
			static_cast<int>(position.GetX()),
			static_cast<int>(position.GetY()),
			SQUARE_RENDER_SIZE * static_cast<int>(mSelectionWidth),
			SQUARE_RENDER_SIZE * static_cast<int>(mSelectionHeight)
		};
		mShowTileSelection = true;
		return;
	}
	else if (mSelectionRectStart.GetX() > mSelectionRectEnd.GetX() && mSelectionRectStart.GetY() < mSelectionRectEnd.GetY())
	{
		mSelectionXStart = mSelectionRectEnd.GetX();
		mSelectionYStart = mSelectionRectStart.GetY();
		mSelectionWidth = mSelectionRectStart.GetX() - mSelectionRectEnd.GetX() + 1;
		mSelectionHeight = mSelectionRectEnd.GetY() - mSelectionRectStart.GetY() + 1;

		Vec2D position =
		{
			static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapXOffset + mSelectionXStart * SQUARE_RENDER_SIZE * mMapZoom)),
			static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_RENDER_SIZE) / 2) * mMapZoom + mMapYOffset + mSelectionYStart * SQUARE_RENDER_SIZE * mMapZoom))
		};

		mSelectionRect =
		{
			static_cast<int>(position.GetX()),
			static_cast<int>(position.GetY()),
			SQUARE_RENDER_SIZE * static_cast<int>(mSelectionWidth),
			SQUARE_RENDER_SIZE * static_cast<int>(mSelectionHeight)
		};
		mShowTileSelection = true;
		return;
	}

	mShowTileSelection = false;
	return;
}

void MapEditorScene::MoveSelectionUp()
{
	for (uint16_t y = mSelectionYStart; y < mSelectionYStart + mSelectionHeight; y++)
	{
		for (uint16_t x = mSelectionXStart; x < mSelectionXStart + mSelectionWidth; x++)
		{
			mMapSpriteIndeces[x][y - 1] = mMapSpriteIndeces[x][y];

			if (y == (mSelectionYStart + mSelectionHeight) - 1)
			{
				mMapSpriteIndeces[x][y] = 0;
			}
		}
	}

	mSelectionYStart -= 1;
	mSelectionRect.y -= SQUARE_RENDER_SIZE;
}

void MapEditorScene::MoveSelectionDown()
{
	for (uint16_t y = mSelectionYStart + mSelectionHeight; y > mSelectionHeight; y--)
	{
		for (uint16_t x = mSelectionXStart; x < mSelectionXStart + mSelectionWidth; x++)
		{
			mMapSpriteIndeces[x][y] = mMapSpriteIndeces[x][y - 1];

			if (y == mSelectionYStart - 1)
			{
				mMapSpriteIndeces[x][y - 1] = 0;
			}
		}
	}

	mSelectionYStart += 1;
	mSelectionRect.y += SQUARE_RENDER_SIZE;
}

void MapEditorScene::MoveSelectionLeft()
{
	for (uint16_t y = mSelectionYStart; y < mSelectionYStart + mSelectionHeight; y++)
	{
		for (uint16_t x = mSelectionXStart; x < mSelectionXStart + mSelectionWidth; x++)
		{
			mMapSpriteIndeces[x - 1][y] = mMapSpriteIndeces[x][y];

			if (x == (mSelectionXStart + mSelectionWidth) - 1)
			{
				mMapSpriteIndeces[x][y] = 0;
			}
		}
	}

	mSelectionXStart -= 1;
	mSelectionRect.x -= SQUARE_RENDER_SIZE;
}

void MapEditorScene::MoveSelectionRight()
{
	for (uint16_t y = mSelectionYStart; y < mSelectionYStart + mSelectionHeight; y++)
	{
		for (uint16_t x = mSelectionXStart + mSelectionWidth; x > mSelectionWidth; x--)
		{
			mMapSpriteIndeces[x][y] = mMapSpriteIndeces[x - 1][y];

			if (x == mSelectionXStart + 1)
			{
				mMapSpriteIndeces[x - 1][y] = 0;
			}
		}
	}

	mSelectionXStart += 1;
	mSelectionRect.x += SQUARE_RENDER_SIZE;
}

void MapEditorScene::SaveMap()
{
	std::string tileMapPath = "./Assets/MapSaveFile.txt";
	std::ifstream tileMapInFile;
	tileMapInFile.open(tileMapPath);
	if (tileMapInFile.is_open())
	{
		tileMapInFile.close();
		std::remove(tileMapPath.c_str());
	}

	std::ofstream tileMapOutFile;
	tileMapOutFile.open(tileMapPath);
	if (tileMapOutFile.is_open())
	{
		tileMapOutFile << ":width " + std::to_string(mMapWidth) << std::endl;
		tileMapOutFile << ":height " + std::to_string(mMapHeight) << std::endl;

		for (int y = 0; y < mMapHeight; y++)
		{
			for (int x = 0; x < mMapWidth; x++)
			{
				tileMapOutFile << ":tile " + std::to_string(mMapSpriteIndeces[x][y]) << std::endl;
			}
		}
	}
	tileMapOutFile.close();
}

void MapEditorScene::LoadMap()
{
	std::string path = "./Assets/MapSaveFile.txt";
	std::ifstream testFile;
	testFile.open(path);
	if (testFile.is_open())
	{
		if (mMapRects && mMapSpriteIndeces)
		{
			for (uint32_t i = 0; i < mMapWidth; i++)
			{
				delete[] mMapRects[i];
				delete[] mMapSpriteIndeces[i];
			}
			delete[] mMapRects;
			delete[] mMapSpriteIndeces;
		}

		FileCommandLoader fileLoader;

		Command mapWidthCommand;
		mapWidthCommand.command = "width";
		mapWidthCommand.parseFunc = [&](ParseFuncParams params)
		{
			mMapWidth = FileCommandLoader::ReadInt(params);
		};
		fileLoader.AddCommand(mapWidthCommand);

		Command mapHeightCommand;
		mapHeightCommand.command = "height";
		mapHeightCommand.parseFunc = [&](ParseFuncParams params)
		{
			mMapHeight = FileCommandLoader::ReadInt(params);
		};
		fileLoader.AddCommand(mapHeightCommand);

		Command spriteCommand;
		spriteCommand.command = "tile";
		spriteCommand.parseFunc = [&](ParseFuncParams params)
		{
			mLoadedSpriteIndeces.push_back(static_cast<uint32_t>(FileCommandLoader::ReadInt(params)));
		};
		fileLoader.AddCommand(spriteCommand);

		fileLoader.LoadFile(path);

		InitMap();
	}

	testFile.close();
}