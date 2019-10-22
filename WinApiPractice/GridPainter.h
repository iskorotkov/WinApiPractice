#pragma once
#include "Windows.h"
#include "libpic.h"
#include "WindowArea.h"
#include <functional>

class GridPainter
{
public:
	explicit GridPainter(HWND& window, int dimension);

	void DrawImage(WindowArea area, Image& img) const;
	void DrawImageWhere(int value, const int* values, Image& img) const;
	void DrawGrid(COLORREF gridColor) const;
	void DrawIconsOnGrid(const int* values) const;
	void DrawCircle(WindowArea area) const;
	void DrawCross(WindowArea area) const;

	// ReSharper disable once CppInconsistentNaming
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

	using CellIndex = int;
	using CellValue = int;
	void ForEachCell(const int* values, const std::function<void(CellIndex, CellValue)>& callback, bool ignoreZero = true) const;
};
