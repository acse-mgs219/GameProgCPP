// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Humanoid.h"
#include "AnimSpriteComponent.h"
#include "Game.h"

Humanoid::Humanoid(Game* game)
	:Actor(game)
	, mRightSpeed(0.0f)
	, mDownSpeed(0.0f)
{
	// Create an animated sprite component
	mAnimSpriteComponent = std::make_unique<AnimSpriteComponent>(this);
	std::vector<SDL_Texture*> anims = {
		game->GetTexture("Assets/Character01.png"),
		game->GetTexture("Assets/Character02.png"),
		game->GetTexture("Assets/Character03.png"),
		game->GetTexture("Assets/Character04.png"),
		game->GetTexture("Assets/Character05.png"),
		game->GetTexture("Assets/Character06.png"),
		game->GetTexture("Assets/Character07.png"),
		game->GetTexture("Assets/Character08.png"),
		game->GetTexture("Assets/Character09.png"),
		game->GetTexture("Assets/Character10.png"),
		game->GetTexture("Assets/Character11.png"),
		game->GetTexture("Assets/Character12.png"),
		game->GetTexture("Assets/Character13.png"),
		game->GetTexture("Assets/Character14.png"),
		game->GetTexture("Assets/Character15.png"),
		game->GetTexture("Assets/Character16.png"),
		game->GetTexture("Assets/Character17.png"),
		game->GetTexture("Assets/Character18.png")
	};
	mAnimSpriteComponent->SetAnimTextures(anims);

	AnimSequence walkingSequence = AnimSequence(0, 5);
	mAnimSpriteComponent->CreateAnimSequence(std::move(walkingSequence), "Walking");

	AnimSequence jumpingSequence = AnimSequence(6, 14);
	mAnimSpriteComponent->CreateAnimSequence(std::move(jumpingSequence), "Jumping");

	mAnimSpriteComponent->CreateAnimSequence(AnimSequence(15, 17), "Punching");
	mAnimSpriteComponent->SetAnimSequence("Walking");
}

void Humanoid::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);
	// Update position based on speeds and delta time
	Vector2 pos = GetPosition();
	pos.x += mRightSpeed * deltaTime;
	pos.y += mDownSpeed * deltaTime;
	// Restrict position to left half of screen
	if (pos.x < 25.0f)
	{
		pos.x = 25.0f;
	}
	else if (pos.x > 500.0f)
	{
		pos.x = 500.0f;
	}
	if (pos.y < 25.0f)
	{
		pos.y = 25.0f;
	}
	else if (pos.y > 743.0f)
	{
		pos.y = 743.0f;
	}
	SetPosition(pos);
}

void Humanoid::ProcessKeyboard(const uint8_t* state)
{
	mRightSpeed = 0.0f;
	mDownSpeed = 0.0f;
	
	std::string animationRequired{};
	int repeatCount = -1;

	// right/left
	if (state[SDL_SCANCODE_D])
	{
		mRightSpeed += 250.0f;
		animationRequired = { "Walking" };
	}
	if (state[SDL_SCANCODE_A])
	{
		mRightSpeed -= 250.0f;
		animationRequired = { "Walking" };
	}
	// up/down
	if (state[SDL_SCANCODE_S])
	{
		mDownSpeed += 300.0f;
		animationRequired = { "Walking" };
	}
	if (state[SDL_SCANCODE_W])
	{
		mDownSpeed -= 300.0f;
		animationRequired = { "Walking" };
	}

	if (state[SDL_SCANCODE_SPACE])
	{
		animationRequired = "Jumping";
		repeatCount = 1;
	}
	if (state[SDL_SCANCODE_E])
	{
		animationRequired = "Punching";
		repeatCount = 1;
	}

	if (!animationRequired.empty())
	{
		mAnimSpriteComponent->SetAnimSequence(animationRequired, repeatCount);
	}
}
