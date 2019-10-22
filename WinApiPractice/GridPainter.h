#pragma once
#include "Windows.h"
#include "libpic.h"
#include "WindowArea.h"

class GridPainter
{
public:
	explicit GridPainter(HWND& window, int dimension);

	void DrawImage(WindowArea area, image& img) const;
	void DrawGrid(COLORREF gridColor) const;
	void DrawIconsOnGrid(const unsigned* circles) const;
	void DrawCircle(WindowArea area) const;
	void DrawCross(WindowArea area) const;

	HDC& GetHDC();

	GridPainter(const GridPainter& other) = delete;
	GridPainter(GridPainter&& other) noexcept = delete;
	GridPainter& operator=(const GridPainter& other) = delete;
	GridPainter& operator=(GridPainter&& other) noexcept = delete;

	~GridPainter();

private:
	HDC hdc;
	PAINTSTRUCT ps{};
	HWND& window;
	int dimension;

	[[nodiscard]] WindowArea CalculateIconDimensions(UINT index) const;
};
