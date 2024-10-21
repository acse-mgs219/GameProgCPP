// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AnimSpriteComponent.h"
#include "Math.h"

const std::string AnimSpriteComponent::sFullSequence = "Full";

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder)
	:SpriteComponent(owner, drawOrder)
{
}

void AnimSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

	if (mAnimTextures.size() == 0 || mAnimState != AnimState::Playing)
	{
		return;
	}

	// Update the current frame based on frame rate
	// and delta time
	mCurrFrame += mAnimFPS * deltaTime;
		
	// Wrap current frame if needed
	if (mCurrFrame >= mCurrentAnim->EndFrame)
	{
		if (mLoopCount-- != 0)
		{
			while (mCurrFrame >= mCurrentAnim->EndFrame)
			{
				mCurrFrame -= mCurrentAnim->NumFrames;
			}
		}
		else
		{
			mCurrFrame = static_cast<float>(mCurrentAnim->EndFrame);
			mAnimState = AnimState::Stopped;
		}
	}

	// Set the current texture
	SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
}

void AnimSpriteComponent::SetAnimTextures(const std::vector<SDL_Texture*>& textures)
{
	mAnimTextures = textures;
	const int animFrames = mAnimTextures.size();
	if (animFrames > 0)
	{
		// Set the active texture to first frame
		mCurrFrame = 0.0f;
		SetTexture(mAnimTextures[0]);
		CreateAnimSequence({ 0, animFrames - 1 }, sFullSequence);
		SetAnimSequence(sFullSequence);
		mAnimState = AnimState::Playing;
	}
}

void AnimSpriteComponent::CreateAnimSequence(AnimSequence&& seq, std::string name, std::optional<int> startWithRepeatCount)
{
	mAnimSequences.emplace(name, std::move(seq));
	if (startWithRepeatCount.has_value())
	{
		SetAnimSequence(name, startWithRepeatCount.value());
	}
}

void AnimSpriteComponent::SetAnimSequence(const std::string& name, int repeatCount)
{
	auto iter = mAnimSequences.find(name);
	if (iter != mAnimSequences.end())
	{
		mCurrentAnim = &iter->second;
		mCurrFrame = static_cast<float>(mCurrentAnim->StartFrame);
		mLoopCount = repeatCount;
	}
}
