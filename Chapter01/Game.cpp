// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

#include <string>
#include <format>
#include <iostream>
#include <type_traits>
#include <thread>
#include <atomic>
#include <ctime>

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

	InitText();

	return true;
}

void Game::RunLoop()
{
	while (mGameState != GameState::GameOver)
	{
		if (mGameState == GameState::Playing)
		{
			ProcessInput();
			UpdateGame();
		}
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
				mGameState == GameState::GameOver;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mGameState == GameState::GameOver;
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
	
	for (auto& paddle : mPaddles)
	{
		if (paddle->IsAIControlled())
		{
			paddle->MockInputAI(deltaTime, mBalls);
		}
		if (paddle->HasLost())
		{
			mGameState = GameState::ShowingScores;
			mPlayerWon = paddle->IsAIControlled();
		}
	}

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
		mBackgroundColor.r,		// R
		mBackgroundColor.g,		// G 
		mBackgroundColor.b,		// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);
	
	for (const auto& go : mGameObjects)
	{
		const SDL_Color& goDrawColor = go->GetColor();
		SDL_SetRenderDrawColor(mRenderer, goDrawColor.r, goDrawColor.g, goDrawColor.b, goDrawColor.a);
		SDL_RenderFillRect(mRenderer, &(go->GetRect()));
	}
	
	if (mGameState == GameState::Playing)
	{
		PrintScores();
	}
	else
	{
		std::thread([this]() {
			std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait for 5 seconds
			mGameState = GameState::GameOver;
		}).detach();  // Detach the thread so it runs independently
		ShowEndScreen();
	}

	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
	for (const Ball ballToSpawn : mBallsToSpawn)
	{
		mBalls.emplace_back(std::make_unique<Ball>(ballToSpawn));
		mGameObjects.push_back(mBalls.back().get());
	}
	mBallsToSpawn.clear();
}

void Game::PrintScores()
{
	if (mFont == nullptr)
	{
		return;
	}

	// this is the color in rgb format,
	// maxing out all would give you the color white,
	// and it will be your text's color
	static SDL_Color White = { 255, 255, 255 };

	// as TTF_RenderText_Solid could only be used on
	// SDL_Surface then you have to create the surface first
	std::string scoreMessage = std::format("Score: {} - {}", mPaddles.front()->GetScore(), mPaddles.back()->GetScore());
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(mFont, scoreMessage.data(), White);

	// now you can convert it into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(mRenderer, surfaceMessage);

	SDL_Rect Message_rect{}; //create a rect
	Message_rect.x = Utils::defaultThickness + 20;  //controls the rect's x coordinate 
	Message_rect.y = Utils::defaultThickness + 20; // controls the rect's y coordinte
	Message_rect.w = 200; // controls the width of the rect
	Message_rect.h = 50; // controls the height of the rect

	// (0,0) is on the top left of the window/screen,
	// think a rect as the text's box,
	// that way it would be very simple to understand

	// Now since it's a texture, you have to put RenderCopy
	// in your game loop area, the area where the whole code executes

	// you put the renderer's name first, the Message,
	// the crop size (you can ignore this if you don't want
	// to dabble with cropping), and the rect which is the size
	// and coordinate of your texture
	SDL_RenderCopy(mRenderer, Message, NULL, &Message_rect);

	// Don't forget to free your surface and texture
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

void Game::ShowEndScreen()
{
	if (mFont == nullptr)
	{
		return;
	}

	// this is the color in rgb format,
	// maxing out all would give you the color white,
	// and it will be your text's color
	static SDL_Color ColorToUse;
	static std::atomic<bool> lastColorWasWhite{ false };
	
	static std::thread flasher([this]() 
	{
		while (true) 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			static SDL_Color White = { 255, 255, 255 };
			static SDL_Color Black = { 0, 0, 0 };

			ColorToUse = lastColorWasWhite ? Black : White;
			mBackgroundColor = lastColorWasWhite ? White : Black;
			lastColorWasWhite = !lastColorWasWhite;
		}
	});


	if (!flasher.joinable()) {
		flasher.detach();
	}

	// as TTF_RenderText_Solid could only be used on
	// SDL_Surface then you have to create the surface first
	const char* scoreFormatString = mPlayerWon ? "YOU'RE WINNER! Final score: {} - {}" : "You lost! Final score: {} - {}";
	std::string playerScore = std::to_string(mPaddles.front()->GetScore());
	std::string aiScore = std::to_string(mPaddles.back()->GetScore());
	std::string scoreMessage = std::vformat(scoreFormatString, std::make_format_args(playerScore, aiScore));

	TTF_SetFontStyle(mFont, TTF_STYLE_BOLD);
	TTF_SetFontStyle(mFont, TTF_STYLE_UNDERLINE);
	TTF_SetFontStyle(mFont, TTF_STYLE_ITALIC);
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(mFont, scoreMessage.data(), ColorToUse);

	// now you can convert it into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(mRenderer, surfaceMessage);

	SDL_Rect Message_rect{};
	Message_rect.x = GetExtents().Center().x;
	Message_rect.y = GetExtents().Center().y;
	Message_rect.w = 200;
	Message_rect.h = 50;

	SDL_RenderCopy(mRenderer, Message, NULL, &Message_rect);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

void Game::SetupWalls()
{
	int orgX = 0;
	int orgY = 0;
	int width = 1024;
	int height = Utils::defaultThickness;

	// top wall
	mWalls.emplace_back(std::make_unique<Wall>(orgX, orgY, width, height));
	mGameObjects.emplace_back(mWalls.back().get());

	// bottom wall
	orgY = 768 - Utils::defaultThickness;
	mWalls.emplace_back(std::make_unique<Wall>(orgX, orgY, width, height));
	mGameObjects.emplace_back(mWalls.back().get());

	// #TODO: We could have data-defined different setups, for example for number of players or shape of walls
	// right wall
	//orgX = 1024 - Utils::defaultThickness;
	//orgY = 0;
	//width = Utils::defaultThickness;
	//height = 1024;
	//mWalls.emplace_back(std::make_unique<Wall>(orgX, orgY, width, height));
	//mGameObjects.emplace_back(mWalls.back().get());
}

void Game::SetupBalls()
{
	Utils::Vector2 ballPos{ 1024.f / 2.f, 768.f / 2.f };
	Utils::Vector2 ballVel{ -200.f, 235.f };
	SpawnBall(ballPos, ballVel);
	
}

void Game::SpawnBall(Utils::Vector2 initialPos, Utils::Vector2 initialVel)
{
	static std::vector<SDL_Color> ballColors = {
	{ 255, 255, 255, 255 },
	{ 255, 0, 0, 255 },
	{ 0, 255, 0, 255 },
	{ 255, 255, 0, 255 },
	{ 0, 255, 255, 255 },
	{ 255, 0, 255, 255 },
	{ 0, 0, 0, 255 }
	};

	SDL_Color ballColor = { 255, 255, 255, 255 };
	if (ballColors.size() > 0)
	{
		const int ballIndex = rand() % ballColors.size();
		ballColor = ballColors[ballIndex];
		ballColors.erase(ballColors.begin() + ballIndex);
	}
	const std::optional<float> ballThickness = std::nullopt; // keep default
	mBallsToSpawn.emplace_back(Ball(initialPos, initialVel, ballThickness, ballColor));
}

void Game::SetupPaddles()
{
	Utils::Vector2 paddlePos{ 10.f, 768.f / 2.f };
	mPaddles.emplace_back(std::make_unique<Paddle>(paddlePos));
	mGameObjects.emplace_back(mPaddles.back().get());

	paddlePos = { 1024.f - 10.f, 768.f / 2.f };
	PaddleControls aiPaddleControls{ .mIsHuman = false, .mAIStrength = PaddleControls::Difficulty::Medium, .mUpButton = SDL_Scancode::SDL_NUM_SCANCODES, .mDownButton = SDL_Scancode::SDL_NUM_SCANCODES };
	//PaddleControls humanPaddleControls{ .mIsHuman = true, .mAIStrength = PaddleControls::Difficulty::None, .mUpButton = SDL_SCANCODE_UP, .mDownButton = SDL_SCANCODE_DOWN };
	mPaddles.emplace_back(std::make_unique<Paddle>(paddlePos, aiPaddleControls));
	mGameObjects.emplace_back(mPaddles.back().get());
}

void Game::InitText()
{
	//this opens a font style and sets a size
	TTF_Init();
	mFont = TTF_OpenFont("Data/whatnot.ttf", 99);
	if (mFont == nullptr)
	{
		std::string error = TTF_GetError();
		std::cout << error;
	}
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
	if (GetExtents().IsInside(ball.mPosition, ball.mThickness) == false)
	{
		HandleBallExited(ball);
	}

	for (const auto& wall : mWalls)
	{
		CheckBallWall(ball, *wall);
	}
}

bool Game::CheckBallPaddle(Ball& ball, Paddle& paddle)
{
	// Did we intersect with the paddle?
	float diffY = std::abs(paddle.mPosition.y - ball.mPosition.y);
	float diffX = std::abs(paddle.mPosition.x - ball.mPosition.x);
	if (
		// Our y-difference is small enough
		diffY <= paddle.mHeight / 2.0f &&
		// We are in the correct x-position
		diffX <= (paddle.mWidth + ball.mThickness) / 2.f)
	{
		ball.mVelocity.x *= -1.0f;
		ball.mLastPaddleTouched = &paddle;
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

void Game::HandleBallExited(Ball& ball)
{
    auto closest_paddle = std::ranges::min_element(mPaddles, std::less{}, [&ball](const std::unique_ptr<Paddle>& paddle) {
        return Utils::Vector2::Distance(ball.mPosition, paddle->mPosition);
    });

	if (closest_paddle != mPaddles.end())
	{
		(*closest_paddle)->LoseScore();
	}
	ball.Reset();

	const int scoreDifference = mPaddles.front()->GetScore() - mPaddles.back()->GetScore();
	
	srand((unsigned)time(0));

	Utils::Vector2 baseVel = Utils::Vector2(200.f, 235.f);
	float velSignX = scoreDifference > 0 ? -1.f : 1.f;
	float velSignY = rand() % 2 == 0 ? -1.f : 1.f;
	Utils::Vector2 newBallVel{ (baseVel.x + rand() % 30) * velSignX, (baseVel.y + rand() % 30) * velSignY };

	Utils::Vector2 newBallPos{ ball.mPosition.x - (100.f + 35.f * (rand() % mBalls.size())) * scoreDifference, ball.mPosition.y + 35.f * (rand() % mBalls.size()) * -velSignY};

	SpawnBall(newBallPos, newBallVel);
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
