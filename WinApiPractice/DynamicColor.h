#pragma once
#include <vector>
#include "Color.h"

class DynamicColor
{
public:
	explicit DynamicColor(std::vector<Color> colors, float speed = 0.01f);

	void Change();
	[[nodiscard]] Color CurrentColor() const;

private:
	float progress = 0.0;
	float speed;
	Color currentColor;
	std::vector<Color> colors;
};
