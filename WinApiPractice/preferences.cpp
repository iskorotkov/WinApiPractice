#include <Windows.h>
#include <sstream>
#include <ios>
#include <iostream>
#include <iomanip>
#include "preferences.h"
#include <tchar.h>

Preferences::~Preferences()
{
	Cleanup();
}

void Preferences::Cleanup()
{
	if (IconFile)
	{
		delete[] IconFile;
		IconFile = nullptr;
	}
	if (CursorFile)
	{
		delete[] CursorFile;
		CursorFile = nullptr;
	}
}

void InvertColor(COLORREF& color)
{
	COLORREF copy = color;
	std::byte* originalColor = (std::byte*)&copy;
	std::byte* invertedColor = (std::byte*)&color;
	invertedColor[0] = originalColor[2];
	invertedColor[2] = originalColor[0];
}

Preferences* StringToPreferences(const TCHAR* content)
{
	Preferences* prefs = new Preferences;
	std::wistringstream is(content);
	const std::ios_base::fmtflags f(is.flags());

	is >> prefs->WindowWidth
	>> prefs->WindowHeight
	>> prefs->GridSize
	>> std::hex >> prefs->BackgroundColor
	>> std::hex >> prefs->GridColor;

	InvertColor(prefs->BackgroundColor);
	InvertColor(prefs->GridColor);
	is.flags(f);

	while (is.peek() == '\r' || is.peek() == '\n')
	{
		is.ignore();
	}

	const UINT bufferSize = 1024;

	// TODO: Fixed size buffer array.
	prefs->IconFile = new TCHAR[bufferSize];
	prefs->CursorFile = new TCHAR[bufferSize];
	is.getline(prefs->IconFile, bufferSize);
	is.getline(prefs->CursorFile, bufferSize);

	return prefs;
}

const TCHAR* PreferencesToString(const Preferences* prefs)
{
	std::wostringstream os;
	const std::ios_base::fmtflags f(os.flags());

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

	os.flags(f);

	os << prefs->IconFile << '\n'
	<< prefs->CursorFile << '\n';

	os << '\0';
	const UINT len = os.tellp();
	TCHAR* buffer = new TCHAR[len];
	wcscpy_s(buffer, len, os.str().c_str());
	return buffer;
}
