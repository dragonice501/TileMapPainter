#include "ECSMapTest.h"
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
#include <SDL_image.h>
#include <imgui.h>
#include <imgui_sdl.h>
#include <imgui_impl_sdl.h>

ECSMapTest::ECSMapTest()
{
}

void ECSMapTest::Init(std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer)
{
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, static_cast<int>(Application::GetWindowWidth()), static_cast<int>(Application::GetWindowHeight()));
	mIsRunning = true;

	registry->AddSystem<RenderSystem>();
	assetStore->AddTexture(renderer, "tilemap", "./Assets/Chapter_0_m.png");

	Setup(registry);
}

void ECSMapTest::Destroy()
{
}

void ECSMapTest::Setup(std::unique_ptr<Registry>& registry)
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

	for (uint32_t y = 0; y < mMapHeight; y++)
	{
		for (uint32_t x = 0; x < mMapWidth; x++)
		{
			Vec2D position = {
				static_cast<float>((Application::GetWindowWidth() / 2 - (mMapWidth * SQUARE_PIXEL_SIZE) / 2 + x * SQUARE_PIXEL_SIZE)),
				static_cast<float>((Application::GetWindowHeight() / 2 - (mMapHeight * SQUARE_PIXEL_SIZE) / 2 + y * SQUARE_PIXEL_SIZE)) };

			Entity tile = registry->CreateEntity();
			tile.AddComponent<TransformComponent>(position, 0, Vec2D(1.0, 1.0));
			tile.AddComponent<SpriteComponent>(
				"tilemap",
				SQUARE_PIXEL_SIZE,
				SQUARE_PIXEL_SIZE,
				0,
				0,
				0);
		}
	}

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

void ECSMapTest::Input(std::unique_ptr<Registry>& registry)
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
					//MoveSelectionUp();
				}
			}
			else if (sdlEvent.key.keysym.sym == SDLK_RIGHT)
			{
				if (mSelectedTool == SELECT_TOOL)
				{
					//MoveSelectionRight();
				}
			}
			else if (sdlEvent.key.keysym.sym == SDLK_LEFT)
			{
				if (mSelectedTool == SELECT_TOOL)
				{
					//MoveSelectionLeft();
				}
			}
			else if (sdlEvent.key.keysym.sym == SDLK_DOWN)
			{
				if (mSelectedTool == SELECT_TOOL)
				{
					//MoveSelectionDown();
				}
			}
			break;
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
		case SDL_MOUSEMOTION:
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
		default:
			break;
		}
	}
}

void ECSMapTest::Update(std::unique_ptr<Registry>& registry, uint32_t deltaTime)
{
	registry->Update();
}

void ECSMapTest::Render(std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

	DrawGUI();

	SDL_RenderPresent(renderer);
}

void ECSMapTest::DrawMap(SDL_Renderer* renderer)
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

void ECSMapTest::DrawTileMap(SDL_Renderer* renderer)
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

void ECSMapTest::DrawGUI()
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
			ImGui::Text("Mouse coordinates (x: %.1f, y: %.1f)", static_cast<float>(mCursorPosition.GetX()), static_cast<float>(mCursorPosition.GetY()));
			ImGui::Text("Map coordinates (x: %.1f, y: %.1f)", static_cast<float>(cursorRect.GetX()), static_cast<float>(cursorRect.GetY()));
			if (ImGui::SliderFloat("Zoom", &zoomLevel, 0.1f, 3.0f))
			{
				//mMouseButtonDown = false;
				//mMapZoom = zoomLevel;
				//SetMapRectPositions();
			}
			if (ImGui::Button("Reset Zoom"))
			{
				//zoomLevel = 1;
				//mMapZoom = zoomLevel;
				//SetMapRectPositions();
			}
			if (ImGui::Button("Recenter Map"))
			{
				//mMapXOffset = 0;
				//mMapYOffset = 0;
				//SetMapRectPositions();
			}
			if (ImGui::Button("Show Overlay"))
			{
				//mShowOverlay = !mShowOverlay;
			}
		}

		if (ImGui::CollapsingHeader("Tools", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Paint Tile"))
			{
				//mSelectedTool = PAINT_TILE_TOOL;
				//mShowSelection = false;
			}
			if (ImGui::Button("Fill Tile"))
			{
				//mSelectedTool = FILL_TILE_TOOL;
			}
			if (ImGui::Button("Paint Unit"))
			{
				//mSelectedTool = PAINT_UNIT_TOOL;
				//mShowSelection = false;
			}
			if (ImGui::Button("Pan"))
			{
				//mSelectedTool = PAN_TOOL;
				//mShowSelection = false;
			}
			if (ImGui::Button("Select"))
			{
				//mSelectedTool = SELECT_TOOL;
				//mShowSelection = true;
			}
		}

		if (ImGui::CollapsingHeader("Tiles/Units", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Tile Map"))
			{
				if (mEditorState == EDITING_MAP) mEditorState = SELECTING_SPRITE;
				else mEditorState = EDITING_MAP;
			}

			static int selectedClassIndex = 0;
			const char* classes[] =
			{
				"Junior Lord", "Knight Lord", "Prince", "Princess", "Master Knight", "Myrmidon", "Swordmaster", "Forrest",
				"Thief", "Thief Fighter", "Dancer", "Axe Fighter", "Warrior", "Bow Fighter", "Sniper", "Cavalier", "Paladin_M", "Paladin_F"
				"Troubadour", "Ranger", "Lance Knight", "Duke Knight", "Axe Knight", "Great Knight", "Bow Knight", "Sword Armour", "General",
				"Pegasus Knight", "Falcon Knight", "Wyvern Rider", "Wyvern Lord", "Thunder Mage", "Wind Mage", "Mage Fighter_M", "Mage Fighter_F",
				"Mage", "Mage Knight", "Bard", "Light Preistess", "Sage", "High Priest"
			};
			ImGui::Combo("Class", &selectedClassIndex, classes, IM_ARRAYSIZE(classes));

			static int level = 1;
			static int hp = 1;
			static int strength = 1;
			static int magic = 1;
			static int skill = 1;
			static int speed = 1;
			static int luck = 1;
			static int defense = 1;
			static int modifier = 1;
			ImGui::InputInt("Level", &level);
			ImGui::InputInt("HP", &hp);
			ImGui::InputInt("Strength", &strength);
			ImGui::InputInt("Magic", &magic);
			ImGui::InputInt("Skill", &skill);
			ImGui::InputInt("Speed", &speed);
			ImGui::InputInt("Luck", &luck);
			ImGui::InputInt("Defense", &defense);
			ImGui::InputInt("Modifier", &modifier);
		}

		if (ImGui::CollapsingHeader("Input/Output", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("New Map"))
			{
				/*InitMap();
				mShowOverlay = false;
				mShowSelection = false;*/
			}

			if (ImGui::Button("Save Map"))
			{
				//SaveMap();
			}

			if (ImGui::Button("Load Map"))
			{
				//LoadMap();
			}
		}
	}
	ImGui::End();

	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
}

bool ECSMapTest::TileInsideCamera(uint16_t x, uint16_t y)
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

bool ECSMapTest::CursorInGUI()
{
	return
		mCursorPosition.GetX() < 280 &&
		mCursorPosition.GetY() < 680;
}

void ECSMapTest::InitMap()
{
	
}

void ECSMapTest::SetMapRectPositions()
{
	for (uint16_t y = 0; y < mMapHeight; y++)
	{
		for (uint16_t x = 0; x < mMapWidth; x++)
		{
			Vec2D position = {
				static_cast<float>((Application::GetWindowWidth() / 2 - ((mMapWidth * SQUARE_PIXEL_SIZE) / 2) * mMapZoom + mMapXOffset + x * SQUARE_PIXEL_SIZE * mMapZoom)),
				static_cast<float>((Application::GetWindowHeight() / 2 - ((mMapHeight * SQUARE_PIXEL_SIZE) / 2) * mMapZoom + mMapYOffset + y * SQUARE_PIXEL_SIZE * mMapZoom)) };

			mMapRects[x][y] = SDL_Rect{
				static_cast<int>(position.GetX()),
				static_cast<int>(position.GetY()),
				static_cast<int>(static_cast<float>(SQUARE_PIXEL_SIZE) * mMapZoom),
				static_cast<int>(static_cast<float>(SQUARE_PIXEL_SIZE) * mMapZoom) };
		}
	}
}

void ECSMapTest::SetMapSpriteIndeces()
{
	for (uint16_t y = 0; y < mMapHeight; y++)
	{
		for (uint16_t x = 0; x < mMapWidth; x++)
		{
			mMapSpriteIndeces[x][y] = 0;
		}
	}
}

Vec2D ECSMapTest::GetCursorMapRect()
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

void ECSMapTest::CheckCursorInMap()
{
	for (uint16_t y = 0; y < mMapHeight; y++)
	{
		for (uint16_t x = 0; x < mMapWidth; x++)
		{
			if (SquareContainsCursorPosition(mMapRects[x][y]))
			{
				std::cout << x << ',' << y << std::endl;
				mMapSpriteIndeces[x][y] = mSelectedSpriteIndex;
				return;
			}
		}
	}
}

void ECSMapTest::CheckCursorInSpriteSheet()
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

bool ECSMapTest::SquareContainsCursorPosition(const SDL_Rect& rect)
{
	return
		mCursorPosition.GetX() >= rect.x &&
		mCursorPosition.GetX() <= rect.x + rect.w &&
		mCursorPosition.GetY() >= rect.y &&
		mCursorPosition.GetY() <= rect.y + rect.h;
}

void ECSMapTest::CopyMapRectSprite()
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

void ECSMapTest::FillTile(uint16_t xIndex, uint16_t yIndex)
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

void ECSMapTest::SetSelectionRect()
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

void ECSMapTest::MoveSelectionUp()
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

void ECSMapTest::MoveSelectionDown()
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

void ECSMapTest::MoveSelectionLeft()
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

void ECSMapTest::MoveSelectionRight()
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

void ECSMapTest::SaveMap()
{
	std::string path = "./Assets/testExport.txt";
	std::ifstream testFile;
	testFile.open(path);
	if (testFile.is_open())
	{
		testFile.close();
		std::remove(path.c_str());
	}

	std::ofstream test;
	test.open(path);
	if (test.is_open())
	{
		test << ":width " + std::to_string(mMapWidth) << std::endl;
		test << ":height " + std::to_string(mMapHeight) << std::endl;

		for (int y = 0; y < mMapHeight; y++)
		{
			for (int x = 0; x < mMapWidth; x++)
			{
				test << ":tile " + std::to_string(mMapSpriteIndeces[x][y]) << std::endl;
			}
		}
	}

	test.close();
}

void ECSMapTest::LoadMap()
{
	std::string path = "./Assets/testExport.txt";
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