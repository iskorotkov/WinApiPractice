#pragma once
#include "Windows.h"
#include "libpic.h"
#include "WindowArea.h"

class GridPainter
{
public:
	explicit GridPainter(HWND& window, int dimension);

	void DrawImage(int row, int column, image& img) const;
	void DrawGrid(COLORREF gridColor) const;
	void DrawIconsOnGrid(const unsigned* circles);
	void DrawCircle(UINT radius, UINT centerX, UINT centerY);
	void DrawCross(UINT radius, UINT centerX, UINT centerY);

	HDC& GetHDC();

	~GridPainter();

private:
	HDC hdc;
	PAINTSTRUCT ps{};
	HWND& window;
	int dimension;

	[[nodiscard]] WindowArea CalculateIconDimensions(UINT index) const;
};
