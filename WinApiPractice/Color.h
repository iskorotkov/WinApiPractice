#pragma once
#include <Windows.h>

constexpr int BLACK = 0;
constexpr int WHITE = 255;

struct Color
{
	Color() = default;
	Color(int red, int green, int blue);
	
	int red = BLACK;
	int green = BLACK;
	int blue = BLACK;

	static Color Red();
	static Color Green();
	static Color Blue();

	[[nodiscard]] COLORREF AsColorRef() const;
	
	Color operator+(const Color& other) const;
	Color operator*(float multiplier) const;
};
