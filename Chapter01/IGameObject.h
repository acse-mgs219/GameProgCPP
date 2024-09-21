#pragma once

#include "SDL/SDL.h"

struct IGameObject
{
public:
	virtual void ProcessInput(const Uint8* state) = 0;
	virtual void UpdateSelf(float deltaTime) = 0;
	virtual const SDL_Rect& GetRect() const = 0;
};

