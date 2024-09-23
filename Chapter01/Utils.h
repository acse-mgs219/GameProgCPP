#pragma once

namespace Utils
{
	inline constexpr int defaultThickness = 15;
	// Vector2 struct just stores x/y coordinates
	// (for now)
	struct Vector2
	{
		float x{ .0f };
		float y{ .0f };
	};

	struct Extents
	{
		float MinY(float height) const;
		float MaxY(float height) const;
		float MinX(float width) const;
		float MaxX(float width) const;

		float top;
		float bottom;
		float right;
		float left;
		float thickness;
	};
}