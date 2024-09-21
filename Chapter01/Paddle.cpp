#include "Paddle.h"

#include "Game.h"

#include <algorithm>

constexpr float defaultHeight = 100.0f;
constexpr float defaultSpeed = 1.f;

Paddle::Paddle(Utils::Vector2 initialPosition, PaddleControls controls, std::optional<float> width, std::optional<float> height, std::optional<float> speed)
	: mControls{controls}
	, mPosition{ initialPosition }
	, mHeight{width.value_or(defaultHeight)}
	, mWidth{height.value_or(Utils::defaultThickness)}
	, mSpeed{speed.value_or(defaultSpeed)}
	, mSDLRect{
		static_cast<int>(mPosition.x),
		static_cast<int>(mPosition.y - mHeight / 2),
		static_cast<int>(mWidth),
		static_cast<int>(mHeight)
	}
{
}

void Paddle::ProcessInput(const Uint8* state)
{
	if (mControls.mIsHuman == false)
	{
		return; // #TODO
	}

	if (state[mControls.mUpButton])
	{
		mDirection = Direction::Up;
	}
	else if (state[mControls.mDownButton])
	{
		mDirection = Direction::Down;
	}
	else
	{
		mDirection = Direction::Still;
	}
}

void Paddle::UpdateSelf(float deltaTime)
{
	if (mDirection == Direction::Still)
	{
		return;
	}

	mPosition.y += static_cast<int>(mDirection) * 300.0f * deltaTime;
	const Utils::Extents& sExtents = Game::GetExtents();

	// Make sure paddle doesn't move off screen!
	mPosition.y = std::clamp(mPosition.y, sExtents.MinY(mHeight), sExtents.MaxY(mHeight));
}
