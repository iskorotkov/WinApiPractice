#include "Color.h"
#include "Math.h"

Color::Color(const int red, const int green, const int blue) : red(red), green(green), blue(blue)
{
}

Color Color::Red()
{
	return Color(WHITE, BLACK, BLACK);
}

Color Color::Green()
{
	return Color(BLACK, WHITE, BLACK);
}

Color Color::Blue()
{
	return Color(BLACK, BLACK, WHITE);
}

COLORREF Color::AsColorRef() const
{
	return RGB(red, green, blue);
}

Color Color::operator+(const Color& other) const
{
	Color c;
	c.red = Math::Clamp(red + other.red, BLACK, WHITE);
	c.green = Math::Clamp(green + other.green, BLACK, WHITE);
	c.blue = Math::Clamp(blue + other.blue, BLACK, WHITE);
	return c;
}

Color Color::operator*(const float multiplier) const
{
	Color c;
	c.red = Math::Clamp(red * multiplier, BLACK, WHITE);     // NOLINT(bugprone-narrowing-conversions)
	c.green = Math::Clamp(green * multiplier, BLACK, WHITE); // NOLINT(bugprone-narrowing-conversions)
	c.blue = Math::Clamp(blue * multiplier, BLACK, WHITE);   // NOLINT(bugprone-narrowing-conversions)
	return c;
}
