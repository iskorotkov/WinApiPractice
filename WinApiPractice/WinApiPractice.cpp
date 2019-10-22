#include <Windows.h>
#include <tchar.h>
#include <cstdio>
#include <cstdlib>
#include <windowsx.h>
#include <WinUser.h>
#include "files.h"
#include "commandline.h"
#include "preferences.h"
#include <string>
#include "libpic.h"

#define ASSET_PATH(filename) ("C:\\Projects\\WinApiPractice\\Assets\\" filename)

const TCHAR CZ_WIN_CLASS[] = _T("MyClassName");
const TCHAR CZ_WIN_NAME[] = _T("MyWindowName");

const auto crossPath = ASSET_PATH("Cross.png");
const auto circlePath = ASSET_PATH("Circle.png");

Preferences* prefs;
image crossImage;
image circleImage;
image icon;
image cursor;

UINT GetDimension()
{
	if (prefs)
	{
		return prefs->GridSize;
	}
	_tprintf(L"Error: preferences are NULL");
	return 1;
}

#define GRID_DIMENSION (GetDimension())

HBRUSH hCurrentBrush;
UINT* circles;

void RunNotepad()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	ZeroMemory(&sInfo, sizeof(STARTUPINFO));

	CreateProcess(_T("C:\\Windows\\Notepad.exe"), nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &sInfo, &pInfo);
}

UINT Random(const UINT max)
{
	return rand() % (max + 1);
}

COLORREF GetRandomColor()
{
	return RGB(Random(255), Random(255), Random(255));
}

void OnClicked(const HWND hwnd, UINT x, UINT y, const UINT value)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	x = x * GRID_DIMENSION / rect.right;
	y = y * GRID_DIMENSION / rect.bottom;
	const UINT index = y * GRID_DIMENSION + x;
	circles[index] = value;

	InvalidateRect(hwnd, nullptr, true);
	UpdateWindow(hwnd);
}

void DrawCircle(HDC hdc, UINT radius, UINT centerX, UINT centerY)
{
	const HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	const HGDIOBJ prevBrush = SelectObject(hdc, hBrush);

	Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

	SelectObject(hdc, prevBrush);
	DeleteObject(hBrush);
}

void DrawCross(HDC hdc, UINT radius, UINT centerX, UINT centerY)
{
	// Draws '\'
	MoveToEx(hdc, centerX - radius, centerY - radius, nullptr);
	LineTo(hdc, centerX + radius, centerY + radius);
	// Draws '/'
	MoveToEx(hdc, centerX + radius, centerY - radius, nullptr);
	LineTo(hdc, centerX - radius, centerY + radius);
}

void CalculateIconDimensions(HWND hwnd, UINT i, UINT& radius, UINT& centerX, UINT& centerY)
{
	const UINT row = i % GRID_DIMENSION;
	const UINT col = i / GRID_DIMENSION;
	RECT rect;
	GetClientRect(hwnd, &rect);
	const UINT height = rect.bottom;
	const UINT width = rect.right;
	radius = min(height/GRID_DIMENSION, width/GRID_DIMENSION) / 3;
	centerX = width * (2 * row + 1) / (2 * GRID_DIMENSION);
	centerY = height * (2 * col + 1) / (2 * GRID_DIMENSION);
}

void DrawIconsOnGrid(HWND hwnd, HDC hdc)
{
	for (UINT i = 0u, len = GRID_DIMENSION * GRID_DIMENSION; i < len; ++i)
	{
		if (circles[i])
		{
			UINT radius;
			UINT centerX;
			UINT centerY;
			CalculateIconDimensions(hwnd, i, radius, centerX, centerY);
			switch (circles[i])
			{
				case 1:
				{
					DrawCircle(hdc, radius, centerX, centerY);
					break;
				}
				case 2:
				{
					DrawCross(hdc, radius, centerX, centerY);
					break;
				}
			}
		}
	}
}

void DrawGrid(HWND hwnd, HDC hdc)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	const HPEN pen = CreatePen(0, 2, prefs->GridColor);
	const HGDIOBJ prevBrush = SelectObject(hdc, pen);

	for (UINT i = 1u; i < GRID_DIMENSION; ++i)
	{
		const UINT x = rect.right * i / GRID_DIMENSION;
		MoveToEx(hdc, x, 0, nullptr);
		LineTo(hdc, x, rect.bottom);

		const UINT y = rect.bottom * i / GRID_DIMENSION;
		MoveToEx(hdc, 0, y, nullptr);
		LineTo(hdc, rect.right, y);
	}

	DrawIconsOnGrid(hwnd, hdc);

	SelectObject(hdc, prevBrush);
	DeleteObject(pen);
}

HDC StretchImage(HDC source, int x, int y)
{
	const auto stretched = CreateCompatibleDC(source);

	RECT rect;
	GetBoundsRect(source, &rect, 0);
	StretchBlt(stretched, 0, 0, x, y, source, 0, 0, rect.right, rect.bottom, COLORONCOLOR);

	return stretched;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE
				|| wParam == 'Q' && GetKeyState(VK_CONTROL) & 0x8000)
			{
				PostQuitMessage(0);
				return 0;
			}
			if (wParam == 'C' && GetKeyState(VK_SHIFT) & 0x8000)
			{
				RunNotepad();
			}
			else if (wParam == VK_RETURN)
			{
				const COLORREF NewColor = GetRandomColor();
				if (prefs)
				{
					prefs->BackgroundColor = NewColor;
				}
				HBRUSH hBrush = CreateSolidBrush(NewColor);
				SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
				DeleteObject(hCurrentBrush);
				hCurrentBrush = hBrush;

				InvalidateRect(hwnd, nullptr, true);
				UpdateWindow(hwnd);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SIZING:
		{
			if (prefs)
			{
				RECT rect;
				GetClientRect(hwnd, &rect);
				prefs->WindowWidth = rect.right;
				prefs->WindowHeight = rect.bottom;
			}
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			const HDC hdc = BeginPaint(hwnd, &ps);
			DrawGrid(hwnd, hdc);
			// ============================
			HBITMAP hBitmap = CreateBitmap(crossImage.width, crossImage.height, 1, crossImage.bit_depth, crossImage.buffer);
			BITMAP bitmap;
			GetObject(hBitmap, sizeof(BITMAP), &bitmap);

			auto hdcMem = CreateCompatibleDC(hdc);
			auto oldBitmap = SelectObject(hdcMem, hBitmap);

			RECT rect;
			GetClientRect(hwnd, &rect);
			TransparentBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0, 0, 0));

			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);
			DeleteObject(hBitmap);
			// ============================
			EndPaint(hwnd, &ps);
		}
		break;
		case WM_LBUTTONDOWN:
		{
			const UINT x = GET_X_LPARAM(lParam);
			const UINT y = GET_Y_LPARAM(lParam);
			OnClicked(hwnd, x, y, 1);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const UINT x = GET_X_LPARAM(lParam);
			const UINT y = GET_Y_LPARAM(lParam);
			OnClicked(hwnd, x, y, 2);
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main(int argc, char** argv)
{
	const ReadingMethod method = GetReadingMethod(argc, argv);
	prefs = ReadConfigFile(method);
	if (prefs == nullptr)
	{
		MessageBox(nullptr, L"Failed to read config file!", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	// ===============================
	using LoadImageFunc = decltype(load_image);
	const auto libpic = LoadLibrary(L"libpic.dll");
	const auto procAddress = (LoadImageFunc*)GetProcAddress(libpic, "load_image");

	crossImage = procAddress(crossPath);
	circleImage = procAddress(circlePath);
	// ===============================

	const UINT len = GRID_DIMENSION * GRID_DIMENSION;
	circles = new UINT[len];
	ZeroMemory(circles, len * sizeof circles);

	const HINSTANCE hThisInstance = GetModuleHandle(nullptr);

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hInstance = hThisInstance;
	wc.lpszClassName = CZ_WIN_CLASS;
	wc.lpfnWndProc = WindowProcedure;

	hCurrentBrush = CreateSolidBrush(prefs->BackgroundColor);
	wc.hbrBackground = hCurrentBrush;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, L"Class registration has failed!", L"Error", MB_OK | MB_ICONERROR);
		const DWORD error = GetLastError();
		printf("Error: %lu\n", error);
		return error;
	}

	const HWND hwnd = CreateWindowEx(
		0,
		CZ_WIN_CLASS,
		CZ_WIN_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		prefs->WindowWidth,
		prefs->WindowHeight,
		HWND_DESKTOP,
		nullptr,
		hThisInstance,
		nullptr
	);

	const UINT nCmdShow = SW_SHOW;
	ShowWindow(hwnd, nCmdShow);

	BOOL bMessageOk;
	MSG message;
	while ((bMessageOk = GetMessage(&message, nullptr, 0, 0)) != 0)
	{
		if (bMessageOk < 0)
		{
			puts("Suddenly, GetMessage failed!");
			break;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	DeleteObject(hCurrentBrush);
	DestroyWindow(hwnd);
	UnregisterClass(CZ_WIN_CLASS, hThisInstance);

	WriteConfigFile(method, prefs);
	delete prefs;
	delete circles;
	return 0;
}
