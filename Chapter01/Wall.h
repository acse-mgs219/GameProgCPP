#pragma once

#include "IGameObject.h"

#include "SDL/SDL.h"

struct Wall : public IGameObject
{
public:
	Wall(int orgX, int orgY, int width, int height);

	// Inherited via IGameObject
	void ProcessInput(const Uint8* state) override {}
	void UpdateSelf(float deltaTime) override {}
	const SDL_Rect& GetRect() const override { return mSDLRect; }

	int mOriginX;
	int mOriginY;
	int mWidth;
	int mHeight;

private:
	SDL_Rect mSDLRect;
};