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
#include "GridPainter.h"
#include "LibraryHandle.h"

#define GRID_DIMENSION (prefs->GridSize)

const TCHAR CZ_WIN_CLASS[] = _T("MyClassName");
const TCHAR CZ_WIN_NAME[] = _T("MyWindowName");

HBRUSH hCurrentBrush;
int* values;

Preferences* prefs;
Image crossImage;
Image circleImage;
Image icon;
Image cursor;

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
	values[index] = value;

	InvalidateRect(hwnd, nullptr, true);
	UpdateWindow(hwnd);
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
			const GridPainter painter(hwnd, prefs->GridSize);
			painter.DrawGrid(prefs->GridColor);
			painter.DrawImageWhere(1, values, crossImage);
			painter.DrawImageWhere(2, values, circleImage);
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

	try
	{
		const LibraryHandle lib(L"libpic.dll");
		const auto loadPicFunc = reinterpret_cast<decltype(LoadPicW)*>(lib.GetMethod("LoadPicW"));
		const auto isValidFunc = reinterpret_cast<decltype(IsValid)*>(lib.GetMethod("IsValid"));

		const auto loadPicWrapper = [loadPicFunc, isValidFunc](Image& img, const wchar_t* name)
		{
			img = loadPicFunc(name);
			if (!isValidFunc(img))
			{
				throw std::exception("Unable to load a picture.");
			}
		};
		
		loadPicWrapper(crossImage, prefs->IconFile);
		loadPicWrapper(circleImage, prefs->CursorFile);
	}
	catch (std::exception& e)
	{
		printf_s("An error happened when tried to use DLL. Error info: %s", e.what());
		return -1;
	}

	const UINT len = GRID_DIMENSION * GRID_DIMENSION;
	values = new int[len];
	ZeroMemory(values, len * sizeof values);

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
	delete values;
	return 0;
}
