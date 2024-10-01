#include "Paddle.h"

#include "Game.h"

#include <algorithm>
#include <cmath>
#include <numeric>

constexpr float defaultHeight = 100.0f;
constexpr float defaultSpeed = 1.f;

Paddle::Paddle(Utils::Vector2 initialPosition, PaddleControls controls, std::optional<float> width, std::optional<float> height, std::optional<float> speed)
	: mControls{controls}
	, mPosition{ initialPosition }
	, mHeight{width.value_or(defaultHeight)}
	, mWidth{height.value_or(Utils::defaultThickness)}
	, mSpeed{speed.value_or(defaultSpeed)}
{
}

void Paddle::ProcessInput(const Uint8* state)
{
	if (mControls.mIsHuman == false)
	{
		return;
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

void Paddle::MockInputAI(float deltaTime, const std::vector<std::unique_ptr<Ball>>& balls)
{
	static float accumulatedTime = .0f;
	accumulatedTime += deltaTime;
	if (accumulatedTime < mControls.UpdateInterval())
	{
		return;
	}
	deltaTime = .0f;

	// #TODO: Could be a util function for Vector 2
	static const auto distanceCalculator = [&mPosition = mPosition](const std::unique_ptr<Ball>& ball)
		{
			if (ball.get() == nullptr)
			{
				return std::numeric_limits<float>::max();
			}

			return std::sqrtf(std::powf((mPosition.x - ball->mPosition.x), 2) + std::powf((mPosition.y - ball->mPosition.y), 2));
		};

	const auto& closestBall = std::min_element(balls.begin(), balls.end(), [&](const std::unique_ptr<Ball>& b1, const std::unique_ptr<Ball>& b2)
		{
			return distanceCalculator(b1) < distanceCalculator(b2);
		});

	// ballDir > 0 => ball is OVER the paddle
	const int ballDir = mPosition.y - closestBall->get()->mPosition.y;

	if (ballDir > 0)
	{
		mDirection = Direction::Up;
	}
	else if (ballDir == 0)
	{
		mDirection = Direction::Still;
	}
	else
	{
		mDirection = Direction::Down;
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

const SDL_Rect& Paddle::GetRect() const
{
	return {
		static_cast<int>(mPosition.x),
		static_cast<int>(mPosition.y - mHeight / 2),
		static_cast<int>(mWidth),
		static_cast<int>(mHeight)
	};
}

float PaddleControls::UpdateInterval() const
{
	switch (mAIStrength)
	{
	case Difficulty::None:
		return std::numeric_limits<float>::max();
	case Difficulty::Easy:
		return .05f;
	case Difficulty::Medium:
		return .04f;
	case Difficulty::Hard:
		return .02f;
	case Difficulty::Impossible:
		return .0f;
	}
}
