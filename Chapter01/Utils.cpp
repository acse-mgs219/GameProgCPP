#include "Utils.h"

namespace Utils
{
	float Extents::MinY(float height) const
	{
		return top + thickness + height / 2.f;
	}

	float Extents::MaxY(float height) const
	{
		return bottom - height / 2.0f - thickness;
	}

	float Extents::MinX(float width) const
	{
		return left + /*thickness +*/ width / 2.f;
	}

	float Extents::MaxX(float width) const
	{
		return right - width / 2.f /*- thickness*/;
	}
}