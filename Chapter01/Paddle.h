#pragma once

#include "IGameObject.h"
#include "Utils.h"

#include "SDL/SDL.h"

#include <optional>

struct PaddleControls
{
	bool mIsHuman{ true };
	SDL_Scancode mUpButton{ SDL_SCANCODE_W };
	SDL_Scancode mDownButton{ SDL_SCANCODE_S };
};

struct Paddle : public IGameObject
{
public:
	enum class Direction
	{
		Up = -1,
		Still = 0,
		Down = 1
	};

	Paddle(Utils::Vector2 initialPosition, PaddleControls controls = {}, std::optional<float> width = std::nullopt, std::optional<float> height = std::nullopt, std::optional<float> speed = std::nullopt);

	// Inherited via IGameObject
	void ProcessInput(const Uint8* state) override;
	void UpdateSelf(float deltaTime) override;
	const SDL_Rect& GetRect() const override;

	void SetDirection(Paddle::Direction dir) { mDirection = dir; }
	void Score() { mScore++; }
	int GetScore() const { return mScore; }

	Direction mDirection{ Direction::Up };
	Utils::Vector2 mPosition;
	float mHeight;
	float mWidth;

private:
	PaddleControls mControls;
	float mSpeed;
	int mScore{0};
};

