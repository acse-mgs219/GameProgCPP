#pragma once

#include "IGameObject.h"
#include "Utils.h"

#include "SDL/SDL.h"

#include <optional>

struct Paddle;

struct Ball : public IGameObject
{
public:
	Ball(Utils::Vector2 pos, Utils::Vector2 vel, std::optional<float> thickness = std::nullopt);

	// Inherited via IGameObject
	void ProcessInput(const Uint8* state) override {}
	void UpdateSelf(float deltaTime) override;
	const SDL_Rect& GetRect() const override;
	void Reset();

	Utils::Vector2 mPosition;
	Utils::Vector2 mVelocity;
	float mThickness;
	Paddle* mLastPaddleTouched{nullptr};

private:
	Utils::Vector2 mOriginalPosition;
	Utils::Vector2 mOriginalVelocity;
};

