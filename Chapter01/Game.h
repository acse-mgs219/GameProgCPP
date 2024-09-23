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

#include <memory>
#include <vector>
#include <utility>

// Game class
class Game
{
public:
	Game();
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

	// Pong specific helper functions
	void SetupWalls();
	void SetupBalls();
	void SetupPaddles();

	void HandleCollisions(Ball& ball);
	bool CheckBallPaddle(Ball& ball, const Paddle& paddle);
	bool CheckBallWall(Ball& ball, const Wall& wall);
	void HandleBallExited();

	// Window created by SDL
	SDL_Window* mWindow;
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	// Game should continue to run
	bool mIsRunning;
	
	// Pong specific
	std::vector<std::unique_ptr<Paddle>> mPaddles;
	std::vector<std::unique_ptr<Ball>> mBalls;
	std::vector<std::unique_ptr<Wall>> mWalls;
	std::vector<IGameObject*> mGameObjects;
	std::pair<int, int> mScores;
};
