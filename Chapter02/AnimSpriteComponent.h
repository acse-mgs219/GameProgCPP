// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SpriteComponent.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include <iostream>

struct AnimSequence
{
	/// Zero-based indexing
	AnimSequence(int start, int end)
		:StartFrame(start)
		, EndFrame(end)
		, NumFrames(end - start + 1)
	{
		std::cout << "Constructing Anim Sequence " << StartFrame << " to " << EndFrame << std::endl;
	}

	AnimSequence(AnimSequence&& mov) noexcept
		: StartFrame(std::move(mov.StartFrame))
		, EndFrame(std::move(mov.EndFrame))
		, NumFrames(EndFrame - StartFrame + 1)
	{
		std::cout << "Moving Anim Sequence " << StartFrame << " to " << EndFrame << std::endl;
	}

	int StartFrame;
	int EndFrame;
	int NumFrames;
};

enum class AnimState
{
	Playing,
	Paused,
	Stopped
};

class AnimSpriteComponent : public SpriteComponent
{
public:
	AnimSpriteComponent(class Actor* owner, int drawOrder = 100);
	// Update animation every frame (overridden from component)
	void Update(float deltaTime) override;
	// Set the textures used for animation
	void SetAnimTextures(const std::vector<SDL_Texture*>& textures);
	// Set/get the animation FPS
	float GetAnimFPS() const { return mAnimFPS; }
	void SetAnimFPS(float fps) { mAnimFPS = fps; }

	// Support different animation sequences
	void CreateAnimSequence(AnimSequence&& seq, std::string name, std::optional<int> startWithRepeatCount = std::nullopt);
	void SetAnimSequence(const std::string& name, int repeatCount = -1);
private:
	// All textures in the animation
	std::vector<SDL_Texture*> mAnimTextures;
	// Current frame displayed. Float to represent time til next frame
	float mCurrFrame{0.0f};
	// Animation frame rate
	float mAnimFPS{24.0f};
	AnimState mAnimState{AnimState::Paused};

	// Support different animation sequences
	std::unordered_map<std::string, AnimSequence> mAnimSequences;
	const AnimSequence* mCurrentAnim{nullptr};
	int mLoopCount{0}; // negative value to loop forever

	static const std::string sFullSequence;
};
