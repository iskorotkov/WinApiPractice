#pragma once
#include <Windows.h>

struct Preferences
{
	UINT WindowHeight;
	UINT WindowWidth;
	UINT GridSize;

	COLORREF BackgroundColor;
	COLORREF GridColor;

	TCHAR* IconFile = NULL;
	TCHAR* CursorFile = NULL;

	void Cleanup();
};

inline void Preferences::Cleanup()
{
	if (IconFile)
	{
		delete[] IconFile;
		IconFile = NULL;
	}
	if (CursorFile)
	{
		delete[] CursorFile;
		CursorFile = NULL;
	}
}
