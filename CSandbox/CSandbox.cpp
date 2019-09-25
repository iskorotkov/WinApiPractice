#include <Windows.h>
#include <tchar.h>
#include <cstdio>
#include <cstdlib>
#include <windowsx.h>
#include <WinUser.h>

const TCHAR czWinClass[] = _T("MyClassName");
const TCHAR czWinName[] = _T("MyWindowName");
const UINT dim = 3;

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
				HBRUSH hBrush = CreateSolidBrush(GetRandomColor());
				SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
				
				InvalidateRect(hwnd, NULL, true);
				UpdateWindow(hwnd);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT rect;
			GetClientRect(hwnd, &rect);
			HPEN pen = CreatePen(0, 2, RGB(127, 0, 0));
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

			for (UINT i = 0u, len = dim * dim; i < len; ++i)
			{
				if (circles[i])
				{
					UINT row = i % dim;
					UINT col = i / dim;
					RECT rect;
					GetClientRect(hwnd, &rect);
					UINT height = rect.bottom;
					UINT width = rect.right;
					UINT radius = min(height/dim, width/dim) / 3;
					UINT centerX = width * (2 * row + 1) / (2 * dim);
					UINT centerY = height * (2 * col + 1) / (2 * dim);
					switch (circles[i])
					{
						case 1:
						{
							HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
							HGDIOBJ prevBrush = SelectObject(hdc, hBrush);

							Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

							SelectObject(hdc, prevBrush);
							DeleteObject(hBrush);
							break;
						}
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

			SelectObject(hdc, prevBrush);
			EndPaint(hwnd, &ps);
			DeleteObject(pen);
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
	UINT len = dim * dim;
	circles = new UINT[len];
	ZeroMemory(circles, len * sizeof circles);

	BOOL bMessageOk;
	MSG message;

	WNDCLASSEX wc = { 0 };

	UINT nCmdShow = SW_SHOW;

	HINSTANCE hThisInstance = GetModuleHandle(NULL);

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	wc.hInstance = hThisInstance;
	wc.lpszClassName = czWinClass;
	wc.lpfnWndProc = WindowProcedure;

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 127));
	wc.hbrBackground = hBrush;

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
		320,
		240,
		HWND_DESKTOP,
		NULL,
		hThisInstance,
		NULL
	);

	ShowWindow(hwnd, nCmdShow);

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

	DestroyWindow(hwnd);
	UnregisterClass(czWinClass, hThisInstance);
	DeleteObject(hBrush);

	delete circles;
	return 0;
}
