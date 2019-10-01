#include <Windows.h>
#include <tchar.h>
#include <cstdio>
#include <cstdlib>
#include <windowsx.h>
#include <WinUser.h>
#include "files.h"
#include "commandline.h"
#include "preferences.h"
#include "filemapping.h"

const TCHAR czWinClass[] = _T("MyClassName");
const TCHAR czWinName[] = _T("MyWindowName");
Preferences* prefs;
UINT dim = 3;

HBRUSH hCurrentBrush;

UINT* circles;

void RunNotepad()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	ZeroMemory(&sInfo, sizeof(STARTUPINFO));

	CreateProcess(_T("C:\\Windows\\Notepad.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

UINT Random(UINT max)
{
	return rand() % (max + 1);
}

COLORREF GetRandomColor()
{
	return RGB(Random(255), Random(255), Random(255));
}

void OnClicked(HWND hwnd, UINT x, UINT y, UINT value)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	x = x * dim / rect.right;
	y = y * dim / rect.bottom;
	UINT index = y * dim + x;
	circles[index] = value;

	InvalidateRect(hwnd, NULL, true);
	UpdateWindow(hwnd);
}

void DrawCircle(HDC hdc, UINT radius, UINT centerX, UINT centerY)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	HGDIOBJ prevBrush = SelectObject(hdc, hBrush);

	Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

	SelectObject(hdc, prevBrush);
	DeleteObject(hBrush);
}

void DrawCross(HDC hdc, UINT radius, UINT centerX, UINT centerY)
{
	// Draws '\'
	MoveToEx(hdc, centerX - radius, centerY - radius, NULL);
	LineTo(hdc, centerX + radius, centerY + radius);
	// Draws '/'
	MoveToEx(hdc, centerX + radius, centerY - radius, NULL);
	LineTo(hdc, centerX - radius, centerY + radius);
}

void CalculateIconDimensions(HWND hwnd, UINT i, UINT& radius, UINT& centerX, UINT& centerY)
{
	UINT row = i % dim;
	UINT col = i / dim;
	RECT rect;
	GetClientRect(hwnd, &rect);
	UINT height = rect.bottom;
	UINT width = rect.right;
	radius = min(height/dim, width/dim) / 3;
	centerX = width * (2 * row + 1) / (2 * dim);
	centerY = height * (2 * col + 1) / (2 * dim);
}

void DrawIconsOnGrid(HWND hwnd, HDC hdc)
{
	for (UINT i = 0u, len = dim * dim; i < len; ++i)
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
	HPEN pen = CreatePen(0, 2, prefs->GridColor);
	HGDIOBJ prevBrush = SelectObject(hdc, pen);

	for (UINT i = 1u; i < dim; ++i)
	{
		UINT x = rect.right * i / dim;
		MoveToEx(hdc, x, 0, NULL);
		LineTo(hdc, x, rect.bottom);

		UINT y = rect.bottom * i / dim;
		MoveToEx(hdc, 0, y, NULL);
		LineTo(hdc, rect.right, y);
	}

	DrawIconsOnGrid(hwnd, hdc);

	SelectObject(hdc, prevBrush);
	DeleteObject(pen);
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
				COLORREF NewColor = GetRandomColor();
				if (prefs)
				{
					prefs->BackgroundColor = NewColor;
				}
				HBRUSH hBrush = CreateSolidBrush(NewColor);
				SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
				DeleteObject(hCurrentBrush);
				hCurrentBrush = hBrush;

				InvalidateRect(hwnd, NULL, true);
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
			HDC hdc = BeginPaint(hwnd, &ps);
			DrawGrid(hwnd, hdc);
			EndPaint(hwnd, &ps);
		}
		break;
		case WM_LBUTTONDOWN:
		{
			UINT x = GET_X_LPARAM(lParam);
			UINT y = GET_Y_LPARAM(lParam);
			OnClicked(hwnd, x, y, 1);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			UINT x = GET_X_LPARAM(lParam);
			UINT y = GET_Y_LPARAM(lParam);
			OnClicked(hwnd, x, y, 2);
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main(int argc, char** argv)
{
	ReadingMethod method = GetReadingMethod(argc, argv);
	prefs = ReadConfigFile(method);
	if (prefs == NULL)
	{
		MessageBox(NULL, L"Failed to read config file!", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}
	dim = prefs->GridSize;

	UINT len = dim * dim;
	circles = new UINT[len];
	ZeroMemory(circles, len * sizeof circles);

	HINSTANCE hThisInstance = GetModuleHandle(NULL);

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hThisInstance;
	wc.lpszClassName = czWinClass;
	wc.lpfnWndProc = WindowProcedure;

	hCurrentBrush = CreateSolidBrush(prefs->BackgroundColor);
	wc.hbrBackground = hCurrentBrush;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Class registration has failed!", L"Error", MB_OK | MB_ICONERROR);
		DWORD error = GetLastError();
		printf("Error: %lu\n", error);
		return error;
	}

	HWND hwnd = CreateWindowEx(
		0,
		czWinClass,
		czWinName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		prefs->WindowWidth,
		prefs->WindowHeight,
		HWND_DESKTOP,
		NULL,
		hThisInstance,
		NULL
	);

	UINT nCmdShow = SW_SHOW;
	ShowWindow(hwnd, nCmdShow);

	BOOL bMessageOk;
	MSG message;
	while ((bMessageOk = GetMessage(&message, NULL, 0, 0)) != 0)
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
	UnregisterClass(czWinClass, hThisInstance);

	WriteConfigFile(method, prefs);
	prefs->Cleanup();
	delete prefs;
	delete circles;
	return 0;
}
