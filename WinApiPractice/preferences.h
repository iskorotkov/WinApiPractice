#pragma once

struct Preferences
{
	~Preferences();
	
	UINT WindowHeight;
	UINT WindowWidth;
	UINT GridSize;

	COLORREF BackgroundColor;
	COLORREF GridColor;

	TCHAR* IconFile = nullptr;
	TCHAR* CursorFile = nullptr;

	void Cleanup();
};

void InvertColor(COLORREF& color);

Preferences* StringToPreferences(const TCHAR* content);
const TCHAR* PreferencesToString(const Preferences* prefs);
