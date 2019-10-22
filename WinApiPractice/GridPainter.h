#pragma once
#include "Windows.h"
#include "libpic.h"

class GridPainter
{
public:
	explicit GridPainter(HWND window);

	void Draw(int row, int column, image& img);
	void DrawGrid();

	~GridPainter();

private:
	HDC hdc;
	PAINTSTRUCT ps{};
	HWND& window;
};
