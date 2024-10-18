// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Ball.h"
#include "Paddle.h"
#include "Wall.h"
#include "Utils.h"

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include <memory>
#include <vector>
#include <utility>

enum class GameState
{
	Playing,
	ShowingScores,
	GameOver
};

// Game class
class Game
{
public:
	Game() = default;
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();

	static Utils::Extents& GetExtents();
private:
	// Helper functions for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void PrintScores();
	void ShowEndScreen();

	// Pong specific helper functions
	void SetupWalls();
	void SetupBalls();
	void SetupPaddles();

	void InitText();
	void HandleCollisions(Ball& ball);
	bool CheckBallPaddle(Ball& ball, Paddle& paddle);
	bool CheckBallWall(Ball& ball, const Wall& wall);
	void HandleBallExited(Ball& ball);

	// Window created by SDL
	SDL_Window* mWindow{ nullptr };
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer{ nullptr };
	// Background color - blue initially
	SDL_Color mBackgroundColor{ 0, 0, 255 };
	// Font we use for showing the scores
	TTF_Font* mFont{ nullptr };
	// Number of ticks since start of game
	Uint32 mTicksCount{ 0 };
	// Game should continue to run
	GameState mGameState{GameState::Playing};
	// PlayerWon == false -> if we're checking this variable, implies AI won
	bool mPlayerWon{ false };
	
	// Pong specific
	std::vector<std::unique_ptr<Paddle>> mPaddles;
	std::vector<std::unique_ptr<Ball>> mBalls;
	std::vector<std::unique_ptr<Wall>> mWalls;
	std::vector<IGameObject*> mGameObjects;
	std::pair<int, int> mScores;
};
