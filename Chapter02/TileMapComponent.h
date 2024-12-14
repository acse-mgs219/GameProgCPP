#pragma once

#include "SpriteComponent.h"

#include <string_view>
#include <vector>

class TileMapComponent : public SpriteComponent
{
public:
	TileMapComponent(class Actor* owner, int drawOrder = 100);
	~TileMapComponent();

	void Draw(SDL_Renderer* renderer) override;
	void SetTextures(SDL_Texture* texture, std::string_view tileMapFilepath, int width, int height);

private:
	std::vector<std::vector<int>> mTileMap;
};