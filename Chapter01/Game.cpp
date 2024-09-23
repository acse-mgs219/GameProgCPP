// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	// Set up pong specific classes
	SetupBalls();
	SetupPaddles();
	SetupWalls();

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	for (auto& go : mGameObjects)
	{
		go->ProcessInput(state);
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	for (auto& go : mGameObjects)
	{
		go->UpdateSelf(deltaTime);
	}

	for (auto& ball : mBalls)
	{
		// Bounce if needed
		HandleCollisions(*ball);
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	for (const auto& go : mGameObjects)
	{
		SDL_RenderFillRect(mRenderer, &(go->GetRect()));
	}
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::SetupWalls()
{
	// top wall
	Wall wall = Wall{ 0, 0, 1024, Utils::defaultThickness };
	mWalls.push_back(std::make_unique<Wall>(wall));
	mGameObjects.emplace_back(mWalls.back().get());

	// bottom wall
	wall.mOriginY = 768 - Utils::defaultThickness;
	mWalls.push_back(std::make_unique<Wall>(wall));
	mGameObjects.emplace_back(mWalls.back().get());

	// right wall
	wall.mOriginX = 1024 - Utils::defaultThickness;
	wall.mOriginY = 0;
	wall.mWidth = Utils::defaultThickness;
	wall.mHeight = 1024;
	mWalls.push_back(std::make_unique<Wall>(wall));
	mGameObjects.emplace_back(mWalls.back().get());
}

void Game::SetupBalls()
{
	Utils::Vector2 ballPos{ 1024.f / 2.f, 768.f / 2.f };
	Utils::Vector2 ballVel{ -200.f, 235.f };
	mBalls.emplace_back(std::make_unique<Ball>(ballPos, ballVel));
	mGameObjects.emplace_back(mBalls.back().get());
}

void Game::SetupPaddles()
{
	Utils::Vector2 paddlePos{ 10.f, 768.f / 2.f };
	mPaddles.emplace_back(std::make_unique<Paddle>(paddlePos));
	mGameObjects.emplace_back(mPaddles.back().get());
}

void Game::HandleCollisions(Ball& ball)
{
	bool anyCollisions{ false };
	for (const auto& paddle : mPaddles)
	{
		anyCollisions &= CheckBallPaddle(ball, *paddle);
	}
	if (anyCollisions)
	{
		return;
	}

	// Did the ball go off the screen? (if so, end game)
	if (ball.mPosition.x <= GetExtents().MinX(ball.mThickness))
	{
		mIsRunning = false;
	}

	for (const auto& wall : mWalls)
	{
		CheckBallWall(ball, *wall);
	}
}

bool Game::CheckBallPaddle(Ball& ball, const Paddle& paddle)
{
	// Did we intersect with the paddle?
	float diffY = std::abs(paddle.mPosition.y - ball.mPosition.y);
	float diffX = std::abs(paddle.mPosition.x - ball.mPosition.x);
	if (
		// Our y-difference is small enough
		diffY <= paddle.mHeight / 2.0f &&
		// We are in the correct x-position
		diffX <= (paddle.mWidth + ball.mThickness) / 2.f &&
		// The ball is moving to the left
		ball.mVelocity.x < 0.0f)
	{
		ball.mVelocity.x *= -1.0f;
		return true;
	}

	return false;
}

bool Game::CheckBallWall(Ball& ball, const Wall& wall)
{
	// Calculate the ball's radius
	float ballRadius = ball.mThickness / 2.0f;

	// Define the boundaries of the ball
	float ballLeft = ball.mPosition.x - ballRadius;
	float ballRight = ball.mPosition.x + ballRadius;
	float ballTop = ball.mPosition.y - ballRadius;
	float ballBottom = ball.mPosition.y + ballRadius;

	// Define the boundaries of the wall
	float wallLeft = static_cast<float>(wall.mOriginX);
	float wallRight = static_cast<float>(wall.mOriginX + wall.mWidth);
	float wallTop = static_cast<float>(wall.mOriginY);
	float wallBottom = static_cast<float>(wall.mOriginY + wall.mHeight);

	// Boolean flag to check if collision happened
	bool collided = false;

	// Check for collisions on each side and reflect the ball accordingly
	// 1. Check if the ball hits the left wall
	if (ballRight >= wallLeft && ballLeft <= wallLeft && ball.mPosition.x < wallLeft) {
		ball.mVelocity.x = -ball.mVelocity.x;  // Reverse horizontal velocity
		ball.mPosition.x = wallLeft - ballRadius;  // Correct the position
		return true;
	}

	// 2. Check if the ball hits the right wall
	if (ballLeft <= wallRight && ballRight >= wallRight && ball.mPosition.x > wallRight) {
		ball.mVelocity.x = -ball.mVelocity.x;  // Reverse horizontal velocity
		ball.mPosition.x = wallRight + ballRadius;  // Correct the position
		return true;
	}

	// 3. Check if the ball hits the top wall
	if (ballBottom >= wallTop && ballTop <= wallTop && ball.mPosition.y < wallTop) {
		ball.mVelocity.y = -ball.mVelocity.y;  // Reverse vertical velocity
		ball.mPosition.y = wallTop - ballRadius;  // Correct the position
		return true;
	}
	
	// 4. Check if the ball hits the bottom wall
	if (ballTop <= wallBottom && ballBottom >= wallBottom && ball.mPosition.y > wallBottom) {
		ball.mVelocity.y = -ball.mVelocity.y;  // Reverse vertical velocity
		ball.mPosition.y = wallBottom + ballRadius;  // Correct the position
		return true;
	}

	return false;  // No collision occurred
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

Utils::Extents& Game::GetExtents()
{
	static Utils::Extents sExtents = {
		.top = 0,
		.bottom = 768,
		.right = 1024,
		.left = 0,
		.thickness = Utils::defaultThickness
	};

	return sExtents;
}
