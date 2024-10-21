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

struct AnimSequence
{
	int StartFrame;
	int EndFrame;
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
	void CreateAnimSequence(AnimSequence&& seq, std::string name);
	void SetAnimSequence(const std::string& name, int repeatCount = -1);
private:
	// All textures in the animation
	std::vector<SDL_Texture*> mAnimTextures;
	// Current frame displayed
	float mCurrFrame;
	// Animation frame rate
	float mAnimFPS;

	// Support different animation sequences
	std::unordered_map<std::string, AnimSequence> mAnimSequences;
	const AnimSequence* mCurrentAnim;
	int mLoopCount; // negative value to loop forever

	static const std::string sFullSequence;
};
