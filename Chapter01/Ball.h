#pragma once

#include "IGameObject.h"
#include "Utils.h"

#include "SDL/SDL.h"

#include <optional>

struct Ball : public IGameObject
{
public:
	Ball(Utils::Vector2 pos, Utils::Vector2 vel, std::optional<float> thickness = std::nullopt);

	// Inherited via IGameObject
	void ProcessInput(const Uint8* state) override {}
	void UpdateSelf(float deltaTime) override;
	const SDL_Rect& GetRect() const override { return mSDLRect; }

	Utils::Vector2 mPosition;
	Utils::Vector2 mVelocity;
	float mThickness;

private:

	SDL_Rect mSDLRect;
};

