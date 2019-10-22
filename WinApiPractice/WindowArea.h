#pragma once

struct WindowArea
{
	WindowArea() = default;
	WindowArea(int centerX, int centerY, int radius);

	int centerX;
	int centerY;
	int radius;
};
