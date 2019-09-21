#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

const wchar_t* clName = _T("MyClassName");
const wchar_t* clWinName = _T("MyWindowName");

LRESULT CALLBACK WinProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(wnd, msg, wParam, lParam);
}

int main()
{
    HINSTANCE hThisInstance = GetModuleHandle(NULL);
	WNDCLASS wincl = { 0 };
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = clName;
	wincl.lpfnWndProc = WinProc;

	if (!RegisterClass(&wincl))
	{
		DWORD error = GetLastError();
		printf("Error: %ul\n", error);
		return error;
	}

	HWND wnd = CreateWindow(
		clName,
		clWinName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		320,
		240,
		HWND_DESKTOP,
		NULL,
		hThisInstance,
		NULL,
	);

	ShowWindow(wnd, SW_SHOW);

	BOOL bOk = FALSE;
	MSG msg;

	// PeekMessage - async
	while ((bOk = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bOk < 0)
		{
			// Process error
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(wnd);

	UnregisterClass(clName, hThisInstance);

	return msg.lParam;
}
