#include "InputController.h"
#include "GameController.h"

#include <SDL.h>

InputController::InputController(): mQuit(nullptr), mnoptrCurrentController(nullptr)
{
}

void InputController::Init(InputAction quitAction)
{
	mQuit = quitAction;
}

void InputController::Update(uint32_t dt)
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			mQuit(dt, SDL_PRESSED);
			break;
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			if (mnoptrCurrentController)
			{
				InputAction action = mnoptrCurrentController->GetActionForKey(sdlEvent.key.keysym.sym);
				action(dt, static_cast<InputState>(sdlEvent.key.state));
			}
			break;
		case SDL_MOUSEMOTION:
			if (mnoptrCurrentController)
			{
				if (MouseMovedAction mouseMoved = mnoptrCurrentController->GetMouseMovedAction())
				{
					MousePosition position;
					position.xPos = sdlEvent.motion.x;
					position.yPos = sdlEvent.motion.y;
					mouseMoved(position);
				}
			}
			break;
		case SDL_MOUSEBUTTONUP:

			break;
		case SDL_MOUSEBUTTONDOWN:
			if (mnoptrCurrentController)
			{
				MouseInputAction action = mnoptrCurrentController->GetMouseButtonActionForMouseButton(static_cast<MouseButton>(sdlEvent.button.button));

				MousePosition position;
				position.xPos = sdlEvent.button.x;
				position.yPos = sdlEvent.button.y;

				action(static_cast<InputState>(sdlEvent.key.state), position);
			}
			break;
		default:
			break;
		}
	}
}

void InputController::SetGameController(GameController* controller)
{
	mnoptrCurrentController = controller;
}
