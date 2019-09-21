#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

const TCHAR czWinClass[] = _T("MyClassName");
const TCHAR czWinName[] = _T("MyWindowName");
HWND hwnd;
HBRUSH hBrush;

void RunNotepad(void)
{
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	ZeroMemory(&sInfo, sizeof(STARTUPINFO));

	puts("Starting Notepad...");
	CreateProcess(_T("C:\\Windows\\Notepad.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

int main(int argc, char** argv)
{
	BOOL bMessageOk;
	MSG message;

	WNDCLASSEX wincl = { 0 };

	int nCmdShow = SW_SHOW;

	HINSTANCE hThisInstance = GetModuleHandle(NULL);

	wincl.cbSize = sizeof(wincl);
	wincl.style = CS_HREDRAW | CS_VREDRAW;
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = czWinClass;
	wincl.lpfnWndProc = WindowProcedure;

	hBrush = CreateSolidBrush(RGB(255,255,255));
	wincl.hbrBackground = hBrush;

	if (!RegisterClassEx(&wincl))
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

	RunNotepad();

	return 0;
}
