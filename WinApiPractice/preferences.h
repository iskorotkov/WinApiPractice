#pragma once

struct Preferences
{
	~Preferences();
	
	UINT WindowHeight;
	UINT WindowWidth;
	UINT GridSize;

	COLORREF BackgroundColor;
	COLORREF GridColor;

	TCHAR* IconFile = NULL;
	TCHAR* CursorFile = NULL;

	void Cleanup();
};

void InvertColor(COLORREF& color);

Preferences* StringToPreferences(TCHAR* content);
const TCHAR* PreferencesToString(Preferences* prefs);
