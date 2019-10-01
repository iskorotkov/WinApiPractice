#pragma once
#include <Windows.h>
#include <sstream>
#include <ios>
#include <iostream>
#include <iomanip>

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

inline void InvertColor(COLORREF& color)
{
	COLORREF copy = color;
	char* originalColor = (char*)&copy;
	char* invertedColor = (char*)&color;
	invertedColor[0] = originalColor[2];
	invertedColor[2] = originalColor[0];
}

inline Preferences* StringToPreferences(TCHAR* content)
{
	Preferences* prefs = new Preferences;
	std::wstringstream is(content);
	std::ios_base::fmtflags f(is.flags());

	is >> prefs->WindowWidth
	>> prefs->WindowHeight
	>> prefs->GridSize
	>> std::hex >> prefs->BackgroundColor
	>> std::hex >> prefs->GridColor;

	is.flags(f);

	InvertColor(prefs->BackgroundColor);
	InvertColor(prefs->GridColor);
	return prefs;
}

inline const TCHAR* PreferencesToString(Preferences* prefs)
{
	std::wstringstream os;
	std::ios_base::fmtflags f(os.flags());

	InvertColor(prefs->BackgroundColor);
	InvertColor(prefs->GridColor);

	os << prefs->WindowWidth << ' '
	<< prefs->WindowHeight << '\n'
	<< prefs->GridSize << '\n'
	<< std::hex << std::setfill(L'0') << std::setw(6)
	<< prefs->BackgroundColor << '\n'
	<< prefs->GridColor << '\n'
	// TODO: Can I really append \0 here?
	<< '\0';

	os.flags(f);

	UINT len = os.tellp();
	TCHAR* buffer = new TCHAR[len];
	os.read(buffer, len);
	return buffer;
}
