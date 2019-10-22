#include "GridPainter.h"
#include <stdexcept>

GridPainter::GridPainter(HWND& window, const int dimension) : window(window), dimension(dimension)
{
	hdc = BeginPaint(window, &ps);
}

void GridPainter::DrawImage(const WindowArea area, image& img) const
{
	const auto hBitmap = CreateBitmap(img.width, img.height, 1, img.bit_depth, img.buffer);
	const auto hdcMem = CreateCompatibleDC(hdc);

	SelectObject(hdcMem, hBitmap);
	TransparentBlt(hdc, 0, 0, img.width, img.height, hdcMem, 0, 0, img.width, img.height, RGB(0, 0, 0));

	DeleteDC(hdcMem);
	DeleteObject(hBitmap);
}

void GridPainter::DrawImageWhere(int value, const unsigned* circles, image& img) const
{
	const auto callback = [this, value, &img](auto index, auto val)
	{
		if (val == value)
		{
			const auto area = CalculateIconDimensions(index);
			DrawImage(area, img);
		}
	};
	ForEachCell(circles, callback);
}

void GridPainter::DrawGrid(COLORREF gridColor) const
{
	RECT rect;
	GetClientRect(window, &rect);
	const HPEN pen = CreatePen(0, 2, gridColor);
	const HGDIOBJ prevBrush = SelectObject(hdc, pen);

	for (UINT i = 1u; i < dimension; ++i)
	{
		const UINT x = rect.right * i / dimension;
		MoveToEx(hdc, x, 0, nullptr);
		LineTo(hdc, x, rect.bottom);

		const UINT y = rect.bottom * i / dimension;
		MoveToEx(hdc, 0, y, nullptr);
		LineTo(hdc, rect.right, y);
	}

	SelectObject(hdc, prevBrush);
	DeleteObject(pen);
}

GridPainter::~GridPainter()
{
	EndPaint(window, &ps);
}

void GridPainter::DrawCircle(const WindowArea area) const
{
	const auto hBrush = CreateSolidBrush(RGB(255, 255, 255));
	const auto prevBrush = SelectObject(hdc, hBrush);

	Ellipse(hdc,
		area.centerX - area.radius,
		area.centerY - area.radius,
		area.centerX + area.radius,
		area.centerY + area.radius);

	SelectObject(hdc, prevBrush);
	DeleteObject(hBrush);
}

void GridPainter::DrawCross(const WindowArea area) const
{
	// Draws '\'
	MoveToEx(hdc,
		area.centerX - area.radius,
		area.centerY - area.radius,
		nullptr);
	LineTo(hdc,
		area.centerX + area.radius,
		area.centerY + area.radius);
	// Draws '/'
	MoveToEx(hdc,
		area.centerX + area.radius,
		area.centerY - area.radius,
		nullptr);
	LineTo(hdc,
		area.centerX - area.radius,
		area.centerY + area.radius);
}

HDC& GridPainter::GetHDC()
{
	return hdc;
}

WindowArea GridPainter::CalculateIconDimensions(const UINT index) const
{
	const auto row = index % dimension;
	const auto col = index / dimension;
	RECT rect;
	GetClientRect(window, &rect);
	const UINT height = rect.bottom;
	const UINT width = rect.right;
	WindowArea area;
	area.radius = min(height/dimension, width/dimension) / 3;
	area.centerX = width * (2 * row + 1) / (2 * dimension);
	area.centerY = height * (2 * col + 1) / (2 * dimension);
	return area;
}

void GridPainter::ForEachCell(const unsigned* circles, const std::function<void(CellIndex, CellValue)>& callback, const bool ignoreZero) const
{
	for (UINT i = 0u, len = dimension * dimension; i < len; ++i)
	{
		if (circles[i] || !ignoreZero)
		{
			callback(i, circles[i]);
		}
	}
}

void GridPainter::DrawIconsOnGrid(const unsigned* circles) const
{
	const auto callback = [this](auto index, auto val)
	{
		const auto area = CalculateIconDimensions(index);
		switch (val)
		{
			case 1:
			{
				DrawCircle(area);
				break;
			}
			case 2:
			{
				DrawCross(area);
				break;
			}
			default:
				throw std::logic_error("Unknown grid value.");
		}
	};
	ForEachCell(circles, callback);
}
