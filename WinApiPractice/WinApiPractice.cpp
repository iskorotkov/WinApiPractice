#include <Windows.h>
#include <tchar.h>
#include <cstdio>
#include <cstdlib>
#include <windowsx.h>
#include <WinUser.h>
#include "libpic.h"
#include <iostream>
#include <string>
#include "GameSession.h"
#include <memory>
#include "preferences.h"
#include "GraphicsThread.h"
#include "GameState.h"
#include "GameRules.h"

const TCHAR CZ_WIN_CLASS[] = _T("MyClassName");
const TCHAR CZ_WIN_NAME[] = _T("MyWindowName");

const std::wstring onClickMessagePrefix = L"OnClick message ";

HBRUSH hCurrentBrush;

// ReSharper disable once IdentifierTypo
unsigned WM_GRIDUPDATE = 0;

Image icon;
Image cursor;

std::unique_ptr<GameSession> gameSession;

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

void OnClicked(const HWND hwnd, UINT x, UINT y)
{
	const auto dimension = gameSession->GetPreferences()->GridSize;
	RECT rect;
	GetClientRect(hwnd, &rect);

	if (x > rect.right || y > rect.bottom)
	{
		return;
	}

	x = x * dimension / rect.right;
	y = y * dimension / rect.bottom;
	gameSession->GetRules()->StartTurn();
	gameSession->GetState()->SetAt(y, x, gameSession->GetRules()->GetOurSign());
	gameSession->GetRules()->FinishTurn();

	SendMessage(HWND_BROADCAST, WM_GRIDUPDATE, 0, 0);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (gameSession->IsStarted() && uMsg == gameSession->GetRules()->GetTurnMessageCode())
	{
		gameSession->GetRules()->RespondToTurnMessage(wParam, lParam);
	}
	const auto prefs = gameSession->GetPreferences();

	// TODO: Find better names.
	const auto firstKey = 0x31;
	const auto lastKey = 0x37;
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
				const auto newColor = GetRandomColor();
				if (prefs)
				{
					prefs->BackgroundColor = newColor;
				}
			}
			else if (wParam == VK_SPACE)
			{
				gameSession->GetGraphicsThread()->ToggleSuspended();
			}
			else if (wParam >= firstKey && wParam <= lastKey)
			{
				gameSession->GetGraphicsThread()->SetPriority(wParam - firstKey);
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
		case WM_LBUTTONDOWN:
		{
			const UINT x = GET_X_LPARAM(lParam);
			const UINT y = GET_Y_LPARAM(lParam);
			OnClicked(hwnd, x, y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const UINT x = GET_X_LPARAM(lParam);
			const UINT y = GET_Y_LPARAM(lParam);
			OnClicked(hwnd, x, y);
			break;
		}
		case WM_PAINT:
		case WM_ERASEBKGND:
			return 0;
		default: break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void RegisterUserMessages()
{
	WM_GRIDUPDATE = RegisterWindowMessage(L"WM_GRIDUPDATE");
}

int main(int argc, char** argv)
{
	RegisterUserMessages();

	gameSession = std::make_unique<GameSession>(argc, argv);
	const auto prefs = gameSession->GetPreferences();

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

	HWND hwnd = CreateWindowEx(
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

	gameSession->Start(hwnd);

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

	return 0;
}
