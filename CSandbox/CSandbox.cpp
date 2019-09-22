#include <Windows.h>
#include <tchar.h>
#include <cstdio>
#include <cstdlib>
#include <windowsx.h>
#include <WinUser.h>

const TCHAR czWinClass[] = _T("MyClassName");
const TCHAR czWinName[] = _T("MyWindowName");
UINT dim = 5;
HWND hwnd;
HBRUSH hBrush;
WNDCLASSEX wc;

void RunNotepad()
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	ZeroMemory(&sInfo, sizeof(STARTUPINFO));

	puts("Starting Notepad...");
	CreateProcess(_T("C:\\Windows\\Notepad.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

void DrawGrid()
{
	PAINTSTRUCT ps;
	//auto hdc = BeginPaint(hwnd, &ps);
	auto hdc = GetDC(hwnd);

	RECT rect;
	GetClientRect(hwnd, &rect);
	auto pen = CreatePen(0, 4, RGB(127, 0, 0));
	auto prevBrush = SelectObject(hdc, pen);

	for (auto i = 1u; i < dim; ++i)
	{
		auto x = rect.right * i / dim;
		MoveToEx(hdc, x, 0, NULL);
		LineTo(hdc, x, rect.bottom);

		auto y = rect.bottom * i / dim;
		MoveToEx(hdc, 0, y, NULL);
		LineTo(hdc, rect.right, y);
	}

	SelectObject(hdc, prevBrush);
	//EndPaint(hwnd, &ps);
	DeleteObject(pen);
}

UINT Random(UINT max)
{
	return rand() % (max + 1);
}

void ChangeBG()
{
	auto hdc = GetDC(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);
	auto randomColor = RGB(Random(255), Random(255), Random(255));
	auto brush = CreateSolidBrush(randomColor);
	FillRect(hdc, &rect, brush);
	DeleteObject(brush);
	ReleaseDC(hwnd, hdc);

	DrawGrid();
}

void DrawCircle(UINT x, UINT y)
{
	PAINTSTRUCT ps;
	auto hdc = BeginPaint(hwnd, &ps);

	//RoundRect(hdc, 100, 100, 100, 100, 20, 20);
	RECT rect = { 0, 20, 40, 60 };
	HBRUSH brush = CreateSolidBrush(RGB(0, 127, 0));
	FillRect(hdc, &rect, brush);

	EndPaint(hwnd, &ps);
	DeleteObject(brush);
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
				ChangeBG();
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
			DrawGrid();
			break;
		case WM_LBUTTONDOWN:
			//DrawCircle(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main(int argc, char** argv)
{
	BOOL bMessageOk;
	MSG message;

	wc = { 0 };

	int nCmdShow = SW_SHOW;

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

	return 0;
}
