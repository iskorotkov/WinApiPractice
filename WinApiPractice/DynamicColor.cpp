#include "DynamicColor.h"
#include "Math.h"
#include <random>

DynamicColor::DynamicColor(std::vector<Color> colors, const float speed) :
	speed(speed),
	currentColor(colors[0]),
	colors(std::move(colors))
{
	std::random_device device;
	const std::uniform_real_distribution<float> distribution(0, this->colors.size());
	progress = distribution(device);
}

void DynamicColor::Change()
{
	const auto prev = Math::Floor(progress);
	const auto next = prev == colors.size() - 1 ? 0 : prev + 1; // NOLINT(clang-diagnostic-sign-compare)

	currentColor = Math::Interpolate(colors[prev], colors[next], progress - prev);
	
	progress += speed;
	if (progress >= colors.size())
	{
		progress = 0.f;
	}
}

Color DynamicColor::CurrentColor() const
{
	return currentColor;
}
