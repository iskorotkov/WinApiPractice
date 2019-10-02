#include <Windows.h>
#include <sstream>
#include <ios>
#include <iostream>
#include <iomanip>
#include "preferences.h"
#include <tchar.h>

const UINT BufferSize = 1024;

Preferences::~Preferences()
{
	Cleanup();
}

void Preferences::Cleanup()
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

void InvertColor(COLORREF& color)
{
	COLORREF copy = color;
	char* originalColor = (char*)&copy;
	char* invertedColor = (char*)&color;
	invertedColor[0] = originalColor[2];
	invertedColor[2] = originalColor[0];
}

Preferences* StringToPreferences(const TCHAR* content)
{
	Preferences* prefs = new Preferences;
	std::wistringstream is(content);
	std::ios_base::fmtflags f(is.flags());

	is >> prefs->WindowWidth
	>> prefs->WindowHeight
	>> prefs->GridSize
	>> std::hex >> prefs->BackgroundColor
	>> std::hex >> prefs->GridColor;

	InvertColor(prefs->BackgroundColor);
	InvertColor(prefs->GridColor);
	is.flags(f);

	is.ignore();
	prefs->IconFile = new TCHAR[BufferSize];
	prefs->CursorFile = new TCHAR[BufferSize];
	is.getline(prefs->IconFile, BufferSize);
	is.getline(prefs->CursorFile, BufferSize);

	return prefs;
}

const TCHAR* PreferencesToString(const Preferences* prefs)
{
	std::wostringstream os;
	std::ios_base::fmtflags f(os.flags());

	// TODO: Add method that return inverted copy of color.
	COLORREF BgColor = prefs->BackgroundColor;
	COLORREF GrColor = prefs->GridColor;
	InvertColor(BgColor);
	InvertColor(GrColor);

	os << prefs->WindowWidth << ' '
	<< prefs->WindowHeight << '\n'
	<< prefs->GridSize << '\n'
	<< std::hex << std::setfill(L'0') << std::setw(6)
	<< BgColor << '\n'
	<< GrColor << '\n';
	// TODO: Can I really append \0 here?

	os.flags(f);

	os << prefs->IconFile << '\n'
	<< prefs->CursorFile << '\n';

	os << '\0';
	UINT len = os.tellp();
	TCHAR* buffer = new TCHAR[len];
	wcscpy_s(buffer, len, os.str().c_str());
	return buffer;
}
