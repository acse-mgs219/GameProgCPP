#include "Ball.h"
#include "Game.h"

#include <algorithm>

Ball::Ball(Utils::Vector2 pos, Utils::Vector2 vel, std::optional<float> thickness) 
	: mPosition(pos)
	, mVelocity(vel)
	, mThickness(thickness.value_or(Utils::defaultThickness))
{
}

void Ball::UpdateSelf(float deltaTime)
{
	const Utils::Extents& sExtents = Game::GetExtents();

	mPosition.x += mVelocity.x * deltaTime;
	mPosition.x = std::clamp(mPosition.x, sExtents.MinX(mThickness), sExtents.MaxX(mThickness));
	mPosition.y += mVelocity.y * deltaTime;
	mPosition.y = std::clamp(mPosition.y, sExtents.MinY(mThickness), sExtents.MaxY(mThickness));
}

const SDL_Rect& Ball::GetRect() const
{
	return {
		static_cast<int>(mPosition.x - mThickness / 2),
		static_cast<int>(mPosition.y - mThickness / 2),
		static_cast<int>(mThickness),
		static_cast<int>(mThickness)
	};
}
