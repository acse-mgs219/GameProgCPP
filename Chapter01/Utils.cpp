#include "Utils.h"

#include <cmath>

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

	Vector2 Extents::Center() const
	{
		return Vector2((left + right) / 2.f, (top + bottom) / 2.f);
	}

	bool Extents::IsInside(Vector2 pos, float thickness) const
	{
		return pos.x > MinX(thickness)
			&& pos.x < MaxX(thickness)
			&& pos.y >= MinY(thickness)
			&& pos.y <= MaxY(thickness);
	}

	float Vector2::Distance(const Vector2& vec1, const Vector2& vec2)
	{
		return std::sqrtf(std::powf((vec1.x - vec2.x), 2) + std::powf((vec1.y - vec2.y), 2));
	}
}