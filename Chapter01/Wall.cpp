#include "Wall.h"

Wall::Wall(int orgX, int orgY, int width, int height)
	: mOriginX{ orgX }
	, mOriginY{ orgY }
	, mWidth{ width }
	, mHeight{height}
	, mSDLRect{ mOriginX, mOriginY, mWidth, mHeight }
{
}