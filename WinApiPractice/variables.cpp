#include "variables.h"
#include <cstdio>
#include "preferences.h"
#include <tchar.h>

// TODO: Move BufferSize to a separate file.

Preferences* ReadConfigUsingVariables(const TCHAR* configFile)
{
	FILE* file;
	_wfopen_s(&file, configFile, L"r");
	if (file == NULL)
	{
		_tprintf(L"Error opening file using variables.");
	}

	fseek(file, 0, SEEK_END);
	size_t lSize = ftell(file) / sizeof(wchar_t);
	rewind(file);

	TCHAR* buffer = new TCHAR[lSize];
	fread(buffer, sizeof(wchar_t), lSize, file);

	fclose(file);
	Preferences* prefs = StringToPreferences(buffer);

	delete[] buffer;
	return prefs;
}

void WriteConfigUsingVariables(const TCHAR* configFile, const Preferences* prefs)
{
	FILE* file;
	_wfopen_s(&file, configFile, L"wb");
	if (file == NULL)
	{
		_tprintf(L"Error opening file using variables.");
	}

	const TCHAR* buffer = PreferencesToString(prefs);
	const UINT len = wcslen(buffer);

	fwrite(buffer, sizeof(wchar_t), len, file);

	fclose(file);
	delete[] buffer;
}
