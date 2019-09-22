#include <Windows.h>
#include <tchar.h>
#include <cstdio>
#include <cstdlib>
#include <windowsx.h>
#include <WinUser.h>

const TCHAR czWinClass[] = _T("MyClassName");
const TCHAR czWinName[] = _T("MyWindowName");
UINT dim = 3;
HWND hwnd;
HBRUSH hBrush;
WNDCLASSEX wc;

UINT* circles;

void RunNotepad()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	ZeroMemory(&sInfo, sizeof(STARTUPINFO));

	puts("Starting Notepad...");
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

void OnClicked(UINT x, UINT y, UINT value)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	x = x * dim / rect.right;
	y = y * dim / rect.bottom;
	auto index = y * dim + x;
	circles[index] = value;

	InvalidateRect(hwnd, NULL, true);
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
			if (wParam == 'C')
			{
				if (GetKeyState(VK_SHIFT) & 0x8000)
				{
					RunNotepad();
				}
			}
			else if (wParam == VK_RETURN)
			{
				DeleteObject(hBrush);
				hBrush = CreateSolidBrush(GetRandomColor());
				SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
				InvalidateRect(hwnd, NULL, true);
				UpdateWindow(hwnd);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			// Draw grid
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT rect;
			GetClientRect(hwnd, &rect);
			HPEN pen = CreatePen(0, 2, RGB(127, 0, 0));
			HGDIOBJ brush = SelectObject(hdc, pen);

			for (auto i = 1u; i < dim; ++i)
			{
				auto x = rect.right * i / dim;
				MoveToEx(hdc, x, 0, NULL);
				LineTo(hdc, x, rect.bottom);

				auto y = rect.bottom * i / dim;
				MoveToEx(hdc, 0, y, NULL);
				LineTo(hdc, rect.right, y);
			}

			for (auto i = 0u, len = dim * dim; i < len; ++i)
			{
				if (circles[i])
				{
					auto row = i % dim;
					auto col = i / dim;
					RECT rect;
					GetClientRect(hwnd, &rect);
					auto height = rect.bottom;
					auto width = rect.right;
					auto radius = min(height/dim, width/dim) / 3;
					auto centerX = width * (2 * row + 1) / (2 * dim);
					auto centerY = height * (2 * col + 1) / (2 * dim);
					switch (circles[i])
					{
						case 1:
							Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
							break;
						case 2:
						{
							// Draws '\'
							MoveToEx(hdc, centerX - radius, centerY - radius, NULL);
							LineTo(hdc, centerX + radius, centerY + radius);
							// Draws '/'
							MoveToEx(hdc, centerX + radius, centerY - radius, NULL);
							LineTo(hdc, centerX - radius, centerY + radius);
							break;
						}
					}
				}
			}

			SelectObject(hdc, brush);
			EndPaint(hwnd, &ps);
			DeleteObject(pen);
		}
		break;
		case WM_LBUTTONDOWN:
		{
			auto x = GET_X_LPARAM(lParam);
			auto y = GET_Y_LPARAM(lParam);
			OnClicked(x, y, 1);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			auto x = GET_X_LPARAM(lParam);
			auto y = GET_Y_LPARAM(lParam);
			OnClicked(x, y, 2);
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main(int argc, char** argv)
{
	auto len = dim * dim;
	circles = new UINT[len];
	ZeroMemory(circles, len * sizeof(BOOL));

	BOOL bMessageOk;
	MSG message;

	wc = { 0 };

	UINT nCmdShow = SW_SHOW;

	HINSTANCE hThisInstance = GetModuleHandle(NULL);

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	wc.hInstance = hThisInstance;
	wc.lpszClassName = czWinClass;
	wc.lpfnWndProc = WindowProcedure;

	hBrush = CreateSolidBrush(RGB(0, 0, 127));
	wc.hbrBackground = hBrush;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Class registration has failed!", L"Error", MB_OK | MB_ICONERROR);
		DWORD error = GetLastError();
		printf("Error: %lu\n", error);
		return error;
	}

	hwnd = CreateWindowEx(
		0,
		czWinClass,
		czWinName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		320,
		240,
		HWND_DESKTOP,
		NULL,
		hThisInstance,
		NULL
	);

	ShowWindow(hwnd, nCmdShow);

	// PeekMessage - async
	while ((bMessageOk = GetMessage(&message, NULL, 0, 0)) != 0)
	{
		if (bMessageOk < 0)
		{
			puts("Suddenly, GetMessage failed! You can call GetLastError() to see what happened");
			break;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	DestroyWindow(hwnd);
	UnregisterClass(czWinClass, hThisInstance);
	DeleteObject(hBrush);

	delete circles;
	return 0;
}
