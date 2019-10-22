#include "GridPainter.h"

GridPainter::GridPainter(const HWND window)
{
	this->window = window;
	hdc = BeginPaint(window, &ps);
}

void GridPainter::Draw(int row, int column, image& img)
{
}

void GridPainter::DrawGrid()
{
}

GridPainter::~GridPainter()
{
	EndPaint(window, &ps);
}
