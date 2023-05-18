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

bool MapEditorScene::Init(SDL_Renderer* renderer, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore)
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
	mMapRects = new SDL_Rect*[mMapWidth];
	mMapSpriteIndeces = new uint16_t* [mMapWidth];
	for (uint16_t i = 0; i < mMapWidth; i++)
	{
		mMapRects[i] = new SDL_Rect[mMapHeight];
		mMapSpriteIndeces[i] = new uint16_t[mMapHeight];
	}

	SetMapRectPositions();
	SetMapSpriteIndeces();

	for (uint8_t y = 0; y < TILE_MAP_HEIGHT; y++)
	{
		for (uint8_t x = 0; x < TILE_MAP_WIDTH; x++)
		{
			Vec2D position = {
				static_cast<float>((Application::GetWindowWidth() / 2 - (TILE_MAP_WIDTH * SQUARE_RENDER_SIZE) / 2 + x * SQUARE_RENDER_SIZE)),
				static_cast<float>((Application::GetWindowHeight() / 2 - (TILE_MAP_HEIGHT * SQUARE_RENDER_SIZE) / 2 + y * SQUARE_RENDER_SIZE)) };
			mSpriteSheetRects[x][y] = new SDL_Rect{ static_cast<int>(position.GetX()),static_cast<int>(position.GetY()), SQUARE_RENDER_SIZE, SQUARE_RENDER_SIZE };
		}
	}

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
				if (mSelectedTool == SELECT_TOOL)
				{
					MoveSelectionUp();
				}
			}
			else if (sdlEvent.key.keysym.sym == SDLK_RIGHT)
			{
				if (mSelectedTool == SELECT_TOOL)
				{
					MoveSelectionRight();
				}
			}
			else if (sdlEvent.key.keysym.sym == SDLK_LEFT)
			{
				if (mSelectedTool == SELECT_TOOL)
				{
					MoveSelectionLeft();
				}
			}
			else if (sdlEvent.key.keysym.sym == SDLK_DOWN)
			{
				if (mSelectedTool == SELECT_TOOL)
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
						case PAINT_TILE_TOOL:
							CheckCursorInMap();
							break;
						case FILL_TILE_TOOL:
							//FillTile(static_cast<uint16_t>(position.GetX()), static_cast<uint16_t>(position.GetY()));
							break;
						case PAINT_UNIT_TOOL:
							PaintUnit(GetCursorMapRect());
							break;
						case PAN_TOOL:
							break;
						case SELECT_TOOL:
							mSelectionRectStart = GetCursorMapRect();
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
					case PAINT_TILE_TOOL:
						CopyMapRectSprite();
						break;
					case FILL_TILE_TOOL:
						CopyMapRectSprite();
						break;
					case PAINT_UNIT_TOOL:
						RemoveUnit(GetCursorMapRect());
						break;
					case PAN_TOOL:
						break;
					case SELECT_TOOL:
						mShowSelection = false;
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
			if (mSelectedTool == SELECT_TOOL)
			{
				if (sdlEvent.button.button == SDL_BUTTON_LEFT)
				{
					mSelectionRectEnd = GetCursorMapRect();
					SetSelectionRect();
				}
				else if (sdlEvent.button.button == SDL_BUTTON_RIGHT)
				{
					mShowSelection = false;
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
				case SELECT_TOOL:
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

	if (mShowSelection)
	{
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
		SDL_RenderFillRect(renderer, &mSelectionRect);
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
			if (ImGui::Button("Show Overlay"))
			{
				mShowOverlay = !mShowOverlay;
			}
		}

		if (ImGui::CollapsingHeader("Input/Output", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("New Map"))
			{
				InitMap();
				mShowOverlay = false;
				mShowSelection = false;

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

		if (ImGui::CollapsingHeader("Tools", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Paint Tile"))
			{
				mSelectedTool = PAINT_TILE_TOOL;
				mShowSelection = false;
			}
			if (ImGui::Button("Fill Tile"))
			{
				mSelectedTool = FILL_TILE_TOOL;
			}
			if (ImGui::Button("Paint Unit"))
			{
				mSelectedTool = PAINT_UNIT_TOOL;
				mShowSelection = false;
			}
			if (ImGui::Button("Pan"))
			{
				mSelectedTool = PAN_TOOL;
				mShowSelection = false;
			}
			if (ImGui::Button("Select"))
			{
				mSelectedTool = SELECT_TOOL;
				mShowSelection = true;
			}
		}

		if (ImGui::CollapsingHeader("Tiles/Units", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Tile Map"))
			{
				if (mEditorState == EDITING_MAP) mEditorState = SELECTING_SPRITE;
				else mEditorState = EDITING_MAP;
			}

			if (ImGui::Button("Save Units"))
			{
				SaveUnits();
			}

			if (ImGui::Button("Load Units"))
			{
				LoadUnits();
			}

			if (ImGui::Button("Reset Stats"))
			{
				mSelectedUnit = KNIGHT_LORD;
				mNewUnitLevel = 1;
				mNewUnitHP = 1;
				mNewUnitStrength = 1;
				mNewUnitMagic = 1;
				mNewUnitSkill = 1;
				mNewUnitSpeed = 1;
				mNewUnitLuck = 1;
				mNewUnitDefense = 1;
				mNewUnitModifier = 1;
			}

			static int selectedUnitClassIndex = mSelectedUnit;
			const char* classes[] =
			{
				"Bow Fighter", "Dancer", "Knight Lord", "Mage", "Sword Armour"
			};
			if (ImGui::Combo("Class", &selectedUnitClassIndex, classes, IM_ARRAYSIZE(classes)))
			{
				SetSelectedUnitClass(selectedUnitClassIndex);
			}

			mSelectedPaintUnit.level = mNewUnitLevel;
			mSelectedPaintUnit.hp = mNewUnitHP;
			mSelectedPaintUnit.strength = mNewUnitStrength;
			mSelectedPaintUnit.magic = mNewUnitMagic;
			mSelectedPaintUnit.skill = mNewUnitSkill;
			mSelectedPaintUnit.speed = mNewUnitSpeed;
			mSelectedPaintUnit.luck = mNewUnitLuck;
			mSelectedPaintUnit.defense = mNewUnitDefense;
			mSelectedPaintUnit.modifier = mNewUnitModifier;
			
			ImGui::InputInt("Level", &mNewUnitLevel);
			ImGui::InputInt("HP", &mNewUnitHP);
			ImGui::InputInt("Strength", &mNewUnitStrength);
			ImGui::InputInt("Magic", &mNewUnitMagic);
			ImGui::InputInt("Skill", &mNewUnitSkill);
			ImGui::InputInt("Speed", &mNewUnitSpeed);
			ImGui::InputInt("Luck", &mNewUnitLuck);
			ImGui::InputInt("Defense", &mNewUnitDefense);
			ImGui::InputInt("Modifier", &mNewUnitModifier);

			std::string unitString = "Selected Unit: " + std::to_string(mSelectedMapUnit.unitTexture);
			std::string levelString = "Selected Unit Level: " + std::to_string(mSelectedMapUnit.level);
			std::string hpString = "Selected Unit HP: " + std::to_string(mSelectedMapUnit.hp);
			std::string strengthString = "Selected Unit Strength: " + std::to_string(mSelectedMapUnit.strength);
			std::string magicString = "Selected Unit magic: " + std::to_string(mSelectedMapUnit.magic);
			std::string skillString = "Selected Unit Skill: " + std::to_string(mSelectedMapUnit.skill);
			std::string speedString = "Selected Unit Speed: " + std::to_string(mSelectedMapUnit.speed);
			std::string luckString = "Selected Unit Luck: " + std::to_string(mSelectedMapUnit.luck);
			std::string defenseString = "Selected Unit Defense: " + std::to_string(mSelectedMapUnit.defense);
			std::string modifierString = "Selected Unit Modifier: " + std::to_string(mSelectedMapUnit.modifier);

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
	for (uint16_t i = 0; i < mMapWidth; i++)
	{
		mMapRects[i] = new SDL_Rect[mMapHeight];
		mMapSpriteIndeces[i] = new uint16_t[mMapHeight];
	}

	SetMapRectPositions();
	SetMapSpriteIndeces();

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
	for (uint16_t y = 0; y < mMapHeight; y++)
	{
		for (uint16_t x = 0; x < mMapWidth; x++)
		{
			mMapSpriteIndeces[x][y] = 0;
		}
	}
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
				if (mSelectedMapUnit != sprite)
				{
					mSelectedMapUnit = sprite;
				}
				else if (mSelectedMapUnit == sprite)
				{
					mSelectedUnit = mSelectedMapUnit.unitTexture;
					mNewUnitLevel = mSelectedMapUnit.level;
					mNewUnitHP = mSelectedMapUnit.hp;
					mNewUnitStrength = mSelectedMapUnit.strength;
					mNewUnitMagic = mSelectedMapUnit.magic;
					mNewUnitSkill = mSelectedMapUnit.skill;
					mNewUnitSpeed = mSelectedMapUnit.speed;
					mNewUnitLuck = mSelectedMapUnit.luck;
					mNewUnitDefense = mSelectedMapUnit.defense;
					mNewUnitModifier = mSelectedMapUnit.modifier;
				}
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
	animatedUnitSprite.modifier = mNewUnitModifier;

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
	default:
		break;
	}

	mSelectedPaintUnit.startTime = SDL_GetTicks();
	mSelectedPaintUnit.position = GetCursorMapRect();
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
			unitOutFile << ":modifier " + std::to_string(static_cast<int>(uint.modifier)) << std::endl << std::endl;
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
		newUnit.modifier = mLoadedUnitsModifier[i];

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

		mShowSelection = true;
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
		mShowSelection = true;
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
		mShowSelection = true;
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
		mShowSelection = true;
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
		mShowSelection = true;
		return;
	}

	mShowSelection = false;
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