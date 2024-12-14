#include "TileMapComponent.h"

#include "Actor.h"
#include "Game.h"

#include <fstream>
#include <sstream>
#include <SDL/SDL_rect.h>

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder)
	: SpriteComponent(owner, drawOrder)
{
}

TileMapComponent::~TileMapComponent()
{
	mOwner->GetGame()->RemoveSprite(this);
}

void TileMapComponent::Draw(SDL_Renderer* renderer)
{
	if (mTexture == nullptr || mTileMap.empty())
	{
		return;
	}

	const int tileMapWidth = mTileMap[0].size();
	const int tileMapHeight = mTileMap.size();
	
	const int textureWidth = mTexWidth / tileMapWidth;
	const int textureHeight = mTexHeight / tileMapHeight;
	for (size_t y = 0; y < tileMapHeight; ++y)
	{
		for (size_t x = 0; x < tileMapWidth; ++x)
		{
			SDL_Rect sourceRect;
			sourceRect.w = textureWidth;
			sourceRect.h = textureHeight;
			sourceRect.x = x * textureWidth;
			sourceRect.y = y * textureHeight;

			SDL_Rect destRect;
			// Scale the width/height by owner's scale
			destRect.w = static_cast<int>(mTexWidth * mOwner->GetScale());
			destRect.h = static_cast<int>(mTexHeight * mOwner->GetScale());
			// Center the rectangle around the position of the owner
			destRect.x = static_cast<int>(mOwner->GetPosition().x - destRect.w / 2);
			destRect.y = static_cast<int>(mOwner->GetPosition().y - destRect.h / 2);

			// Draw (have to convert angle from radians to degrees, and clockwise to counter)
			SDL_RenderCopyEx(renderer,
				mTexture,
				&sourceRect,
				&destRect,
				-Math::ToDegrees(mOwner->GetRotation()),
				nullptr,
				SDL_FLIP_NONE);
		}
	}
}

void TileMapComponent::SetTextures(SDL_Texture* texture, std::string_view tileMapFilepath)
{
	SpriteComponent::SetTexture(texture);
	
	std::ifstream file(tileMapFilepath.data());
	mTileMap.clear();

	std::string line;

	while (std::getline(file, line))
	{
		std::vector<int> row;
		std::stringstream iss(line);
		int c = 0;

		while (iss << c << ",")
		{
			row.push_back(c);
		}
		mTileMap.push_back(row);
	}
}
