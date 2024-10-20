#include "IGameObject.h"

const SDL_Color& IGameObject::GetColor() const
{
	static SDL_Color defaultColorWhite = { 255, 255, 255, 255 };
	return defaultColorWhite;
}
